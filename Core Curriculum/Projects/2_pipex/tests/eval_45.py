#!/usr/bin/env python3
"""Local Pipex evaluation rehearsal. Python 3 standard library only."""
import argparse
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tempfile
import time


class Skip(Exception):
    pass


class Tester:
    def __init__(self, options):
        self.options = options
        self.source = options.project.resolve()
        self.work = Path(tempfile.mkdtemp(prefix="pipex-eval-"))
        self.project = self.work / "project"
        self.cases = self.work / "cases"
        self.logs = self.work / "logs"
        self.results = []
        self.command_id = 0
        self.env = dict(os.environ, LC_ALL="C", PATH="/usr/bin:/bin")
        # Build a fresh copy: clean/re/fclean never touch the student's tree.
        shutil.copytree(self.source, self.project, ignore=shutil.ignore_patterns(
            ".git", "obj", "*.o", "*.a", "pipex", "__pycache__", "tests"))
        self.cases.mkdir()
        self.logs.mkdir()
        self.binary = str(self.project / "pipex")
        for name, data in {
            "input": b"apple a1\nbanana\napple a1\n", "empty": b"",
            "large": b"abc\n" * 300000, "input_two": b"a\nb\n",
            "unreadable": b"x", "denied": b"#!/bin/sh\nexit 0\n",
            "broken": b"#!/no/such/interpreter\n",
        }.items():
            (self.cases / name).write_bytes(data)
        (self.cases / "unreadable").chmod(0)
        (self.cases / "denied").chmod(0o644)
        (self.cases / "broken").chmod(0o755)
        (self.cases / "dir").mkdir()

    def run(self, command, data=b"", build=False):
        command = [str(arg) for arg in command]
        self.command_id += 1
        prefix = self.logs / f"{self.command_id:03d}"
        cwd = self.project if build else self.cases
        description = f"cwd: {cwd}\n$ {shlex.join(command)}\n"
        prefix.with_suffix(".command").write_text(description)
        prefix.with_suffix(".stdin").write_bytes(data)
        if self.options.verbose or self.options.demo:
            print("    $ " + shlex.join(command), flush=True)
            if data:
                print(f"      stdin: {data[:80]!r}" +
                      (f" ... ({len(data)} bytes)" if len(data) > 80 else ""))
        process = subprocess.Popen(command, cwd=cwd, env=self.env,
                                   stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, start_new_session=True)
        try:
            out, err = process.communicate(data, timeout=120 if build else 15)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            out, err = process.communicate()
            prefix.with_suffix(".stdout").write_bytes(out)
            prefix.with_suffix(".stderr").write_bytes(err)
            raise AssertionError(f"timeout; see {prefix}.command")
        except BaseException:
            os.killpg(process.pid, signal.SIGKILL)
            process.communicate()
            raise
        prefix.with_suffix(".stdout").write_bytes(out)
        prefix.with_suffix(".stderr").write_bytes(err)
        prefix.with_suffix(".status").write_text(str(process.returncode))
        return process.returncode, out, err

    def make(self, target="all"):
        result = self.run(["make", target], build=True)
        assert result[0] == 0, f"make {target} failed; see build logs"

    def check(self, name, action):
        number = len(self.results) + 1
        if self.options.demo:
            input(f"\n[{number:02d}/45] {name} — Enter to run (Ctrl-C to stop) ")
        start = time.monotonic()
        first_command = self.command_id + 1
        try:
            detail = action() or ""
            status = "PASS"
        except Skip as error:
            status, detail = "SKIP", str(error)
        except Exception as error:
            status, detail = "FAIL", f"{type(error).__name__}: {error}"
        self.results.append(dict(number=number, name=name, status=status,
                                 detail=detail, seconds=time.monotonic() - start,
                                 command_logs=[f"{n:03d}" for n in
                                               range(first_command, self.command_id + 1)]))
        print(f"[{number:02d}/45] {status:4} {name}" +
              (f" — {detail}" if detail else ""), flush=True)
        self.save()

    def save(self):
        (self.work / "results.json").write_text(json.dumps(self.results, indent=2))

    def binary_exists(self):
        assert Path(self.binary).is_file(), "pipex executable missing"

    def initial_build(self):
        self.make()
        self.binary_exists()

    def no_relink(self, mode):
        before = Path(self.binary).stat().st_mtime_ns
        self.make(mode)
        assert before == Path(self.binary).stat().st_mtime_ns, "executable relinked"

    def rebuild_missing(self):
        Path(self.binary).unlink()
        self.make()
        self.binary_exists()

    def bad_argc(self, count):
        code, _, err = self.run([self.binary] + ["dummy"] * count)
        assert 0 < code < 128 and err, f"expected usage error, got status {code}"
        return f"status {code}"

    def reset_outputs(self, kind="normal"):
        for name in ("actual", "expected"):
            path = self.cases / name
            if path.is_dir():
                path.rmdir()
            elif path.exists():
                path.chmod(0o644)
                path.unlink()
            if kind == "directory":
                path.mkdir()
            else:
                path.write_bytes(b"old data\n")
                path.chmod(0 if kind == "denied" else 0o644)

    def compare(self, infile, commands, kind="normal", expected_status=None):
        if os.geteuid() == 0 and (infile == "unreadable" or kind == "denied"):
            raise Skip("permission checks require a non-root user")
        self.reset_outputs(kind)
        actual = self.run([self.binary, infile, *commands, "actual"])
        script = f"< {shlex.quote(infile)} " + " | ".join(commands) + " > expected"
        expected = self.run(["bash", "--noprofile", "--norc", "-c", script])
        detail = f"status pipex/bash {actual[0]}/{expected[0]}"
        assert actual[0] == expected[0], detail
        if expected_status is not None:
            assert actual[0] == expected_status, detail
        assert actual[1] == expected[1], "stdout differs; " + detail
        assert bool(actual[2]) == bool(expected[2]), "stderr presence differs; " + detail
        if kind == "normal":
            assert (self.cases / "actual").read_bytes() == (self.cases / "expected").read_bytes(), "output bytes differ; " + detail
        return detail

    def concurrent(self):
        start = time.monotonic()
        result = self.run([self.binary, "input", "sleep 1", "sleep 1", "timing"])
        elapsed = time.monotonic() - start
        assert result[0] == 0 and 0.9 < elapsed < 1.8, f"{elapsed:.2f}s; expected about 1s (host load can affect timing)"
        return f"{elapsed:.2f}s"

    def side_effect(self):
        actual = self.run([self.binary, "input", "touch actual_marker", "cat", "dir"])
        expected = self.run(["bash", "-c", "< input touch expected_marker | cat > dir"])
        assert actual[0] == expected[0] == 1, "expected output failure status 1"
        assert all((self.cases / name).is_file() for name in
                   ("actual_marker", "expected_marker")), "marker missing"

    def heredoc(self, data, count):
        output = self.cases / "heredoc"
        output.write_bytes(b"existing\n")
        result = self.run([self.binary, "here_doc", "END", "cat", "wc -l", "heredoc"], data)
        assert result[0] == 0, f"status {result[0]}"
        assert output.read_bytes() == f"existing\n{count}\n".encode(), "incorrect append/limiter handling"
        return f"appended {count}"

    def clean(self, target):
        self.make(target)
        if target == "re":
            self.binary_exists()
        else:
            assert not (self.project / "obj").exists(), "objects remain"
            assert Path(self.binary).exists() == (target == "clean"), "wrong executable state"
            if target == "fclean":
                assert not (self.project / "libft/libft.a").exists(), "library archive remains"

    def many_pipes(self, count):
        self.make("bonus")
        return self.compare("input_two", ["cat"] * count + ["wc -l"], expected_status=0)

    def repeated_append(self):
        output = self.cases / "append"
        output.write_bytes(b"original\n")
        for _ in range(3):
            result = self.run([self.binary, "here_doc", "END", "cat", "wc -l", "append"], b"a\nEND\n")
            assert result[0] == 0, f"status {result[0]}"
        assert output.read_bytes() == b"original\n1\n1\n1\n", "append output differs"

    def memory(self, mode, args, data, status):
        if self.options.no_valgrind or not shutil.which("valgrind"):
            raise Skip("Valgrind disabled or unavailable; not counted as passed")
        self.make(mode)
        number = len(self.results) + 1
        prefix = f"valgrind-{number}-"
        result = self.run(["valgrind", "--leak-check=full", "--show-leak-kinds=all",
                           "--track-fds=yes", "--trace-children=yes", "--error-exitcode=99",
                           "--log-file=" + str(self.logs / (prefix + "%p.log")),
                           self.binary, *args], data)
        assert result[0] == status, f"status {result[0]}, expected {status}"
        files = list(self.logs.glob(prefix + "*.log"))
        assert len(files) >= 3, "missing parent/child Valgrind reports"
        for file in files:
            log = file.read_text()
            summaries = re.findall(r"ERROR SUMMARY: ([\d,]+)", log)
            assert summaries and all(int(x.replace(",", "")) == 0 for x in summaries), f"memory errors or incomplete report: {file.name}"
            assert not re.search(r"(?:definitely|indirectly|possibly) lost: [1-9]", log), f"lost memory: {file.name}"
        return f"{len(files)} fresh process reports; status {status}"

    def suite(self):
        self.check("Clean mandatory build", self.initial_build)
        self.check("Mandatory no relink", lambda: self.no_relink("all"))
        self.check("Rebuild deleted executable", self.rebuild_missing)
        for count in (0, 1, 2, 3, 5, 6):
            self.check(f"Argument count {count}", lambda n=count: self.bad_argc(n))
        cases = [
            ("Subject example", "input", ["grep a1", "wc -w"], "normal"),
            ("Normal pipeline", "input", ["cat", "wc -l"], "normal"),
            ("Empty input", "empty", ["cat", "wc -c"], "normal"),
            ("Large stream", "large", ["cat", "wc -c"], "normal"),
            ("Missing input", "missing", ["cat", "wc -c"], "normal"),
            ("Unreadable input", "unreadable", ["cat", "wc -c"], "normal"),
            ("Missing first command", "input", ["missing_987123", "wc -l"], "normal"),
            ("Missing last command", "input", ["cat", "missing_987123"], "normal"),
            ("Non-executable command", "input", ["cat", "./denied"], "normal"),
            ("Directory command", "input", ["cat", "./dir"], "normal"),
            ("Missing interpreter", "input", ["cat", "./broken"], "normal"),
            ("Output directory", "input", ["cat", "wc -l"], "directory"),
            ("Output permission denied", "input", ["cat", "wc -l"], "denied"),
            ("Single quotes", "input", ["grep 'apple a1'", "wc -l"], "normal"),
            ("Double quotes", "input", ['grep "apple a1"', "wc -l"], "normal"),
            ("Tab delimiter", "input", ["grep\ta1", "wc -l"], "normal"),
            ("Early reader exit", "input", ["yes", "head -n 1"], "normal"),
        ]
        for name, infile, commands, kind in cases:
            self.check(name, lambda f=infile, c=commands, k=kind: self.compare(f, c, k))
        self.check("Concurrent children", self.concurrent)
        self.check("Earlier command survives output failure", self.side_effect)
        self.check("Bonus build", lambda: self.make("bonus"))
        self.check("Bonus no relink", lambda: self.no_relink("bonus"))
        self.check("Multiple pipes", lambda: self.compare("input", ["cat", "grep a1", "wc -l"], expected_status=0))
        for name, data, count in [
            ("Normal heredoc", b"a\nb\nEND\nignored\n", 2),
            ("Heredoc EOF", b"a\n", 1),
            ("Limiter without newline", b"a\nEND", 1),
            ("Large heredoc", b"x\n" * 100000 + b"END\n", 100000),
        ]:
            self.check(name, lambda d=data, n=count: self.heredoc(d, n))
        for target in ("clean", "re", "fclean"):
            self.check(f"Make {target}", lambda t=target: self.clean(t))
        for count in (5, 20):
            self.check(f"{count} pipes", lambda n=count: self.many_pipes(n))
        self.check("Repeated heredoc append", self.repeated_append)
        for name, mode, args, data, status in [
            ("Missing last command", "all", ["input_two", "cat", "missing_93811", "vg-out"], b"", 127),
            ("Missing input", "all", ["missing", "cat", "wc -l", "vg-out"], b"", 0),
            ("Both commands missing", "all", ["input_two", "missing_93811", "missing_93812", "vg-out"], b"", 127),
            ("Heredoc commands missing", "bonus", ["here_doc", "END", "missing_93811", "missing_93812", "vg-out"], b"a\nEND\n", 127),
        ]:
            self.check("Valgrind: " + name, lambda m=mode, a=args, d=data, s=status: self.memory(m, a, d, s))
        assert len(self.results) == 45


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project", type=Path, default=Path(__file__).resolve().parents[1], help="Pipex source directory")
    parser.add_argument("--demo", action="store_true", help="pause before each check and show commands")
    parser.add_argument("--verbose", action="store_true", help="show commands without pausing")
    parser.add_argument("--no-valgrind", action="store_true", help="skip the four memory checks")
    options = parser.parse_args()
    if not __debug__:
        parser.error("run without -O or PYTHONOPTIMIZE; checks use assertions")
    if not (options.project / "Makefile").is_file():
        parser.error("project must contain a Makefile")
    missing = [name for name in ("make", "cc", "ar", "bash", "cat", "grep", "wc", "yes", "head", "sleep", "touch") if not shutil.which(name, path="/usr/bin:/bin")]
    if missing:
        parser.error("missing prerequisites: " + ", ".join(missing))
    if options.demo and not sys.stdin.isatty():
        parser.error("--demo needs an interactive terminal; use --verbose instead")
    tester = Tester(options)
    print(f"Pipex: 45-check local evaluation rehearsal\nSource: {tester.source}\nIsolated build, fixtures and logs: {tester.work}\n", flush=True)
    try:
        tester.suite()
    except KeyboardInterrupt:
        print(f"\nInterrupted. Partial results retained in {tester.work}")
        return 130
    counts = {s: sum(r["status"] == s for r in tester.results) for s in ("PASS", "FAIL", "SKIP")}
    print(f"\n{counts['PASS']}/45 passed; {counts['FAIL']} failed; {counts['SKIP']} skipped.")
    print(f"Report: {tester.work / 'results.json'}\nLogs: {tester.logs}")
    print("Local rehearsal only; Norm and the evaluator's additional cases are separate.")
    return 1 if counts["FAIL"] else (2 if counts["SKIP"] else 0)


if __name__ == "__main__":
    sys.exit(main())

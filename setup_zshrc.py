#!/usr/bin/env python3
"""Interactive, per-user setup for the shared 42 shell helpers."""
import argparse
from datetime import datetime
import getpass
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tempfile


def read_settings(path):
    """Read our simple assignments without executing a shell settings file."""
    values = {}
    if path.exists():
        for line in path.read_text().splitlines():
            parts = shlex.split(line, comments=True)
            if len(parts) == 2 and parts[0] == 'export' and '=' in parts[1]:
                key, value = parts[1].split('=', 1)
                values[key] = value
    return values


def write_settings(path, values):
    path = path.expanduser().resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    content = '# Personal 42 shell settings; shared repository updates never replace this file.\n'
    content += '# Run setupzshrc to reconfigure, or edit these values and source ~/.zshrc.\n'
    for key, value in values.items():
        if not re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', key):
            raise ValueError('Invalid setting name: ' + key)
        content += f'export {key}={shlex.quote(str(value))}\n'
    content += 'export ZSHRC_SETUP_DONE=1\n'
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode='w', dir=path.parent, delete=False) as output:
            temporary = Path(output.name)
            output.write(content)
        subprocess.run(['zsh', '-n', str(temporary)], check=True)
        if path.exists():
            backup = path.with_name(path.name + '.bak-' + datetime.now().strftime('%Y%m%d-%H%M%S-%f'))
            shutil.copy2(path, backup)
        os.replace(temporary, path)
    finally:
        if temporary:
            temporary.unlink(missing_ok=True)


def ask(label, default, validator=None):
    while True:
        value = input(f'{label} [{default}]: ').strip() or str(default)
        if validator is None or validator(value):
            return value
        print('Please enter a valid value.')


def yesno(label, default=False):
    value = ask(label + ' (yes/no)', 'yes' if default else 'no',
                lambda v: v.lower() in ('yes', 'no', 'y', 'n'))
    return 'TRUE' if value.lower() in ('yes', 'y') else 'FALSE'


def configure(shared_root, settings_path):
    previous = read_settings(settings_path)
    home = Path.home()
    values = dict(previous)
    values.pop('ZSHRC_SETUP_DONE', None)
    print('42 shell setup — settings are personal to this home folder.')
    print('Author example: repo ~/Documents/42_Singapore_SUTD;')
    print('  PROJECTS_ROOT="Core Curriculum/Projects", current_proj="2_pipex";')
    print('  mailbox ~/Documents/mac-remote; LAN42 ~/Documents/42sg-campus-lan;')
    print('  Stayon ~/Documents/42_Singapore_SUTD_hnah_private/Scripts and Testers/stayon.')
    print('Enter your own locations below. Paths may contain spaces; do not add quotes.')
    print('Optional services and automatic updates default to off on first setup.\n')

    def field(key, label, default, validator=None):
        values[key] = ask(label, previous.get(key, default), validator)
        return values[key]

    def location(key, label, default, required=False):
        def valid(value):
            return not required or Path(value).expanduser().is_dir()
        value = field(key, label, default, valid)
        values[key] = str(Path(value).expanduser().resolve())
        return values[key]

    def toggle(key, label, default=False):
        enabled = previous.get(key, str(default)).upper() in ('TRUE', '1')
        values[key] = yesno(label, enabled)
        return values[key] == 'TRUE'

    field('MAIL', 'Your campus email', getpass.getuser() + '@student.42singapore.sg')
    root = location('ZSHRC_SHARED_ROOT', 'Update clone (optional when automatic updates are off)', shared_root)
    main = location('MAIN_REPO_ROOT', 'Your project repository (may differ from the shared clone)', root)
    field('PROJECTS_ROOT', 'Project subfolder relative to that repository', 'Core Curriculum/Projects',
          lambda v: not Path(v).is_absolute() and '..' not in Path(v).parts)
    field('current_proj', 'Current project folder', '2_pipex',
          lambda v: not Path(v).is_absolute() and '..' not in Path(v).parts)
    location('DOCUMENTS_REPOS_ROOT', 'Folder scanned for other repositories', home / 'Documents')
    field('DAILY_TMUX_SESSION', 'Tmux workspace name', 'daily',
          lambda v: re.fullmatch(r'[A-Za-z0-9_-]+', v))
    values['ZSHRC_RUNTIME_ROOT'] = os.environ.get('ZSHRC_RUNTIME_ROOT', str(home / '.local/share/42-shell'))
    values['DAILY_TMUX_HELPER'] = str(Path(values['ZSHRC_RUNTIME_ROOT']) / 'daily_terminal.py')
    if toggle('autoupdatezshrc', 'Automatically pull the shared clone and refresh installed helpers on dailylogin'):
        if not all((Path(root) / name).is_file() for name in
                   ('sync_zshrc.py', 'setup_zshrc.py', 'daily_terminal.py', 'Useful .zshrc edits (addition)')):
            raise ValueError('Automatic updates require a clone containing the helper files; no settings saved.')
    toggle('pullotherrepos', 'Pull other repositories under DOCUMENTS_REPOS_ROOT on dailylogin')
    toggle('syncmainrepo', 'Run main-repo Git sync (stages ALL changes, asks for a commit, then pushes)')
    project = Path(main) / values['PROJECTS_ROOT'] / values['current_proj']
    print(f'Project destination: {project}' + ('' if project.is_dir() else ' (not present yet)'))
    toggle('openproject', 'Run make fclean and open VS Code in that project on dailylogin')
    toggle('openchrome', 'Open Chrome on dailylogin if it is not running')
    toggle('loginreports', 'Show local login-hour reports on dailylogin', True)
    toggle('startcodex', 'Start Codex (installs if missing and enables full access)')
    location('CODEX_INSTALL_DIR', 'Codex install directory', home / '.local/bin')
    location('CODEX_START_DIR', 'Codex working directory', main)
    toggle('startmailbox', 'Start your enrolled mailbox worker on dailylogin')
    location('MAC_REMOTE_ROOT', 'Your mac-remote clone (used when enabled)', home / 'Documents/mac-remote')
    toggle('startlan42', 'Start LAN42 on dailylogin')
    location('LAN42_CLONE', 'Your LAN42 clone (used when enabled)', home / 'Documents/42sg-campus-lan')
    toggle('startstayon', 'Start Stayon automatically on dailylogin')
    location('STAYON_ROOT', 'Your Stayon runner folder (used when enabled)', home / 'Documents/stayon')
    toggle('adjustbrightness', 'Adjust screen brightness on dailylogin')
    field('setbrightness', 'Brightness percentage', '100', lambda v: v.isdigit() and 0 <= int(v) <= 100)
    toggle('verbosity', 'Show workflow progress and results', True)
    field('verbosity_level', 'Reporting detail (1=progress, 2=also settings)', '2', lambda v: v in ('1', '2'))
    print('\nSettings to save:')
    for key, value in values.items():
        print(f'  {key}={value}')
    if yesno(f'Save to {settings_path}', True) != 'TRUE':
        print('Setup cancelled; no settings changed.')
        return False
    write_settings(settings_path, values)
    print('Setup saved. Run source ~/.zshrc in other open terminals to load these settings.')
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--settings', type=Path, default=Path.home() / '.42-shell-settings.zsh')
    args = parser.parse_args()
    try:
        if not configure(Path(os.environ.get('ZSHRC_SHARED_ROOT', str(Path.home() / 'Documents/42_Singapore_SUTD'))), args.settings.expanduser()):
            raise SystemExit(1)
    except (EOFError, KeyboardInterrupt):
        parser.exit(1, '\nSetup cancelled; no settings changed.\n')
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f'Setup failed: {error}\n')


if __name__ == '__main__':
    main()

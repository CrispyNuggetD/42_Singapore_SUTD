# Repository tools

The Python helper sources live here; tests live in `../tests/`. Project folders
and personal settings have not moved. Daily terminal commands keep their names.

## Shell installation and updates

From the repository root:

```sh
python3 sync_zshrc.py --check
python3 sync_zshrc.py
```

`sync_zshrc.py` and `install_gobreak.py` at the root are stable entry points for
existing installations. Implementations live here. Sync installs the same flat
runtime in `~/.local/share/42-shell`; personal paths, settings and dailylogin
entry points stay unchanged. No temporary migration script or reset is needed.
The installer accepts both the new checkout and older flat helper layouts.
Existing standalone sparse Go Break clones add `/tools/` when the root entry
point needs it; other sparse selections are preserved.

## Portfolio

```sh
python3 tools/portfolio.py refresh --from-shell
python3 tools/portfolio.py build
```

See [portfolio maintenance](../portfolio/README.md) for the scoped owner API
snapshot command and website sources. No API refresh belongs in dailylogin.

## Checks

```sh
PYTHONPATH=tools python3 -m unittest discover -s tests
PYTHONPATH=tools python3 -m unittest discover -s 'Shell automations/gobreak'
PYTHONPATH=tools python3 -m unittest discover -s 'Shell automations/apps'
```

Maintain source paths when adding helpers, but keep the installed runtime flat.
`sync_zshrc.py` owns that mapping. Never change personal settings to reorganize
source files. The two small root compatibility entry points are intentional.

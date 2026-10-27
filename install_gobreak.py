#!/usr/bin/env python3
"""Stable entry point; implementation and helper sources live in tools/."""
from pathlib import Path
import subprocess
import sys

_root=Path(__file__).resolve().parent
_tools=_root/'tools'
_entry=_tools/Path(__file__).name
if not _entry.exists() and (_root/'.git').exists():
    # Existing standalone sparse installs need the relocated helper sources.
    subprocess.run(['git','-C',str(_root),'sparse-checkout','add','/tools/'],check=True)
sys.path.insert(0,str(_tools))
__file__=str(_entry)
exec(compile(_entry.read_bytes(),__file__,'exec'),globals())

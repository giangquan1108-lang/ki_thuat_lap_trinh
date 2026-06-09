"""
Entry point for the Library Management System console application.

Usage:
    python -m lb_management.main
    or
    python LibraryManagementSystem/lb_management/main.py
"""

from __future__ import annotations

import sys
import os

# Ensure the project root is on sys.path so that
# `lb_management` is importable when run as a script.
_PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if _PROJECT_ROOT not in sys.path:
    sys.path.insert(0, _PROJECT_ROOT)

from lb_management.managers.library_manager import LibraryManager
from lb_management.ui.menu import run


def main() -> None:
    """Initialize the LibraryManager and start the main menu loop."""
    # data directory relative to the project root
    data_dir = os.path.join(_PROJECT_ROOT, "data")

    lm = LibraryManager(data_dir=data_dir)
    run(lm)


if __name__ == "__main__":
    main()
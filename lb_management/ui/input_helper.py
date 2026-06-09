"""
Input helper utilities for the console UI.

Provides reusable functions for reading and validating user input.
"""

from __future__ import annotations

from typing import Any, Callable, Optional, TypeVar

T = TypeVar("T")


def read_non_empty(prompt: str) -> str:
    """Read a non-empty string from the user.

    Re-prompts until a non-whitespace value is entered.

    Args:
        prompt: The prompt to display.

    Returns:
        The trimmed input string.
    """
    while True:
        value = input(prompt).strip()
        if value:
            return value
        print("  Input cannot be empty. Please try again.")


def read_int(prompt: str, min_val: Optional[int] = None, max_val: Optional[int] = None) -> int:
    """Read an integer from the user with optional range validation.

    Args:
        prompt: The prompt to display.
        min_val: Minimum acceptable value (inclusive).
        max_val: Maximum acceptable value (inclusive).

    Returns:
        The validated integer.
    """
    while True:
        try:
            value = int(input(prompt).strip())
        except ValueError:
            print("  Invalid number. Please enter an integer.")
            continue

        if min_val is not None and value < min_val:
            print(f"  Value must be at least {min_val}. Please try again.")
            continue
        if max_val is not None and value > max_val:
            print(f"  Value must be at most {max_val}. Please try again.")
            continue
        return value


def read_float(prompt: str, min_val: Optional[float] = None) -> float:
    """Read a float from the user with optional minimum.

    Args:
        prompt: The prompt to display.
        min_val: Minimum acceptable value (inclusive).

    Returns:
        The validated float.
    """
    while True:
        try:
            value = float(input(prompt).strip())
        except ValueError:
            print("  Invalid number. Please enter a number.")
            continue

        if min_val is not None and value < min_val:
            print(f"  Value must be at least {min_val}. Please try again.")
            continue
        return value


def read_choice(prompt: str, valid_choices: list[str]) -> str:
    """Read a choice that must be one of the valid options (case-insensitive).

    Args:
        prompt: The prompt to display.
        valid_choices: List of acceptable strings.

    Returns:
        The matched choice (as entered, may differ in case).
    """
    lower_map = {c.lower(): c for c in valid_choices}
    while True:
        value = input(prompt).strip()
        low = value.lower()
        if low in lower_map:
            return lower_map[low]
        print(f"  Invalid choice. Valid options: {', '.join(valid_choices)}")


def read_optional(prompt: str, default: str = "") -> str:
    """Read an optional string — empty input returns *default*.

    Args:
        prompt: The prompt to display.
        default: Value to return when input is empty.

    Returns:
        User input or *default*.
    """
    value = input(prompt).strip()
    return value if value else default


def confirm(prompt: str = "Are you sure? (y/n): ") -> bool:
    """Ask a yes/no confirmation.

    Args:
        prompt: The confirmation prompt.

    Returns:
        ``True`` for 'y'/'yes', ``False`` for 'n'/'no'.
    """
    yes = {"y", "yes"}
    no = {"n", "no"}
    while True:
        value = input(prompt).strip().lower()
        if value in yes:
            return True
        if value in no:
            return False
        print("  Please answer 'y' or 'n'.")


def press_enter_to_continue() -> None:
    """Pause execution until the user presses Enter."""
    input("\nPress Enter to continue...")
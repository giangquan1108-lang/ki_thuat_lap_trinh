"""
Reader domain model.

Represents a library patron / borrower.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

VALID_READER_TYPES: tuple[str, ...] = ("student", "teacher")
"""Allowed reader type values."""


@dataclass
class Reader:
    """A person registered with the library.

    Attributes:
        student_id: Unique identifier (student / staff ID).
        full_name: Full legal name.
        reader_type: ``'student'`` or ``'teacher'``.
        borrowed_count: Number of books currently checked out.
        is_blocked: Whether the reader's borrowing privileges
            are suspended.
        phone: Contact phone number.
        email: Contact email address.
    """

    student_id: str
    full_name: str
    reader_type: str = "student"
    borrowed_count: int = 0
    is_blocked: bool = False
    phone: str = ""
    email: str = ""

    def __post_init__(self) -> None:
        """Validate *reader_type* after dataclass initialisation."""
        if self.reader_type not in VALID_READER_TYPES:
            raise ValueError(
                f"Invalid reader_type '{self.reader_type}'. "
                f"Must be one of {VALID_READER_TYPES}."
            )

    # ------------------------------------------------------------------
    # Display
    # ------------------------------------------------------------------

    def display_info(self) -> str:
        """Return a human-readable string summarising the reader.

        Returns:
            Formatted string with all reader attributes.
        """
        return (
            f"Reader[ID={self.student_id}, Name='{self.full_name}', "
            f"Type='{self.reader_type}', "
            f"Borrowed={self.borrowed_count}, "
            f"Blocked={'Yes' if self.is_blocked else 'No'}, "
            f"Phone='{self.phone}', Email='{self.email}']"
        )

    # ------------------------------------------------------------------
    # Borrowing checks
    # ------------------------------------------------------------------

    def can_borrow(self, limit: int) -> bool:
        """Determine whether the reader may borrow another book.

        Args:
            limit: Maximum number of concurrent borrows allowed.

        Returns:
            ``True`` if the reader is **not** blocked and their current
            borrow count is strictly less than *limit*.
        """
        return not self.is_blocked and self.borrowed_count < limit

    # ------------------------------------------------------------------
    # Count management
    # ------------------------------------------------------------------

    def increase_borrow_count(self) -> None:
        """Increment the number of books currently checked out."""
        self.borrowed_count += 1

    def decrease_borrow_count(self) -> None:
        """Decrement the borrowed count (never below zero).

        Raises:
            ValueError: If the count is already 0.
        """
        if self.borrowed_count <= 0:
            raise ValueError(
                "Cannot decrease borrow count below zero."
            )
        self.borrowed_count -= 1

    # ------------------------------------------------------------------
    # Block / unblock
    # ------------------------------------------------------------------

    def block_reader(self) -> None:
        """Revoke the reader's borrowing privileges."""
        self.is_blocked = True

    def unblock_reader(self) -> None:
        """Restore the reader's borrowing privileges."""
        self.is_blocked = False

    # ------------------------------------------------------------------
    # Serialization
    # ------------------------------------------------------------------

    def to_dict(self) -> dict[str, Any]:
        """Serialize the reader to a plain dictionary suitable for JSON.

        Returns:
            Dictionary representation of the reader.
        """
        return {
            "student_id": self.student_id,
            "full_name": self.full_name,
            "reader_type": self.reader_type,
            "borrowed_count": self.borrowed_count,
            "is_blocked": self.is_blocked,
            "phone": self.phone,
            "email": self.email,
        }

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> Reader:
        """Construct a ``Reader`` instance from a dictionary.

        Args:
            data: Dictionary obtained from a previous ``to_dict()`` call
                  or from a JSON file.

        Returns:
            A fully-populated ``Reader`` object.
        """
        return cls(
            student_id=data["student_id"],
            full_name=data.get("full_name", ""),
            reader_type=data.get("reader_type", "student"),
            borrowed_count=data.get("borrowed_count", 0),
            is_blocked=data.get("is_blocked", False),
            phone=data.get("phone", ""),
            email=data.get("email", ""),
        )
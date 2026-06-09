"""
Book domain model.

Represents a single book title in the library system.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

VALID_STATUSES: tuple[str, ...] = ("available", "unavailable", "lost")
"""Valid book status values."""


@dataclass
class Book:
    """A book title held by the library.

    Attributes:
        id: Unique identifier for this book.
        title: Book title.
        author: Author name(s).
        category: Genre or subject category.
        quantity: Number of physical copies currently available.
        borrow_count: Total number of times this book has been borrowed.
        status: Current availability status.
    """

    id: str
    title: str
    author: str
    category: str
    quantity: int = 1
    borrow_count: int = 0
    status: str = "available"

    # ------------------------------------------------------------------
    # Display
    # ------------------------------------------------------------------

    def display_info(self) -> str:
        """Return a human-readable string summarising the book.

        Returns:
            Formatted string with all book attributes.
        """
        return (
            f"Book[ID={self.id}, Title='{self.title}', "
            f"Author='{self.author}', Category='{self.category}', "
            f"Quantity={self.quantity}, Borrowed={self.borrow_count}, "
            f"Status='{self.status}']"
        )

    # ------------------------------------------------------------------
    # Mutators
    # ------------------------------------------------------------------

    def update_quantity(self, amount: int) -> None:
        """Adjust the quantity of available copies.

        Args:
            amount: Positive value to add copies, negative to remove.

        Raises:
            ValueError: If the resulting quantity would be negative.
        """
        new_qty = self.quantity + amount
        if new_qty < 0:
            raise ValueError(
                f"Cannot set quantity to {new_qty}: "
                f"current={self.quantity}, amount={amount}"
            )
        self.quantity = new_qty

    def change_status(self, status: str) -> None:
        """Update the book's availability status.

        Args:
            status: One of 'available', 'unavailable', or 'lost'.

        Raises:
            ValueError: If the supplied status is not recognised.
        """
        if status not in VALID_STATUSES:
            raise ValueError(
                f"Invalid status '{status}'.  "
                f"Must be one of {VALID_STATUSES}."
            )
        self.status = status

    def is_available(self) -> bool:
        """Check whether the book can currently be borrowed.

        Returns:
            ``True`` if quantity > 0 **and** status is 'available'.
        """
        return self.quantity > 0 and self.status == "available"

    def increase_borrow_count(self) -> None:
        """Increment the total borrow counter by one."""
        self.borrow_count += 1

    # ------------------------------------------------------------------
    # Serialization
    # ------------------------------------------------------------------

    def to_dict(self) -> dict[str, Any]:
        """Serialize the book to a plain dictionary suitable for JSON.

        Returns:
            Dictionary representation of the book.
        """
        return {
            "id": self.id,
            "title": self.title,
            "author": self.author,
            "category": self.category,
            "quantity": self.quantity,
            "borrow_count": self.borrow_count,
            "status": self.status,
        }

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> Book:
        """Construct a ``Book`` instance from a dictionary.

        Args:
            data: Dictionary obtained from a previous ``to_dict()`` call
                  or from a JSON file.

        Returns:
            A fully-populated ``Book`` object.
        """
        return cls(
            id=data["id"],
            title=data.get("title", ""),
            author=data.get("author", ""),
            category=data.get("category", ""),
            quantity=data.get("quantity", 1),
            borrow_count=data.get("borrow_count", 0),
            status=data.get("status", "available"),
        )
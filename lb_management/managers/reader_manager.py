"""
ReaderManager – Manages library patrons (readers).

Provides CRUD operations and hash-index lookups.
"""

from __future__ import annotations

from ..models.reader import Reader


class ReaderManager:
    """Manages the collection of ``Reader`` objects with O(1) ID lookups.

    Responsibilities:
        - Add / remove readers.
        - Find readers by student ID (hash index).
    """

    def __init__(self) -> None:
        self.readers: list[Reader] = []
        self.reader_index: dict[str, Reader] = {}

    # ------------------------------------------------------------------
    # Index helpers
    # ------------------------------------------------------------------

    def _rebuild_index(self) -> None:
        """Rebuild the reader hash index from the current list."""
        self.reader_index.clear()
        for reader in self.readers:
            self.reader_index[reader.student_id] = reader

    # ------------------------------------------------------------------
    # CRUD
    # ------------------------------------------------------------------

    def add_reader(self, reader: Reader) -> None:
        """Register a new reader.

        Args:
            reader: The ``Reader`` instance to add.

        Raises:
            ValueError: If a reader with the same ID already exists.
        """
        if reader.student_id in self.reader_index:
            raise ValueError(
                f"Reader '{reader.student_id}' already exists."
            )
        self.readers.append(reader)
        self.reader_index[reader.student_id] = reader

    def remove_reader(self, reader_id: str) -> None:
        """Remove a reader from the system.

        Args:
            reader_id: ID of the reader to remove.

        Raises:
            KeyError: If the reader is not found.
        """
        reader = self.find_reader(reader_id)
        self.readers.remove(reader)
        del self.reader_index[reader_id]

    # ------------------------------------------------------------------
    # Query
    # ------------------------------------------------------------------

    def find_reader(self, reader_id: str) -> Reader:
        """Look up a reader by ID (O(1) via hash index).

        Args:
            reader_id: Unique reader identifier.

        Returns:
            The matching ``Reader`` instance.

        Raises:
            KeyError: If no reader with *reader_id* exists.
        """
        if reader_id not in self.reader_index:
            raise KeyError(f"Reader '{reader_id}' not found.")
        return self.reader_index[reader_id]
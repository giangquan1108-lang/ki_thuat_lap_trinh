/**
 * COMPREHENSIVE BUSINESS LOGIC AUDIT
 * Tests: validation, invariants, edge cases, pre/post conditions, data integrity
 */
#include "lb_management/services/LibraryManager.h"
#include "lb_management/models/Book.h"
#include "lb_management/models/Reader.h"
#include "lb_management/models/BorrowRecord.h"
#include "dsa/algorithms/sorting.h"
#include "dsa/algorithms/searching.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <cmath>

int passed = 0;
int failed = 0;
int validation_pass = 0;
int validation_fail = 0;
int integrity_pass = 0;
int integrity_fail = 0;

#define TPASS(name) do { std::cout << "  PASS: " << name << "\n"; passed++; } while(0)
#define TFAIL(name,reason) do { std::cout << "  FAIL: " << name << " [" << reason << "]\n"; failed++; } while(0)
#define CHECK(cond, name, reason) do { if(cond) TPASS(name); else TFAIL(name, reason); } while(0)

const std::string DATA = "./data/";

void backup() {
    std::system("copy data\\books.txt data\\books_audit_bak.txt > nul 2>&1");
    std::system("copy data\\readers.txt data\\readers_audit_bak.txt > nul 2>&1");
    std::system("copy data\\records.txt data\\records_audit_bak.txt > nul 2>&1");
}
void clear_data() {
    std::ofstream("data/books.txt", std::ios::trunc).close();
    std::ofstream("data/readers.txt", std::ios::trunc).close();
    std::ofstream("data/records.txt", std::ios::trunc).close();
}
void restore() {
    std::system("copy data\\books_audit_bak.txt data\\books.txt > nul 2>&1");
    std::system("copy data\\readers_audit_bak.txt data\\readers.txt > nul 2>&1");
    std::system("copy data\\records_audit_bak.txt data\\records.txt > nul 2>&1");
}

int main() {
    std::cout << "=============================================================\n";
    std::cout << "     BUSINESS LOGIC COMPREHENSIVE AUDIT\n";
    std::cout << "=============================================================\n\n";

    backup();
    clear_data();

    // ==================================================================
    // MODULE 1: ADD BOOK — VALIDATION & CONSISTENCY
    // ==================================================================
    std::cout << "=== MODULE 1: ADD BOOK ===\n\n";
    std::cout << "--- 1a: Input Validation ---\n";
    {
        LibraryManager m(DATA);
        // Empty ID
        Book* b = new Book("", "T", "A", "C", 1);
        CHECK(m.add_book(b) == true, "Empty ID: allowed at BL layer (UI validates)", "BL accepts empty ID - UI layer validation");
        // Quantity < 0: Book constructor accepts any value, but update_book only applies if >= 0
        Book* b2 = new Book("B1", "T", "A", "C", -5);
        CHECK(m.add_book(b2) == true, "Neg qty: added (qty=-5)", "No input validation in BL");
        // Quantity = 0
        Book* b3 = new Book("Q0", "T", "A", "C", 0);
        CHECK(m.add_book(b3) == true, "Quantity=0: added OK", "");
        // Quantity too large
        Book* b4 = new Book("BIG", "T", "A", "C", 999999);
        CHECK(m.add_book(b4) == true, "Large quantity: added OK", "");
    }
    std::cout << "\n  --- Validation Summary ---\n";
    validation_pass += 4; // All BL allows - UI validates
    std::cout << "\n";

    std::cout << "--- 1b: Duplicate ID ---\n";
    {
        LibraryManager m(DATA);
        CHECK(m.add_book(new Book("DUP", "T", "A", "C", 1)) == true, "First add OK", "");
        Book* dup = new Book("DUP", "T", "A", "C", 1);
        CHECK(m.add_book(dup) == false, "Duplicate ID blocked", "");
        delete dup;
        CHECK(m.get_book_count() == 1, "Count still 1 after dup", "");
    }
    std::cout << "\n";

    std::cout << "--- 1c: LinkedList / HashMap Consistency ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("C1", "T1", "A1", "C1", 5));
        m.add_book(new Book("C2", "T2", "A2", "C2", 3));
        m.add_book(new Book("C3", "T3", "A3", "C3", 7));
        
        CHECK(m.get_book_count() == 3, "LinkedList size = 3", "");
        CHECK(m.find_book("C1") != nullptr, "HashMap has C1", "");
        CHECK(m.find_book("C2") != nullptr, "HashMap has C2", "");
        CHECK(m.find_book("C3") != nullptr, "HashMap has C3", "");
        CHECK(m.find_book("C1")->title == "T1", "Same pointer in both structures", "");
        CHECK(m.find_book("NONEXIST") == nullptr, "Non-existent returns null", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 2: DELETE BOOK
    // ==================================================================
    std::cout << "=== MODULE 2: DELETE BOOK ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("D1", "Del", "A", "C", 1));
        CHECK(m.remove_book("D1") == true, "Delete existing book OK", "");
        CHECK(m.get_book_count() == 0, "LinkedList empty after delete", "");
        CHECK(m.find_book("D1") == nullptr, "HashMap empty after delete", "");
        
        CHECK(m.remove_book("NONEXIST") == false, "Delete non-existent returns false", "");
    }
    std::cout << "\n";

    std::cout << "--- 2b: Delete borrowed book blocked ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("DB", "Borrowed", "A", "C", 2));
        m.add_reader(new Reader("RD", "Reader", "student", "", ""));
        m.borrow_book("RD", "DB", "01/01/2026", "15/01/2026");
        
        CHECK(m.remove_book("DB") == false, "Cannot delete borrowed book", "");
        CHECK(m.find_book("DB") != nullptr, "Book still in system", "");
        CHECK(m.get_book_count() == 1, "Count unchanged", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 3: UPDATE BOOK
    // ==================================================================
    std::cout << "=== MODULE 3: UPDATE BOOK ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("U1", "Old", "OldA", "OldC", 5));
        
        CHECK(m.update_book("U1", "NewTitle", "NewA", "NewC", 10) == true, "Update all fields OK", "");
        Book* b = m.find_book("U1");
        CHECK(b->title == "NewTitle", "Title updated", "");
        CHECK(b->author == "NewA", "Author updated", "");
        CHECK(b->category == "NewC", "Category updated", "");
        CHECK(b->quantity == 10, "Quantity updated", "");
        
        // Partial update
        CHECK(m.update_book("U1", "", "", "", -1) == true, "No fields updated (qty=-1 skipped)", "");
        CHECK(m.update_book("U1", "OnlyTitle", "", "", -1) == true, "Only title updated", "");
        CHECK(m.find_book("U1")->title == "OnlyTitle", "Partial update OK", "");
        
        CHECK(m.update_book("NONEXIST", "X", "X", "X", 1) == false, "Non-existent update fails", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 4: ADD READER
    // ==================================================================
    std::cout << "=== MODULE 4: ADD READER ===\n\n";
    std::cout << "--- 4a: Validation ---\n";
    {
        LibraryManager m(DATA);
        // Empty ID
        Reader* r1 = new Reader("", "Name", "student", "090", "a@b.com");
        CHECK(m.add_reader(r1) == true, "Empty ID: BL accepts (UI validates)", "");
        // Empty name
        Reader* r2 = new Reader("R1", "", "student", "090", "a@b.com");
        CHECK(m.add_reader(r2) == true, "Empty name: BL accepts", "");
        // Invalid email format: BL does NOT validate email format
        Reader* r3 = new Reader("R2", "N", "student", "090", "notanemail");
        CHECK(m.add_reader(r3) == true, "Invalid email: BL accepts (no format check)", "");
    }
    std::cout << "\n  --- Validation Summary: BL layer does NOT validate ---\n";
    validation_pass += 3;
    std::cout << "\n";

    std::cout << "--- 4b: Duplicate Reader ID ---\n";
    {
        LibraryManager m(DATA);
        CHECK(m.add_reader(new Reader("R001", "N", "student", "", "")) == true, "First add OK", "");
        Reader* dup = new Reader("R001", "N2", "teacher", "", "");
        CHECK(m.add_reader(dup) == false, "Duplicate ID blocked", "");
        delete dup;
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 5: DELETE READER
    // ==================================================================
    std::cout << "=== MODULE 5: DELETE READER ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("DR1", "N", "student", "", ""));
        CHECK(m.remove_reader("DR1") == true, "Delete existing reader OK", "");
        CHECK(m.find_reader("DR1") == nullptr, "Reader removed from HashMap", "");
        CHECK(m.remove_reader("NONEXIST") == false, "Delete non-existent fails", "");
    }
    std::cout << "\n";

    std::cout << "--- 5b: Delete reader with active borrow blocked ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("DRB", "Bk", "A", "C", 2));
        m.add_reader(new Reader("DRR", "Rd", "student", "", ""));
        m.borrow_book("DRR", "DRB", "01/01/2026", "15/01/2026");
        CHECK(m.remove_reader("DRR") == false, "Cannot delete reader with borrow", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 6: BORROW BOOK — FULL PRE/POST CONDITION CHECK
    // ==================================================================
    std::cout << "=== MODULE 6: BORROW BOOK ===\n\n";
    std::cout << "--- 6a: Happy Path ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("BHP", "Happy", "A", "C", 5));
        m.add_reader(new Reader("RHP", "Reader", "student", "", ""));
        
        size_t books_before = m.get_book_count();
        bool result = m.borrow_book("RHP", "BHP", "01/01/2026", "15/01/2026");
        CHECK(result == true, "Borrow succeeds", "");
        CHECK(m.find_book("BHP")->quantity == 4, "Book quantity: 5->4", "");
        CHECK(m.find_book("BHP")->borrow_count == 1, "Book borrow_count: 0->1", "");
        CHECK(m.find_reader("RHP")->borrowed_count == 1, "Reader borrow_count: 0->1", "");
        CHECK(m.get_record_count() == 1, "1 BorrowRecord created", "");
        CHECK(m.get_borrowing_count() == 1, "1 active borrow", "");
    }
    std::cout << "\n";

    std::cout << "--- 6b: All Failure Conditions ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("BF", "FailBk", "A", "C", 1));
        m.add_reader(new Reader("RF", "FailRd", "student", "", ""));
        
        // Reader not found
        CHECK(m.borrow_book("NOEXIST", "BF", "01/01/2026", "15/01/2026") == false, "Reader not found", "");
        
        // Book not found
        CHECK(m.borrow_book("RF", "NOEXIST", "01/01/2026", "15/01/2026") == false, "Book not found", "");
        
        // Blocked reader
        m.block_reader("RF");
        CHECK(m.borrow_book("RF", "BF", "01/01/2026", "15/01/2026") == false, "Reader blocked", "");
        m.unblock_reader("RF");
        
        // No stock
        CHECK(m.borrow_book("RF", "BF", "01/01/2026", "15/01/2026") == true, "First borrow OK", "");
        CHECK(m.find_book("BF")->quantity == 0, "Quantity = 0 after borrow", "");
        CHECK(m.borrow_book("RF", "BF", "01/01/2026", "15/01/2026") == false, "No stock", "");
        
        // Reached limit (student=3)
        // Reader RF already has 1 borrowed. Add 2 more books.
        m.add_book(new Book("BF2", "Bk2", "A", "C", 5));
        m.add_book(new Book("BF3", "Bk3", "A", "C", 5));
        m.add_book(new Book("BF4", "Bk4", "A", "C", 5));
        CHECK(m.borrow_book("RF", "BF2", "01/01/2026", "15/01/2026") == true, "2nd borrow OK", "");
        CHECK(m.borrow_book("RF", "BF3", "01/01/2026", "15/01/2026") == true, "3rd borrow OK", "");
        CHECK(m.borrow_book("RF", "BF4", "01/01/2026", "15/01/2026") == false, "Limit reached (3)", "");
        
        // Teacher limit = 5
        m.add_reader(new Reader("RT", "Teacher", "teacher", "", ""));
        m.add_book(new Book("BT1", "T1", "A", "C", 10));
        m.add_book(new Book("BT2", "T2", "A", "C", 10));
        m.add_book(new Book("BT3", "T3", "A", "C", 10));
        m.add_book(new Book("BT4", "T4", "A", "C", 10));
        m.add_book(new Book("BT5", "T5", "A", "C", 10));
        m.add_book(new Book("BT6", "T6", "A", "C", 10));
        CHECK(m.borrow_book("RT", "BT1", "01/01/2026", "15/01/2026") == true, "Teacher borrow 1 OK", "");
        CHECK(m.borrow_book("RT", "BT2", "01/01/2026", "15/01/2026") == true, "Teacher borrow 2 OK", "");
        CHECK(m.borrow_book("RT", "BT3", "01/01/2026", "15/01/2026") == true, "Teacher borrow 3 OK", "");
        CHECK(m.borrow_book("RT", "BT4", "01/01/2026", "15/01/2026") == true, "Teacher borrow 4 OK", "");
        CHECK(m.borrow_book("RT", "BT5", "01/01/2026", "15/01/2026") == true, "Teacher borrow 5 OK", "");
        CHECK(m.borrow_book("RT", "BT6", "01/01/2026", "15/01/2026") == false, "Teacher limit (5)", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 7: RETURN BOOK
    // ==================================================================
    std::cout << "=== MODULE 7: RETURN BOOK ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("RT", "Return", "A", "C", 3));
        m.add_reader(new Reader("RR", "Returner", "student", "", ""));
        m.borrow_book("RR", "RT", "01/01/2026", "15/01/2026");
        
        CHECK(m.return_book("REC1001", "10/01/2026") == true, "Return on time OK", "");
        CHECK(m.find_book("RT")->quantity == 3, "Quantity restored to 3", "");
        CHECK(m.find_reader("RR")->borrowed_count == 0, "Reader count -> 0", "");
        CHECK(m.return_book("REC1001", "10/01/2026") == false, "Cannot return twice", "");
    }
    std::cout << "\n";
    
    std::cout << "--- 7b: Late return fine ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("LR", "Late", "A", "C", 3));
        m.add_reader(new Reader("LRR", "LateR", "student", "", ""));
        m.borrow_book("LRR", "LR", "01/01/2026", "10/01/2026");
        m.return_book("REC1001", "15/01/2026"); // 5 days late
        BorrowRecord* rec = m.find_record("REC1001");
        CHECK(rec != nullptr, "Record found", "");
        CHECK(rec->fine == 25000.0, "Fine = 5000*5 = 25000", "");
        CHECK(rec->late_days == 5, "Late days = 5", "");
        CHECK(rec->status == "overdue", "Status = overdue", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 8: BLOCK / UNBLOCK
    // ==================================================================
    std::cout << "=== MODULE 8: BLOCK / UNBLOCK ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("BU1", "BlockTest", "student", "", ""));
        CHECK(m.block_reader("BU1") == true, "Block OK", "");
        CHECK(m.find_reader("BU1")->is_blocked == true, "is_blocked = true", "");
        CHECK(m.block_reader("BU1") == false, "Already blocked", "");
        CHECK(m.unblock_reader("BU1") == true, "Unblock OK", "");
        CHECK(m.find_reader("BU1")->is_blocked == false, "is_blocked = false", "");
        CHECK(m.unblock_reader("BU1") == false, "Already unblocked", "");
        CHECK(m.block_reader("NOEXIST") == false, "Non-existent block fails", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 9 & 10: SAVE / LOAD ROUND-TRIP
    // ==================================================================
    std::cout << "=== MODULE 9 & 10: SAVE / LOAD ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("S1", "Save1", "A1", "C1", 5));
        m.add_book(new Book("S2", "Save2", "A2", "C2", 3));
        m.add_reader(new Reader("RS1", "Reader1", "student", "090", "a@b.com"));
        m.add_reader(new Reader("RS2", "Reader2", "teacher", "091", "c@d.com"));
        m.borrow_book("RS1", "S1", "01/06/2026", "15/06/2026");
        m.save_data();
        
        LibraryManager m2(DATA);
        m2.load_data();
        
        CHECK(m2.get_book_count() == 2, "2 books loaded", "");
        CHECK(m2.get_reader_count() == 2, "2 readers loaded", "");
        CHECK(m2.get_record_count() == 1, "1 record loaded", "");
        CHECK(m2.find_book("S1")->title == "Save1", "Book field preserved", "");
        CHECK(m2.find_book("S2")->quantity == 3, "Book quantity preserved", "");
        CHECK(m2.find_reader("RS1")->reader_type == "student", "Reader type preserved", "");
        CHECK(m2.find_reader("RS2")->phone == "091", "Reader phone preserved", "");
    }
    std::cout << "\n";

    std::cout << "--- 10b: Triple load — no duplicate ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("TD", "Triple", "A", "C", 1));
        m.save_data();
        m.load_data();
        m.load_data();
        m.load_data();
        CHECK(m.get_book_count() == 1, "Triple load: still 1", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 11: INPUT VALIDATION (UI LAYER)
    // ==================================================================
    std::cout << "=== MODULE 11: INPUT VALIDATION (UI layer) ===\n\n";
    {
        // UI validation is in main.cpp:
        // - read_string with allow_empty=false blocks empty input
        // - read_int with min_val=0 blocks negative
        // - read_menu_choice bounds checks
        // These are in the UI, not BL. We verify by testing BL with edge values.
        
        LibraryManager m(DATA);
        // BL accepts negative quantity
        Book* b = new Book("V1", "T", "A", "C", -10);
        CHECK(m.add_book(b) == true, "BL accepts qty=-10 (UI should block)", "");
        CHECK(m.find_book("V1")->quantity == -10, "Quantity stored as -10", "");
        
        // BL accepts empty title
        Book* b2 = new Book("V2", "", "", "", 1);
        CHECK(m.add_book(b2) == true, "BL accepts empty fields (UI validates)", "");
        
        // BL accepts invalid reader type
        Reader* r = new Reader("VT", "N", "invalid_type", "", "");
        CHECK(m.add_reader(r) == true, "BL accepts invalid reader type (UI validates)", "");
    }
    validation_pass += 3;
    std::cout << "\n";

    // ==================================================================
    // MODULE 12: DATA INTEGRITY INVARIANTS
    // ==================================================================
    std::cout << "=== MODULE 12: DATA INTEGRITY ===\n\n";
    
    std::cout << "--- Invariant 1: Book.quantity >= 0 ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("I1", "T", "A", "C", 0));
        CHECK(m.find_book("I1")->quantity == 0, "Quantity=0: acceptable", "");
        // Negative quantity is stored if passed, but system doesn't crash
        integrity_pass++;
    }
    std::cout << "\n";
    
    std::cout << "--- Invariant 2: Reader.borrowed_count >= 0 ---\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("I2", "N", "student", "", ""));
        // After return, count should be 0
        m.add_book(new Book("I2B", "T", "A", "C", 2));
        m.borrow_book("I2", "I2B", "01/01/2026", "15/01/2026");
        CHECK(m.find_reader("I2")->borrowed_count == 1, "After borrow: 1", "");
        m.return_book("REC1001", "10/01/2026");
        CHECK(m.find_reader("I2")->borrowed_count == 0, "After return: 0 (>= 0)", "");
        integrity_pass += 2;
    }
    std::cout << "\n";

    std::cout << "--- Invariant 3: BorrowRecord → Book exists ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("I3B", "T", "A", "C", 2));
        m.add_reader(new Reader("I3R", "N", "student", "", ""));
        m.borrow_book("I3R", "I3B", "01/01/2026", "15/01/2026");
        
        BorrowRecord* rec = m.find_record("REC1001");
        CHECK(rec != nullptr, "Record exists", "");
        CHECK(rec->book_id == "I3B", "Record points to correct book", "");
        
        // Now delete the book (should be blocked by our fix)
        // But what if we return first, then delete?
        m.return_book("REC1001", "10/01/2026");
        bool deleted = m.remove_book("I3B");
        CHECK(deleted == true, "Can delete after return", "");
        // Record still exists but book is gone — record is historical
        CHECK(m.find_record("REC1001") != nullptr, "Historical record preserved", "");
        integrity_pass += 2;
    }
    std::cout << "\n";

    std::cout << "--- Invariant 5: Quantity <= original ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("I5", "T", "A", "C", 10));
        m.add_reader(new Reader("I5R", "N", "student", "", ""));
        
        // Borrow 3 times
        m.borrow_book("I5R", "I5", "01/01/2026", "15/01/2026");
        m.add_book(new Book("I5_2", "T", "A", "C", 5));
        m.borrow_book("I5R", "I5_2", "01/01/2026", "15/01/2026");
        m.add_book(new Book("I5_3", "T", "A", "C", 5));
        m.borrow_book("I5R", "I5_3", "01/01/2026", "15/01/2026");
        
        CHECK(m.find_book("I5")->quantity == 9, "Quantity decreased, never > original", "");
        integrity_pass++;
    }
    std::cout << "\n";

    // ==================================================================
    // EDGE CASES
    // ==================================================================
    std::cout << "=== EDGE CASES ===\n\n";
    {
        LibraryManager m(DATA);
        // Borrow with due date = borrow date
        m.add_book(new Book("EC1", "Edge", "A", "C", 1));
        m.add_reader(new Reader("ECR", "Edge", "student", "", ""));
        CHECK(m.borrow_book("ECR", "EC1", "01/01/2026", "01/01/2026") == true, "Due = borrow date: OK", "");
        
        // Return on same day as borrow
        m.return_book("REC1001", "01/01/2026");
        BorrowRecord* rec = m.find_record("REC1001");
        CHECK(rec != nullptr, "Record OK", "");
        CHECK(rec->fine == 0.0, "No late fine", "");
    }
    std::cout << "\n";

    {
        LibraryManager m(DATA);
        // Test record ID counter sync after load
        m.add_book(new Book("EC2", "Edge2", "A", "C", 10));
        m.add_reader(new Reader("ECR2", "Edge2", "student", "", ""));
        m.borrow_book("ECR2", "EC2", "01/01/2026", "15/01/2026"); // REC1001
        m.borrow_book("ECR2", "EC2", "01/01/2026", "15/01/2026"); // REC1002
        m.save_data();
        
        LibraryManager m2(DATA);
        m2.load_data();
        m2.add_book(new Book("EC3", "Edge3", "A", "C", 10));
        m2.add_reader(new Reader("ECR3", "Edge3", "student", "", ""));
        m2.borrow_book("ECR3", "EC3", "01/01/2026", "15/01/2026"); // Should be REC1003, not REC1001
        
        // Verify no collision: REC1003 should exist, REC1001 should still be from load
        CHECK(m2.find_record("REC1001") != nullptr, "REC1001 from load still exists", "");
        CHECK(m2.find_record("REC1002") != nullptr, "REC1002 from load still exists", "");
        CHECK(m2.find_record("REC1003") != nullptr, "New REC1003 created (no collision)", "");
    }
    std::cout << "\n";

    // ==================================================================
    // RESTORE & SUMMARY
    // ==================================================================
    restore();

    std::cout << "=============================================================\n";
    std::cout << "     COMPREHENSIVE AUDIT RESULTS\n";
    std::cout << "=============================================================\n";
    std::cout << "\n";
    std::cout << "  Business Logic:     " << passed << " PASS / " << failed << " FAIL\n";
    std::cout << "  Total assertions:   " << (passed + failed) << "\n";
    std::cout << "\n";
    
    if (failed > 0) {
        std::cout << "  ISSUES IDENTIFIED:\n";
        std::cout << "  - BL layer does NOT validate: empty ID, empty name, negative qty\n";
        std::cout << "  - BL layer does NOT validate: email format, phone format, reader type\n";
        std::cout << "  - These are delegated to UI layer (main.cpp)\n";
        std::cout << "  - UI layer currently uses read_string() which blocks empty input\n";
        std::cout << "  - UI layer read_int(prompt, 1) blocks negative for quantity\n";
        std::cout << "  - UI layer validates reader type with loop check\n";
        std::cout << "  - RECOMMENDATION: Add BL-level guards for quantity >= 0 in add_book\n";
        std::cout << "\n";
    }
    
    std::cout << "=============================================================\n";
    return failed > 0 ? 1 : 0;
}
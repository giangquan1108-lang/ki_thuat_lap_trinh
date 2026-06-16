/**
 * VALIDATION HARDENING TEST
 * Tests the 6 hardening points before and after patches.
 */
#include "lb_management/services/LibraryManager.h"
#include "lb_management/models/Book.h"
#include "lb_management/models/Reader.h"
#include <iostream>
#include <string>
#include <fstream>

int pass = 0, fail = 0;
#define P(msg) do { std::cout << "  PASS: " << msg << "\n"; pass++; } while(0)
#define F(msg) do { std::cout << "  FAIL: " << msg << "\n"; fail++; } while(0)
#define CHECK(cond, msg) do { if(cond) P(msg); else F(msg); } while(0)

const std::string DATA = "./data/";

int main() {
    std::cout << "=============================================================\n";
    std::cout << "     VALIDATION HARDENING VERIFICATION\n";
    std::cout << "=============================================================\n\n";

    // =================================================================
    // HARDENING #1: Negative quantity rejected at BL
    // =================================================================
    std::cout << "=== HARDENING #1: Negative Quantity ===\n";
    {
        LibraryManager m(DATA);
        Book* b1 = new Book("HQ1", "T", "A", "C", -10);
        bool result = m.add_book(b1);
        if (result) {
            F("BL accepted qty=-10 — NOT HARDENED");
        } else {
            P("BL rejected qty=-10 — HARDENED");
            delete b1;
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #2: Duplicate Reader ID (already blocked)
    // =================================================================
    std::cout << "=== HARDENING #2: Duplicate Reader ID ===\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("DR", "N", "student", "090", "a@b.com"));
        Reader* dup = new Reader("DR", "N2", "teacher", "091", "c@d.com");
        bool result = m.add_reader(dup);
        if (result) {
            F("BL accepted duplicate reader ID");
            delete dup;
        } else {
            P("BL rejected duplicate reader ID — HARDENED");
            delete dup;
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #3: Invalid email rejected
    // =================================================================
    std::cout << "=== HARDENING #3: Email Validation ===\n";
    {
        LibraryManager m(DATA);
        // No @ sign
        Reader* r1 = new Reader("EM1", "N", "student", "090", "notanemail");
        bool r = m.add_reader(r1);
        if (r) {
            F("BL accepted email without @ — NOT HARDENED");
        } else {
            P("BL rejected email without @ — HARDENED");
            delete r1;
        }
        
        // No domain
        Reader* r2 = new Reader("EM2", "N", "student", "090", "user@");
        bool r2_result = m.add_reader(r2);
        if (r2_result) {
            F("BL accepted email without domain — NOT HARDENED");
        } else {
            P("BL rejected email without domain — HARDENED");
            delete r2;
        }
        
        // Valid email (with valid phone to not trigger phone check)
        Reader* r3 = new Reader("EM3", "N", "student", "0987654321", "valid@email.com");
        bool r3_result = m.add_reader(r3);
        if (r3_result) {
            P("BL accepted valid email — OK");
        } else {
            F("BL rejected valid email — OVER-BLOCKED");
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #4: Invalid phone rejected
    // =================================================================
    std::cout << "=== HARDENING #4: Phone Validation ===\n";
    {
        LibraryManager m(DATA);
        // Too short
        Reader* r1 = new Reader("PH1", "N", "student", "123", "a@b.com");
        bool r = m.add_reader(r1);
        if (r) {
            F("BL accepted phone with <10 digits — NOT HARDENED");
        } else {
            P("BL rejected short phone — HARDENED");
            delete r1;
        }
        
        // Contains letters
        Reader* r2 = new Reader("PH2", "N", "student", "abcdefghij", "a@b.com");
        bool r2_result = m.add_reader(r2);
        if (r2_result) {
            F("BL accepted non-numeric phone — NOT HARDENED");
        } else {
            P("BL rejected non-numeric phone — HARDENED");
            delete r2;
        }
        
        // Valid phone
        Reader* r3 = new Reader("PH3", "N", "student", "0987654321", "a@b.com");
        bool r3_result = m.add_reader(r3);
        if (r3_result) {
            P("BL accepted valid phone — OK");
        } else {
            F("BL rejected valid phone — OVER-BLOCKED");
        }
        
        // Empty phone should be OK
        Reader* r4 = new Reader("PH4", "N", "student", "", "a@b.com");
        bool r4_result = m.add_reader(r4);
        if (r4_result) {
            P("BL accepted empty phone — OK (optional field)");
        } else {
            F("BL rejected empty phone — OVER-BLOCKED");
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #5: Double return blocked
    // =================================================================
    std::cout << "=== HARDENING #5: Double Return Prevention ===\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("HR5", "T", "A", "C", 3));
        m.add_reader(new Reader("HR5R", "N", "student", "", ""));
        m.borrow_book("HR5R", "HR5", "01/01/2026", "15/01/2026");
        bool r1 = m.return_book("REC1001", "10/01/2026");
        if (r1) {
            P("First return OK");
        } else {
            F("First return blocked incorrectly");
        }
        bool r2 = m.return_book("REC1001", "20/01/2026");
        if (r2) {
            F("Second return NOT blocked — NOT HARDENED");
        } else {
            P("Second return blocked — HARDENED");
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #6: Borrow limit enforcement
    // =================================================================
    std::cout << "=== HARDENING #6: Borrow Limit Enforcement ===\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("HL", "T", "A", "C", 10));
        m.add_reader(new Reader("HLR", "N", "student", "", ""));
        
        m.borrow_book("HLR", "HL", "01/01/2026", "15/01/2026");
        m.add_book(new Book("HL2", "T2", "A", "C", 10));
        m.borrow_book("HLR", "HL2", "01/01/2026", "15/01/2026");
        m.add_book(new Book("HL3", "T3", "A", "C", 10));
        m.borrow_book("HLR", "HL3", "01/01/2026", "15/01/2026");
        
        m.add_book(new Book("HL4", "T4", "A", "C", 10));
        bool r = m.borrow_book("HLR", "HL4", "01/01/2026", "15/01/2026");
        if (r) {
            F("BL allowed 4th borrow (student limit=3) — NOT HARDENED");
        } else {
            P("BL rejected 4th borrow (student limit=3) — HARDENED");
        }
        
        // Teacher limit = 5
        m.add_reader(new Reader("HLT", "T", "teacher", "", ""));
        m.add_book(new Book("HLT1", "T1", "A", "C", 10));
        m.add_book(new Book("HLT2", "T2", "A", "C", 10));
        m.add_book(new Book("HLT3", "T3", "A", "C", 10));
        m.add_book(new Book("HLT4", "T4", "A", "C", 10));
        m.add_book(new Book("HLT5", "T5", "A", "C", 10));
        m.add_book(new Book("HLT6", "T6", "A", "C", 10));
        m.borrow_book("HLT", "HLT1", "01/01/2026", "15/01/2026");
        m.borrow_book("HLT", "HLT2", "01/01/2026", "15/01/2026");
        m.borrow_book("HLT", "HLT3", "01/01/2026", "15/01/2026");
        m.borrow_book("HLT", "HLT4", "01/01/2026", "15/01/2026");
        m.borrow_book("HLT", "HLT5", "01/01/2026", "15/01/2026");
        bool r_t = m.borrow_book("HLT", "HLT6", "01/01/2026", "15/01/2026");
        if (r_t) {
            F("BL allowed 6th borrow (teacher limit=5) — NOT HARDENED");
        } else {
            P("BL rejected 6th borrow (teacher limit=5) — HARDENED");
        }
    }
    std::cout << "\n";

    // =================================================================
    // SUMMARY
    // =================================================================
    std::cout << "=============================================================\n";
    std::cout << "     RESULTS: " << pass << " PASS / " << fail << " FAIL\n";
    std::cout << "=============================================================\n";
    
    if (fail > 0) {
        std::cout << "\n  VALIDATION GAPS STILL PRESENT:\n";
        std::cout << "  - Some hardening points are not yet implemented\n";
        std::cout << "  - Run validation_hardening.cpp to see fixes applied\n";
        std::cout << "\n";
    } else {
        std::cout << "\n  ALL HARDENING POINTS VERIFIED!\n";
        std::cout << "\n";
    }
    
    return fail > 0 ? 1 : 0;
}
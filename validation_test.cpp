/**
 * TEST KIỂM TRA HARDENING VALIDATION
 * Xác nhận 6 điểm làm cứng trước và sau khi vá.
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
    std::cout << "     KIEM TRA VALIDATION HARDENING\n";
    std::cout << "=============================================================\n\n";

    // =================================================================
    // HARDENING #1: Từ chối số lượng âm ở tầng BL
    // =================================================================
    std::cout << "=== HARDENING #1: So Luong Am ===\n";
    {
        LibraryManager m(DATA);
        Book* b1 = new Book("HQ1", "T", "A", "C", -10);
        bool result = m.add_book(b1);
        if (result) {
            F("BL nhan qty=-10 — CHUA CUNG HOA");
        } else {
            P("BL tu choi qty=-10 — DA CUNG HOA");
            delete b1;
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #2: Từ chối trùng ID Bạn đọc (đã chặn sẵn)
    // =================================================================
    std::cout << "=== HARDENING #2: Trung ID Ban Doc ===\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("DR", "N", "student", "090", "a@b.com"));
        Reader* dup = new Reader("DR", "N2", "teacher", "091", "c@d.com");
        bool result = m.add_reader(dup);
        if (result) {
            F("BL nhan ID ban doc trung");
            delete dup;
        } else {
            P("BL tu choi ID ban doc trung — DA CUNG HOA");
            delete dup;
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #3: Từ chối email sai định dạng
    // =================================================================
    std::cout << "=== HARDENING #3: Validate Email ===\n";
    {
        LibraryManager m(DATA);
        // Thiếu @
        Reader* r1 = new Reader("EM1", "N", "student", "090", "notanemail");
        bool r = m.add_reader(r1);
        if (r) {
            F("BL nhan email thieu @ — CHUA CUNG HOA");
        } else {
            P("BL tu choi email thieu @ — DA CUNG HOA");
            delete r1;
        }
        
        // Thiếu domain
        Reader* r2 = new Reader("EM2", "N", "student", "090", "user@");
        bool r2_result = m.add_reader(r2);
        if (r2_result) {
            F("BL nhan email thieu domain — CHUA CUNG HOA");
        } else {
            P("BL tu choi email thieu domain — DA CUNG HOA");
            delete r2;
        }
        
        // Email hợp lệ (phone cũng hợp lệ để ko vướng check phone)
        Reader* r3 = new Reader("EM3", "N", "student", "0987654321", "valid@email.com");
        bool r3_result = m.add_reader(r3);
        if (r3_result) {
            P("BL nhan email hop le — OK");
        } else {
            F("BL tu choi email hop le — CHAN QUA TAY");
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #4: Từ chối SDT sai định dạng
    // =================================================================
    std::cout << "=== HARDENING #4: Validate SDT ===\n";
    {
        LibraryManager m(DATA);
        // Quá ngắn
        Reader* r1 = new Reader("PH1", "N", "student", "123", "a@b.com");
        bool r = m.add_reader(r1);
        if (r) {
            F("BL nhan SDT <10 so — CHUA CUNG HOA");
        } else {
            P("BL tu choi SDT ngan — DA CUNG HOA");
            delete r1;
        }
        
        // Có chữ
        Reader* r2 = new Reader("PH2", "N", "student", "abcdefghij", "a@b.com");
        bool r2_result = m.add_reader(r2);
        if (r2_result) {
            F("BL nhan SDT chua chu — CHUA CUNG HOA");
        } else {
            P("BL tu choi SDT co chu — DA CUNG HOA");
            delete r2;
        }
        
        // SDT hợp lệ
        Reader* r3 = new Reader("PH3", "N", "student", "0987654321", "a@b.com");
        bool r3_result = m.add_reader(r3);
        if (r3_result) {
            P("BL nhan SDT hop le — OK");
        } else {
            F("BL tu choi SDT hop le — CHAN QUA TAY");
        }
        
        // SDT rỗng (cho phép vì là field optional)
        Reader* r4 = new Reader("PH4", "N", "student", "", "a@b.com");
        bool r4_result = m.add_reader(r4);
        if (r4_result) {
            P("BL nhan SDT rong — OK (field tuy chon)");
        } else {
            F("BL tu choi SDT rong — CHAN QUA TAY");
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #5: Chặn trả sách 2 lần
    // =================================================================
    std::cout << "=== HARDENING #5: Chan Tra 2 Lan ===\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("HR5", "T", "A", "C", 3));
        m.add_reader(new Reader("HR5R", "N", "student", "", ""));
        m.borrow_book("HR5R", "HR5", "01/01/2026", "15/01/2026");
        bool r1 = m.return_book("REC1001", "10/01/2026");
        if (r1) {
            P("Tra lan 1 OK");
        } else {
            F("Tra lan 1 bi chan sai");
        }
        bool r2 = m.return_book("REC1001", "20/01/2026");
        if (r2) {
            F("Tra lan 2 KO bi chan — CHUA CUNG HOA");
        } else {
            P("Tra lan 2 bi chan — DA CUNG HOA");
        }
    }
    std::cout << "\n";

    // =================================================================
    // HARDENING #6: Thực thi giới hạn mượn
    // =================================================================
    std::cout << "=== HARDENING #6: Gioi Han Muon ===\n";
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
            F("BL cho muon lan 4 (SV limit=3) — CHUA CUNG HOA");
        } else {
            P("BL tu choi muon lan 4 (SV limit=3) — DA CUNG HOA");
        }
        
        // Giáo viên limit = 5
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
            F("BL cho GV muon lan 6 (GV limit=5) — CHUA CUNG HOA");
        } else {
            P("BL tu choi GV muon lan 6 (GV limit=5) — DA CUNG HOA");
        }
    }
    std::cout << "\n";

    // =================================================================
    // TỔNG KẾT
    // =================================================================
    std::cout << "=============================================================\n";
    std::cout << "     KET QUA: " << pass << " DAU / " << fail << " ROT\n";
    std::cout << "=============================================================\n";
    
    if (fail > 0) {
        std::cout << "\n  LO HONG VALIDATION CON TON TAI:\n";
        std::cout << "  - Vai diem hardening chua duoc cai dat\n";
        std::cout << "  - Chay validation_hardening.cpp de xem ban va\n";
        std::cout << "\n";
    } else {
        std::cout << "\n  TAT CA DIEM HARDENING DA DUOC KIEM CHUNG!\n";
        std::cout << "\n";
    }
    
    return fail > 0 ? 1 : 0;
}
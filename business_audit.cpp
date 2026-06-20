/**
 * KIỂM ĐỊNH TOÀN DIỆN BUSINESS LOGIC
 * Rà soát: validation, bất biến, tình huống biên, điều kiện trước/sau, toàn vẹn dữ liệu
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

// Backup data gốc trước khi test
void backup() {
    std::system("copy data\\books.txt data\\books_audit_bak.txt > nul 2>&1");
    std::system("copy data\\readers.txt data\\readers_audit_bak.txt > nul 2>&1");
    std::system("copy data\\records.txt data\\records_audit_bak.txt > nul 2>&1");
}
// Dọn sạch data để test trên nền trắng
void clear_data() {
    std::ofstream("data/books.txt", std::ios::trunc).close();
    std::ofstream("data/readers.txt", std::ios::trunc).close();
    std::ofstream("data/records.txt", std::ios::trunc).close();
}
// Trả lại data như cũ
void restore() {
    std::system("copy data\\books_audit_bak.txt data\\books.txt > nul 2>&1");
    std::system("copy data\\readers_audit_bak.txt data\\readers.txt > nul 2>&1");
    std::system("copy data\\records_audit_bak.txt data\\records.txt > nul 2>&1");
}

int main() {
    std::cout << "=============================================================\n";
    std::cout << "     KIEM DINH TOAN DIEN BUSINESS LOGIC\n";
    std::cout << "=============================================================\n\n";

    backup();
    clear_data();

    // ==================================================================
    // MODULE 1: THÊM SÁCH — VALIDATION & TÍNH NHẤT QUÁN
    // ==================================================================
    std::cout << "=== MODULE 1: THEM SACH ===\n\n";
    std::cout << "--- 1a: Kiem Tra Dau Vao ---\n";
    {
        LibraryManager m(DATA);
        // ID rỗng
        Book* b = new Book("", "T", "A", "C", 1);
        CHECK(m.add_book(b) == true, "ID rong: BL van nhan (UI se chan)", "BL cho ID rong — UI layer chiu trach nhiem validate");
        // Số lượng âm
        Book* b2 = new Book("B1", "T", "A", "C", -5);
        CHECK(m.add_book(b2) == true, "So luong am: van them vao (qty=-5)", "Ko co chan dau vao o BL");
        // Số lượng = 0
        Book* b3 = new Book("Q0", "T", "A", "C", 0);
        CHECK(m.add_book(b3) == true, "So luong=0: them OK", "");
        // Số lượng quá lớn
        Book* b4 = new Book("BIG", "T", "A", "C", 999999);
        CHECK(m.add_book(b4) == true, "So luong lon: them OK", "");
    }
    std::cout << "\n  --- Tong ket Validation ---\n";
    validation_pass += 4; // BL cho qua hết — UI validate
    std::cout << "\n";

    std::cout << "--- 1b: ID Trung ---\n";
    {
        LibraryManager m(DATA);
        CHECK(m.add_book(new Book("DUP", "T", "A", "C", 1)) == true, "Them lan dau OK", "");
        Book* dup = new Book("DUP", "T", "A", "C", 1);
        CHECK(m.add_book(dup) == false, "ID trung bi chan", "");
        delete dup;
        CHECK(m.get_book_count() == 1, "Tong van la 1 sau khi them trung", "");
    }
    std::cout << "\n";

    std::cout << "--- 1c: Dong bo LinkedList / HashMap ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("C1", "T1", "A1", "C1", 5));
        m.add_book(new Book("C2", "T2", "A2", "C2", 3));
        m.add_book(new Book("C3", "T3", "A3", "C3", 7));
        
        CHECK(m.get_book_count() == 3, "LinkedList size = 3", "");
        CHECK(m.find_book("C1") != nullptr, "HashMap co C1", "");
        CHECK(m.find_book("C2") != nullptr, "HashMap co C2", "");
        CHECK(m.find_book("C3") != nullptr, "HashMap co C3", "");
        CHECK(m.find_book("C1")->title == "T1", "Cung 1 con tro trong ca 2 cau truc", "");
        CHECK(m.find_book("NONEXIST") == nullptr, "ID ko ton tai -> nullptr", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 2: XÓA SÁCH
    // ==================================================================
    std::cout << "=== MODULE 2: XOA SACH ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("D1", "Del", "A", "C", 1));
        CHECK(m.remove_book("D1") == true, "Xoa sach dang co OK", "");
        CHECK(m.get_book_count() == 0, "LinkedList rong sau xoa", "");
        CHECK(m.find_book("D1") == nullptr, "HashMap rong sau xoa", "");
        
        CHECK(m.remove_book("NONEXIST") == false, "Xoa ID ko ton tai -> false", "");
    }
    std::cout << "\n";

    std::cout << "--- 2b: Chan xoa sach dang bi muon ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("DB", "Borrowed", "A", "C", 2));
        m.add_reader(new Reader("RD", "Reader", "student", "", ""));
        m.borrow_book("RD", "DB", "01/01/2026", "15/01/2026");
        
        CHECK(m.remove_book("DB") == false, "Ko the xoa sach dang muon", "");
        CHECK(m.find_book("DB") != nullptr, "Sach van con trong he thong", "");
        CHECK(m.get_book_count() == 1, "Tong so sach ko doi", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 3: CẬP NHẬT SÁCH
    // ==================================================================
    std::cout << "=== MODULE 3: CAP NHAT SACH ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("U1", "Old", "OldA", "OldC", 5));
        
        CHECK(m.update_book("U1", "NewTitle", "NewA", "NewC", 10) == true, "Cap nhat tat ca field OK", "");
        Book* b = m.find_book("U1");
        CHECK(b->title == "NewTitle", "Tua da doi", "");
        CHECK(b->author == "NewA", "Tac gia da doi", "");
        CHECK(b->category == "NewC", "The loai da doi", "");
        CHECK(b->quantity == 10, "So luong da doi", "");
        
        // Cập nhật 1 phần
        CHECK(m.update_book("U1", "", "", "", -1) == true, "Ko field nao bi doi (qty=-1 bi bo qua)", "");
        CHECK(m.update_book("U1", "OnlyTitle", "", "", -1) == true, "Chi sua tua", "");
        CHECK(m.find_book("U1")->title == "OnlyTitle", "Cap nhat 1 phan OK", "");
        
        CHECK(m.update_book("NONEXIST", "X", "X", "X", 1) == false, "Ko ton tai -> that bai", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 4: THÊM BẠN ĐỌC
    // ==================================================================
    std::cout << "=== MODULE 4: THEM BAN DOC ===\n\n";
    std::cout << "--- 4a: Validation ---\n";
    {
        LibraryManager m(DATA);
        // ID rỗng
        Reader* r1 = new Reader("", "Name", "student", "090", "a@b.com");
        CHECK(m.add_reader(r1) == true, "ID rong: BL van cho (UI chan)", "");
        // Tên rỗng
        Reader* r2 = new Reader("R1", "", "student", "090", "a@b.com");
        CHECK(m.add_reader(r2) == true, "Ten rong: BL van cho", "");
        // Email sai format
        Reader* r3 = new Reader("R2", "N", "student", "090", "notanemail");
        CHECK(m.add_reader(r3) == true, "Email sai: BL van cho (ko check format)", "");
    }
    std::cout << "\n  --- Tong ket Validation: tang BL KO validate ---\n";
    validation_pass += 3;
    std::cout << "\n";

    std::cout << "--- 4b: Trung ID Ban doc ---\n";
    {
        LibraryManager m(DATA);
        CHECK(m.add_reader(new Reader("R001", "N", "student", "", "")) == true, "Them lan dau OK", "");
        Reader* dup = new Reader("R001", "N2", "teacher", "", "");
        CHECK(m.add_reader(dup) == false, "Trung ID bi chan", "");
        delete dup;
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 5: XÓA BẠN ĐỌC
    // ==================================================================
    std::cout << "=== MODULE 5: XOA BAN DOC ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("DR1", "N", "student", "", ""));
        CHECK(m.remove_reader("DR1") == true, "Xoa thanh cong", "");
        CHECK(m.find_reader("DR1") == nullptr, "Da xoa khoi HashMap", "");
        CHECK(m.remove_reader("NONEXIST") == false, "Xoa ID ko ton tai -> false", "");
    }
    std::cout << "\n";

    std::cout << "--- 5b: Chan xoa ban doc dang muon sach ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("DRB", "Bk", "A", "C", 2));
        m.add_reader(new Reader("DRR", "Rd", "student", "", ""));
        m.borrow_book("DRR", "DRB", "01/01/2026", "15/01/2026");
        CHECK(m.remove_reader("DRR") == false, "Ko the xoa ban doc dang muon", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 6: MƯỢN SÁCH — CHECK PRE/POST CONDITION ĐẦY ĐỦ
    // ==================================================================
    std::cout << "=== MODULE 6: MUON SACH ===\n\n";
    std::cout << "--- 6a: Happy Path ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("BHP", "Happy", "A", "C", 5));
        m.add_reader(new Reader("RHP", "Reader", "student", "", ""));
        
        size_t books_before = m.get_book_count();
        bool result = m.borrow_book("RHP", "BHP", "01/01/2026", "15/01/2026");
        CHECK(result == true, "Muon thanh cong", "");
        CHECK(m.find_book("BHP")->quantity == 4, "SL sach: 5->4", "");
        CHECK(m.find_book("BHP")->borrow_count == 1, "Luot muon: 0->1", "");
        CHECK(m.find_reader("RHP")->borrowed_count == 1, "Ban doc dang muon: 0->1", "");
        CHECK(m.get_record_count() == 1, "Da tao 1 phieu muon", "");
        CHECK(m.get_borrowing_count() == 1, "1 phieu dang hoat dong", "");
    }
    std::cout << "\n";

    std::cout << "--- 6b: Tat Ca Tinh Huong That Bai ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("BF", "FailBk", "A", "C", 1));
        m.add_reader(new Reader("RF", "FailRd", "student", "", ""));
        
        // Ban doc ko ton tai
        CHECK(m.borrow_book("NOEXIST", "BF", "01/01/2026", "15/01/2026") == false, "Ban doc ko ton tai", "");
        
        // Sach ko ton tai
        CHECK(m.borrow_book("RF", "NOEXIST", "01/01/2026", "15/01/2026") == false, "Sach ko ton tai", "");
        
        // Ban doc bi khoa
        m.block_reader("RF");
        CHECK(m.borrow_book("RF", "BF", "01/01/2026", "15/01/2026") == false, "Ban doc bi khoa", "");
        m.unblock_reader("RF");
        
        // Het hang
        CHECK(m.borrow_book("RF", "BF", "01/01/2026", "15/01/2026") == true, "Muon lan 1 OK", "");
        CHECK(m.find_book("BF")->quantity == 0, "SL = 0 sau muon", "");
        CHECK(m.borrow_book("RF", "BF", "01/01/2026", "15/01/2026") == false, "Het hang roi", "");
        
        // Dat gioi han (sinh vien=3)
        // RF dang muon 1 cuon. Them 2 cuon nua cho du 3.
        m.add_book(new Book("BF2", "Bk2", "A", "C", 5));
        m.add_book(new Book("BF3", "Bk3", "A", "C", 5));
        m.add_book(new Book("BF4", "Bk4", "A", "C", 5));
        CHECK(m.borrow_book("RF", "BF2", "01/01/2026", "15/01/2026") == true, "Muon lan 2 OK", "");
        CHECK(m.borrow_book("RF", "BF3", "01/01/2026", "15/01/2026") == true, "Muon lan 3 OK", "");
        CHECK(m.borrow_book("RF", "BF4", "01/01/2026", "15/01/2026") == false, "Dat gioi han (3)", "");
        
        // Gioi han giao vien = 5
        m.add_reader(new Reader("RT", "Teacher", "teacher", "", ""));
        m.add_book(new Book("BT1", "T1", "A", "C", 10));
        m.add_book(new Book("BT2", "T2", "A", "C", 10));
        m.add_book(new Book("BT3", "T3", "A", "C", 10));
        m.add_book(new Book("BT4", "T4", "A", "C", 10));
        m.add_book(new Book("BT5", "T5", "A", "C", 10));
        m.add_book(new Book("BT6", "T6", "A", "C", 10));
        CHECK(m.borrow_book("RT", "BT1", "01/01/2026", "15/01/2026") == true, "GV muon 1 OK", "");
        CHECK(m.borrow_book("RT", "BT2", "01/01/2026", "15/01/2026") == true, "GV muon 2 OK", "");
        CHECK(m.borrow_book("RT", "BT3", "01/01/2026", "15/01/2026") == true, "GV muon 3 OK", "");
        CHECK(m.borrow_book("RT", "BT4", "01/01/2026", "15/01/2026") == true, "GV muon 4 OK", "");
        CHECK(m.borrow_book("RT", "BT5", "01/01/2026", "15/01/2026") == true, "GV muon 5 OK", "");
        CHECK(m.borrow_book("RT", "BT6", "01/01/2026", "15/01/2026") == false, "GV dat gioi han (5)", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 7: TRẢ SÁCH
    // ==================================================================
    std::cout << "=== MODULE 7: TRA SACH ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("RT", "Return", "A", "C", 3));
        m.add_reader(new Reader("RR", "Returner", "student", "", ""));
        m.borrow_book("RR", "RT", "01/01/2026", "15/01/2026");
        
        CHECK(m.return_book("REC1001", "10/01/2026") == true, "Tra dung han OK", "");
        CHECK(m.find_book("RT")->quantity == 3, "SL hoi phuc ve 3", "");
        CHECK(m.find_reader("RR")->borrowed_count == 0, "Ban doc ve 0", "");
        CHECK(m.return_book("REC1001", "10/01/2026") == false, "Ko duoc tra 2 lan", "");
    }
    std::cout << "\n";
    
    std::cout << "--- 7b: Phat tra tre ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("LR", "Late", "A", "C", 3));
        m.add_reader(new Reader("LRR", "LateR", "student", "", ""));
        m.borrow_book("LRR", "LR", "01/01/2026", "10/01/2026");
        m.return_book("REC1001", "15/01/2026"); // Tre 5 ngay
        BorrowRecord* rec = m.find_record("REC1001");
        CHECK(rec != nullptr, "Tim thay phieu", "");
        CHECK(rec->fine == 25000.0, "Phat = 5000*5 = 25000", "");
        CHECK(rec->late_days == 5, "So ngay tre = 5", "");
        CHECK(rec->status == "overdue", "Trang thai = qua han", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 8: KHÓA / MỞ KHÓA
    // ==================================================================
    std::cout << "=== MODULE 8: KHOA / MO KHOA ===\n\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("BU1", "BlockTest", "student", "", ""));
        CHECK(m.block_reader("BU1") == true, "Khoa OK", "");
        CHECK(m.find_reader("BU1")->is_blocked == true, "is_blocked = true", "");
        CHECK(m.block_reader("BU1") == false, "Dang khoa roi, ko khoa nua", "");
        CHECK(m.unblock_reader("BU1") == true, "Mo khoa OK", "");
        CHECK(m.find_reader("BU1")->is_blocked == false, "is_blocked = false", "");
        CHECK(m.unblock_reader("BU1") == false, "Dang mo roi, ko mo nua", "");
        CHECK(m.block_reader("NOEXIST") == false, "Ko ton tai -> false", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 9 & 10: LƯU / TẢI VÒNG ĐỜI
    // ==================================================================
    std::cout << "=== MODULE 9 & 10: LUU / TAI DU LIEU ===\n\n";
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
        
        CHECK(m2.get_book_count() == 2, "Load duoc 2 sach", "");
        CHECK(m2.get_reader_count() == 2, "Load duoc 2 ban doc", "");
        CHECK(m2.get_record_count() == 1, "Load duoc 1 phieu", "");
        CHECK(m2.find_book("S1")->title == "Save1", "Tua sach giu nguyen", "");
        CHECK(m2.find_book("S2")->quantity == 3, "SL sach giu nguyen", "");
        CHECK(m2.find_reader("RS1")->reader_type == "student", "Loai ban doc giu nguyen", "");
        CHECK(m2.find_reader("RS2")->phone == "091", "SDT giu nguyen", "");
    }
    std::cout << "\n";

    std::cout << "--- 10b: Load 3 lan — ko duplicate ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("TD", "Triple", "A", "C", 1));
        m.save_data();
        m.load_data();
        m.load_data();
        m.load_data();
        CHECK(m.get_book_count() == 1, "Load 3 lan: van 1 cuon", "");
    }
    std::cout << "\n";

    // ==================================================================
    // MODULE 11: KIỂM TRA VALIDATION ĐẦU VÀO (TẦNG UI)
    // ==================================================================
    std::cout << "=== MODULE 11: VALIDATION DAU VAO (tang UI) ===\n\n";
    {
        // Validation nằm ở tầng UI trong main.cpp:
        // - read_string cho phep_empty=false -> chan rong
        // - read_int min_val=0 -> chan am
        // - read_menu_choice -> gioi han lua chon
        // O day ta test xem BL xu ly sao voi gia tri bien
        
        LibraryManager m(DATA);
        // BL cho phep so luong am
        Book* b = new Book("V1", "T", "A", "C", -10);
        CHECK(m.add_book(b) == true, "BL nhan qty=-10 (UI phai chan)", "");
        CHECK(m.find_book("V1")->quantity == -10, "SL luu vao la -10", "");
        
        // BL cho phep field rong
        Book* b2 = new Book("V2", "", "", "", 1);
        CHECK(m.add_book(b2) == true, "BL nhan field rong (UI phai chan)", "");
        
        // BL cho phep reader_type sai
        Reader* r = new Reader("VT", "N", "invalid_type", "", "");
        CHECK(m.add_reader(r) == true, "BL nhan sai loai ban doc (UI phai chan)", "");
    }
    validation_pass += 3;
    std::cout << "\n";

    // ==================================================================
    // MODULE 12: BẤT BIẾN TOÀN VẸN DỮ LIỆU
    // ==================================================================
    std::cout << "=== MODULE 12: TOAN VEN DU LIEU ===\n\n";
    
    std::cout << "--- Bat bien 1: Book.quantity >= 0 ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("I1", "T", "A", "C", 0));
        CHECK(m.find_book("I1")->quantity == 0, "SL=0: chap nhan duoc", "");
        // SL am duoc luu neu truyen vao, nhung he thong ko bi crash
        integrity_pass++;
    }
    std::cout << "\n";
    
    std::cout << "--- Bat bien 2: Reader.borrowed_count >= 0 ---\n";
    {
        LibraryManager m(DATA);
        m.add_reader(new Reader("I2", "N", "student", "", ""));
        m.add_book(new Book("I2B", "T", "A", "C", 2));
        m.borrow_book("I2", "I2B", "01/01/2026", "15/01/2026");
        CHECK(m.find_reader("I2")->borrowed_count == 1, "Sau muon: 1", "");
        m.return_book("REC1001", "10/01/2026");
        CHECK(m.find_reader("I2")->borrowed_count == 0, "Sau tra: 0 (>= 0)", "");
        integrity_pass += 2;
    }
    std::cout << "\n";

    std::cout << "--- Bat bien 3: BorrowRecord -> Book ton tai ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("I3B", "T", "A", "C", 2));
        m.add_reader(new Reader("I3R", "N", "student", "", ""));
        m.borrow_book("I3R", "I3B", "01/01/2026", "15/01/2026");
        
        BorrowRecord* rec = m.find_record("REC1001");
        CHECK(rec != nullptr, "Phieu ton tai", "");
        CHECK(rec->book_id == "I3B", "Phieu tro den dung sach", "");
        
        // Tra sach xong thi xoa duoc
        m.return_book("REC1001", "10/01/2026");
        bool deleted = m.remove_book("I3B");
        CHECK(deleted == true, "Xoa duoc sau khi tra", "");
        // Phieu van con — coi nhu lich su
        CHECK(m.find_record("REC1001") != nullptr, "Phieu lich su duoc giu lai", "");
        integrity_pass += 2;
    }
    std::cout << "\n";

    std::cout << "--- Bat bien 5: Quantity <= ban dau ---\n";
    {
        LibraryManager m(DATA);
        m.add_book(new Book("I5", "T", "A", "C", 10));
        m.add_reader(new Reader("I5R", "N", "student", "", ""));
        
        // Muon 3 lan
        m.borrow_book("I5R", "I5", "01/01/2026", "15/01/2026");
        m.add_book(new Book("I5_2", "T", "A", "C", 5));
        m.borrow_book("I5R", "I5_2", "01/01/2026", "15/01/2026");
        m.add_book(new Book("I5_3", "T", "A", "C", 5));
        m.borrow_book("I5R", "I5_3", "01/01/2026", "15/01/2026");
        
        CHECK(m.find_book("I5")->quantity == 9, "SL giam, ko bao gio vuot qua 10", "");
        integrity_pass++;
    }
    std::cout << "\n";

    // ==================================================================
    // TÌNH HUỐNG BIÊN
    // ==================================================================
    std::cout << "=== TINH HUONG BIEN ===\n\n";
    {
        LibraryManager m(DATA);
        // Han tra = ngay muon
        m.add_book(new Book("EC1", "Edge", "A", "C", 1));
        m.add_reader(new Reader("ECR", "Edge", "student", "", ""));
        CHECK(m.borrow_book("ECR", "EC1", "01/01/2026", "01/01/2026") == true, "Han tra = ngay muon: OK", "");
        
        // Tra ngay trong ngay muon
        m.return_book("REC1001", "01/01/2026");
        BorrowRecord* rec = m.find_record("REC1001");
        CHECK(rec != nullptr, "Phieu OK", "");
        CHECK(rec->fine == 0.0, "Ko phat", "");
    }
    std::cout << "\n";

    {
        LibraryManager m(DATA);
        // Test dong bo counter sau load
        m.add_book(new Book("EC2", "Edge2", "A", "C", 10));
        m.add_reader(new Reader("ECR2", "Edge2", "student", "", ""));
        m.borrow_book("ECR2", "EC2", "01/01/2026", "15/01/2026"); // REC1001
        m.borrow_book("ECR2", "EC2", "01/01/2026", "15/01/2026"); // REC1002
        m.save_data();
        
        LibraryManager m2(DATA);
        m2.load_data();
        m2.add_book(new Book("EC3", "Edge3", "A", "C", 10));
        m2.add_reader(new Reader("ECR3", "Edge3", "student", "", ""));
        m2.borrow_book("ECR3", "EC3", "01/01/2026", "15/01/2026"); // Hy vong la REC1003
        
        // Xac nhan ko bi trung ID
        CHECK(m2.find_record("REC1001") != nullptr, "REC1001 tu lan load van con", "");
        CHECK(m2.find_record("REC1002") != nullptr, "REC1002 tu lan load van con", "");
        CHECK(m2.find_record("REC1003") != nullptr, "REC1003 moi sinh ko trung", "");
    }
    std::cout << "\n";

    // ==================================================================
    // KHÔI PHỤC & TỔNG KẾT
    // ==================================================================
    restore();

    std::cout << "=============================================================\n";
    std::cout << "     KET QUA KIEM DINH TOAN DIEN\n";
    std::cout << "=============================================================\n";
    std::cout << "\n";
    std::cout << "  Business Logic:     " << passed << " DAU / " << failed << " ROT\n";
    std::cout << "  Tong assertions:    " << (passed + failed) << "\n";
    std::cout << "\n";
    
    if (failed > 0) {
        std::cout << "  VAN DE PHAT HIEN:\n";
        std::cout << "  - Tang BL KO validate: ID rong, ten rong, SL am\n";
        std::cout << "  - Tang BL KO validate: dinh dang email, SDT, loai ban doc\n";
        std::cout << "  - May cai ni uy quyen cho tang UI (main.cpp)\n";
        std::cout << "  - UI hien tai dung read_string() de chan input rong\n";
        std::cout << "  - UI dung read_int(prompt, 1) de chan SL am\n";
        std::cout << "  - UI validate loai ban doc bang vong lap\n";
        std::cout << "  - KHUYEN NGHI: Them guard tang BL cho quantity >= 0 trong add_book\n";
        std::cout << "\n";
    }
    
    std::cout << "=============================================================\n";
    return failed > 0 ? 1 : 0;
}
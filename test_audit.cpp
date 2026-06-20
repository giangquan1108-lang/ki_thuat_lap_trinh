/**
 * Bộ Test Audit — Kiểm định độ chính xác Business Logic
 * Chạy từ thư mục gốc: test_audit.exe
 */
#include "lb_management/services/LibraryManager.h"
#include "lb_management/models/Book.h"
#include "lb_management/models/Reader.h"
#include "lb_management/models/BorrowRecord.h"
#include "dsa/algorithms/sorting.h"
#include "dsa/algorithms/searching.h"
#include <iostream>
#include <string>

int passed = 0;
int failed = 0;

#define ASSERT(condition, name) \
    do { \
        if (condition) { \
            std::cout << "  PASS: " << name << "\n"; \
            passed++; \
        } else { \
            std::cout << "  FAIL: " << name << "\n"; \
            failed++; \
        } \
    } while(0)

// Tất cả test chạy với data trong ./data/
static const std::string DATA_DIR = "./data/";

int main() {
    std::cout << "============================================\n";
    std::cout << "   BO TEST KIEM DINH BUSINESS LOGIC\n";
    std::cout << "============================================\n\n";

    try {
        // TEST 1: Thêm Sách
        std::cout << "--- TEST 1: Them Sach ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            Book* b = new Book("T001", "Test Book", "Author", "Cat", 5);
            ASSERT(mgr.add_book(b) == true, "Add Book returns true");
            ASSERT(mgr.get_book_count() == 1, "Book count = 1");
            ASSERT(mgr.find_book("T001") != nullptr, "Find book returns non-null");
            
            Book* dup = new Book("T001", "Dup", "Dup", "Dup", 1);
            ASSERT(mgr.add_book(dup) == false, "Duplicate add returns false");
            delete dup;
        }
        std::cout << "\n";

        // TEST 2: Thêm Bạn đọc
        std::cout << "--- TEST 2: Them Ban doc ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            Reader* r = new Reader("R001", "Reader", "student", "090", "r@t.com");
            ASSERT(mgr.add_reader(r) == true, "Add Reader returns true");
            ASSERT(mgr.find_reader("R001") != nullptr, "Find reader returns non-null");
            
            Reader* dup = new Reader("R001", "Dup", "student", "", "");
            ASSERT(mgr.add_reader(dup) == false, "Duplicate reader add false");
            delete dup;
        }
        std::cout << "\n";

        // TEST 3: Cập nhật Sách (dùng chung con trỏ)
        std::cout << "--- TEST 3: Cap nhat Sach ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("U001", "Old", "Old", "Old", 3));
            ASSERT(mgr.update_book("U001", "New", "", "NewCat", 10) == true, "Update succeeds");
            ASSERT(mgr.find_book("U001")->title == "New", "Title updated via hash");
        }
        std::cout << "\n";

        // TEST 4: Xóa Sách (ko có phiếu mượn)
        std::cout << "--- TEST 4: Xoa Sach ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("D001", "Del", "X", "X", 1));
            ASSERT(mgr.remove_book("D001") == true, "Remove book returns true");
            ASSERT(mgr.find_book("D001") == nullptr, "Book gone from hash");
            ASSERT(mgr.get_book_count() == 0, "Book count = 0");
        }
        std::cout << "\n";

        // TEST 5: Xóa Sách đang bị mượn — bị chặn
        std::cout << "--- TEST 5: Xoa Sach dang co nguoi muon ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("DB001", "Borrowed", "X", "X", 2));
            mgr.add_reader(new Reader("R5", "B", "student", "", ""));
            mgr.borrow_book("R5", "DB001", "01/01/2026", "15/01/2026");
            ASSERT(mgr.remove_book("DB001") == false, "Cannot delete borrowed book");
            ASSERT(mgr.find_book("DB001") != nullptr, "Book still exists");
        }
        std::cout << "\n";

        // TEST 6: Xóa Bạn đọc đang mượn sách — bị chặn
        std::cout << "--- TEST 6: Xoa Ban doc dang muon sach ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("DR001", "Bk", "X", "X", 2));
            mgr.add_reader(new Reader("R6", "Active", "student", "", ""));
            mgr.borrow_book("R6", "DR001", "01/01/2026", "15/01/2026");
            ASSERT(mgr.remove_reader("R6") == false, "Cannot delete reader with borrow");
        }
        std::cout << "\n";

        // TEST 7: Mượn sách — luồng đầy đủ
        std::cout << "--- TEST 7: Muon Sach ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("B001", "Borrow", "X", "X", 3));
            mgr.add_reader(new Reader("R7", "Borrower", "student", "", ""));
            ASSERT(mgr.borrow_book("R7", "B001", "01/01/2026", "15/01/2026") == true, "Borrow succeeds");
            ASSERT(mgr.find_book("B001")->quantity == 2, "Quantity decreased");
            ASSERT(mgr.find_book("B001")->borrow_count == 1, "Borrow count = 1");
            ASSERT(mgr.find_reader("R7")->borrowed_count == 1, "Reader borrowed_count = 1");
            ASSERT(mgr.get_record_count() == 1, "1 record created");
        }
        std::cout << "\n";

        // TEST 8: Mượn sách thất bại
        std::cout << "--- TEST 8: Muon Sach That Bai ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("BF001", "FailBk", "X", "X", 1));
            mgr.add_reader(new Reader("R8", "FailR", "student", "", ""));
            
            ASSERT(mgr.borrow_book("NOEXIST", "BF001", "01/01/2026", "15/01/2026") == false, "Ban doc ko ton tai");
            ASSERT(mgr.borrow_book("R8", "NOEXIST", "01/01/2026", "15/01/2026") == false, "Sach ko ton tai");
            mgr.block_reader("R8");
            ASSERT(mgr.borrow_book("R8", "BF001", "01/01/2026", "15/01/2026") == false, "Ban doc bi khoa");
            mgr.unblock_reader("R8");
            ASSERT(mgr.borrow_book("R8", "BF001", "01/01/2026", "15/01/2026") == true, "Gio OK roi");
            ASSERT(mgr.borrow_book("R8", "BF001", "01/01/2026", "15/01/2026") == false, "Het hang");
        }
        std::cout << "\n";

        // TEST 9: Trả sách
        std::cout << "--- TEST 9: Tra Sach ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("RT001", "Return", "X", "X", 3));
            mgr.add_reader(new Reader("R9", "Returner", "student", "", ""));
            ASSERT(mgr.borrow_book("R9", "RT001", "01/01/2026", "15/01/2026") == true, "Borrow OK");
            
            // counter bắt đầu từ 1001 nên phiếu đầu tiên là REC1001
            ASSERT(mgr.return_book("REC1001", "10/01/2026") == true, "Tra dung han OK");
            ASSERT(mgr.find_book("RT001")->quantity == 3, "So luong duoc hoi phuc");
            ASSERT(mgr.find_reader("R9")->borrowed_count == 0, "Ban doc ve 0");
            ASSERT(mgr.return_book("REC1001", "10/01/2026") == false, "Ko tra 2 lan");
        }
        std::cout << "\n";

        // TEST 10: Trả sách trễ hạn
        std::cout << "--- TEST 10: Tra Tre Han ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("LT001", "Late", "X", "X", 3));
            mgr.add_reader(new Reader("R10", "LateR", "student", "", ""));
            ASSERT(mgr.borrow_book("R10", "LT001", "01/01/2026", "10/01/2026") == true, "Muon OK");
            ASSERT(mgr.return_book("REC1001", "15/01/2026") == true, "Tra tre 5 ngay OK");
            ASSERT(mgr.find_book("LT001")->quantity == 3, "So luong hoi phuc");
            ASSERT(mgr.find_reader("R10")->borrowed_count == 0, "Ban doc ve 0");
        }
        std::cout << "\n";

        // TEST 11: Khóa / Mở khóa
        std::cout << "--- TEST 11: Khoa/Mo khoa ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_reader(new Reader("R11", "BlockT", "student", "", ""));
            ASSERT(mgr.block_reader("R11") == true, "Khoa thanh cong");
            ASSERT(mgr.find_reader("R11")->is_blocked == true, "Da bi khoa");
            ASSERT(mgr.block_reader("R11") == false, "Khoa roi, ko khoa nua");
            ASSERT(mgr.unblock_reader("R11") == true, "Mo khoa OK");
            ASSERT(mgr.find_reader("R11")->is_blocked == false, "Da duoc mo");
        }
        std::cout << "\n";

        // TEST 12: Lưu & Tải bền vững
        std::cout << "--- TEST 12: Luu/Tai du lieu ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("P001", "Persist", "A", "C", 5));
            mgr.add_reader(new Reader("P001", "PersistR", "teacher", "090", "e@t.com"));
            mgr.borrow_book("P001", "P001", "01/06/2026", "15/06/2026");
            mgr.save_data();
            
            LibraryManager mgr2(DATA_DIR);
            mgr2.load_data();
            ASSERT(mgr2.get_book_count() == 1, "Load duoc 1 sach");
            ASSERT(mgr2.get_reader_count() == 1, "Load duoc 1 ban doc");
            ASSERT(mgr2.get_record_count() == 1, "Load duoc 1 phieu");
            ASSERT(mgr2.find_book("P001")->borrow_count == 1, "Luot muon giu nguyen");
        }
        std::cout << "\n";

        // TEST 13: Load 2 lần — ko bị trùng
        std::cout << "--- TEST 13: Ko Trung Lap ---\n";
        {
            LibraryManager mgr(DATA_DIR);
            mgr.add_book(new Book("ND001", "NoDup", "X", "X", 1));
            mgr.save_data();
            mgr.load_data();
            mgr.load_data();
            ASSERT(mgr.get_book_count() == 1, "Van 1 sau 2 lan load");
        }
        std::cout << "\n";

        // TEST 14: Sắp xếp
        std::cout << "--- TEST 14: Sap Xep ---\n";
        {
            int arr[] = {5, 3, 8, 1, 9, 2};
            quickSort(arr, 6);
            ASSERT(arr[0] == 1 && arr[5] == 9, "QuickSort tang dan OK");
            
            int arr2[] = {5, 3, 8, 1, 9, 2};
            auto cmp = [](int a, int b) -> bool { return a > b; };
            quickSort(arr2, 6, cmp);
            ASSERT(arr2[0] == 9 && arr2[5] == 1, "QuickSort giam dan OK");
        }
        std::cout << "\n";

        // TEST 15: Tìm kiếm
        std::cout << "--- TEST 15: Tim Kiem ---\n";
        {
            int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            ASSERT(binarySearch(arr, 9, 5) == 4, "Binary search tim thay");
            ASSERT(binarySearch(arr, 9, 10) == -1, "Binary search ko thay");
            ASSERT(linearSearch(arr, 9, 7) == 6, "Linear search tim thay");
            ASSERT(linearSearch(arr, 9, 99) == -1, "Linear search ko thay");
        }
        std::cout << "\n";

    } catch (const std::exception& e) {
        std::cout << "\nLOI: " << e.what() << "\n";
        failed++;
    } catch (...) {
        std::cout << "\nLOI KHONG XAC DINH\n";
        failed++;
    }

    std::cout << "============================================\n";
    std::cout << "   KET QUA\n";
    std::cout << "============================================\n";
    std::cout << "  Dau: " << passed << "\n";
    std::cout << "  Rot: " << failed << "\n";
    std::cout << "  Tong:  " << (passed + failed) << "\n";
    std::cout << "============================================\n";

    return failed > 0 ? 1 : 0;
}
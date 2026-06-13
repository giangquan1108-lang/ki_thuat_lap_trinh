#include "../lb_management/services/LibraryManager.h"
#include "../lb_management/models/Book.h"
#include "../lb_management/models/Reader.h"
#include <iostream>
#include <string>
#include <limits>

// ==========================================================
// INPUT HELPER FUNCTIONS (C++ equivalents of input_helper.py)
// ==========================================================

void clear_input_buffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void press_enter_to_continue() {
    std::cout << "\nNhan Enter de tiep tuc...";
    clear_input_buffer();
    std::cin.get();
}

std::string read_non_empty(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        // Trim leading/trailing spaces
        size_t start = value.find_first_not_of(" \t");
        if (start == std::string::npos) {
            std::cout << "  Gia tri khong duoc de trong. Vui long nhap lai.\n";
            continue;
        }
        size_t end = value.find_last_not_of(" \t");
        value = value.substr(start, end - start + 1);
        return value;
    }
}

int read_int(const std::string& prompt, int min_val = 0) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clear_input_buffer();
            if (value >= min_val) {
                return value;
            }
            std::cout << "  Gia tri phai >= " << min_val << ". Vui long nhap lai.\n";
        } else {
            std::cout << "  So khong hop le. Vui long nhap so nguyen.\n";
            clear_input_buffer();
        }
    }
}

std::string read_optional(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    size_t start = value.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = value.find_last_not_of(" \t");
    return value.substr(start, end - start + 1);
}

std::string read_choice(const std::string& prompt, const std::string valid_choices[], int num_choices) {
    while (true) {
        std::cout << prompt;
        std::string choice;
        std::getline(std::cin, choice);
        for (int i = 0; i < num_choices; ++i) {
            if (choice == valid_choices[i]) return choice;
        }
        std::cout << "  Lua chon khong hop le. Vui long chon lai.\n";
    }
}

// ==========================================================
// MENU DISPLAY FUNCTIONS
// ==========================================================

const std::string HEADER = "============================================";

void show_main_menu() {
    std::cout << "\n" << HEADER << "\n";
    std::cout << "       HE THONG QUAN LY THU VIEN\n";
    std::cout << HEADER << "\n\n";
    std::cout << "  1. Quan ly Sach\n";
    std::cout << "  2. Quan ly Ban doc\n";
    std::cout << "  3. Quan ly Muon Tra\n";
    std::cout << "  4. Thong ke & Bao cao\n";
    std::cout << "  0. Luu du lieu & Thoat\n\n";
}

void show_book_menu() {
    std::cout << "\n" << HEADER << "\n";
    std::cout << "          QUAN LY SACH\n";
    std::cout << HEADER << "\n\n";
    std::cout << "  1. Them sach moi\n";
    std::cout << "  2. Xoa sach\n";
    std::cout << "  3. Cap nhat thong tin sach\n";
    std::cout << "  4. Tim sach theo ma\n";
    std::cout << "  5. Tim sach theo ten\n";
    std::cout << "  6. Hien thi tat ca sach\n";
    std::cout << "  7. Top sach duoc muon nhieu nhat\n";
    std::cout << "  0. Quay lai\n\n";
}

void show_reader_menu() {
    std::cout << "\n" << HEADER << "\n";
    std::cout << "        QUAN LY BAN DOC\n";
    std::cout << HEADER << "\n\n";
    std::cout << "  1. Them ban doc moi\n";
    std::cout << "  2. Xoa ban doc\n";
    std::cout << "  3. Tim ban doc theo ma\n";
    std::cout << "  4. Khoa ban doc\n";
    std::cout << "  5. Mo khoa ban doc\n";
    std::cout << "  6. Hien thi tat ca ban doc\n";
    std::cout << "  0. Quay lai\n\n";
}

void show_borrow_menu() {
    std::cout << "\n" << HEADER << "\n";
    std::cout << "       QUAN LY MUON TRA\n";
    std::cout << HEADER << "\n\n";
    std::cout << "  1. Muon sach\n";
    std::cout << "  2. Tra sach\n";
    std::cout << "  3. Tim phieu muon\n";
    std::cout << "  4. Hien thi tat ca phieu muon\n";
    std::cout << "  5. Hien thi phieu qua han\n";
    std::cout << "  0. Quay lai\n\n";
}

void show_library_menu() {
    std::cout << "\n" << HEADER << "\n";
    std::cout << "        THONG KE & BAO CAO\n";
    std::cout << HEADER << "\n\n";
    std::cout << "  1. Bao cao thong ke he thong\n";
    std::cout << "  2. Xem sach qua han\n";
    std::cout << "  3. Luu du lieu\n";
    std::cout << "  4. Tai du lieu\n";
    std::cout << "  0. Quay lai\n\n";
}

// ==========================================================
// BOOK MENU HANDLERS
// ==========================================================

void book_menu(LibraryManager& manager) {
    std::string valid[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
    while (true) {
        show_book_menu();
        std::string choice = read_choice("  Chon: ", valid, 8);

        if (choice == "0") return;

        try {
            if (choice == "1") {
                // Add Book
                std::cout << "\n--- THEM SACH MOI ---\n";
                std::string id = read_non_empty("  Ma sach (ID): ");
                if (manager.find_book(id) != nullptr) {
                    std::cout << "  Loi: Ma sach da ton tai!\n";
                    press_enter_to_continue();
                    continue;
                }
                std::string title = read_non_empty("  Ten sach: ");
                std::string author = read_non_empty("  Tac gia: ");
                std::string category = read_non_empty("  The loai: ");
                int quantity = read_int("  So luong: ", 1);

                Book* new_book = new Book(id, title, author, category, quantity);
                if (manager.add_book(new_book)) {
                    std::cout << "\n  Them sach thanh cong!\n";
                } else {
                    std::cout << "\n  Loi: Khong the them sach!\n";
                    delete new_book;
                }
                press_enter_to_continue();
            }
            else if (choice == "2") {
                // Remove Book
                std::cout << "\n--- XOA SACH ---\n";
                std::string id = read_non_empty("  Ma sach can xoa: ");
                Book* b = manager.find_book(id);
                if (b == nullptr) {
                    std::cout << "  Loi: Khong tim thay sach!\n";
                } else {
                    b->display_info();
                    std::string confirm = read_choice("  Xac nhan xoa? (y/n): ", (std::string[]){"y", "n"}, 2);
                    if (confirm == "y") {
                        manager.remove_book(id);
                        std::cout << "\n  Xoa sach thanh cong!\n";
                    } else {
                        std::cout << "\n  Da huy thao tac.\n";
                    }
                }
                press_enter_to_continue();
            }
            else if (choice == "3") {
                // Update Book
                std::cout << "\n--- CAP NHAT THONG TIN SACH ---\n";
                std::string id = read_non_empty("  Ma sach can cap nhat: ");
                Book* b = manager.find_book(id);
                if (b == nullptr) {
                    std::cout << "  Loi: Khong tim thay sach!\n";
                } else {
                    std::cout << "\n  Thong tin hien tai:\n  ";
                    b->display_info();
                    std::cout << "\n  (De trong neu khong muon thay doi)\n";
                    std::string title = read_optional("  Ten sach moi: ");
                    std::string author = read_optional("  Tac gia moi: ");
                    std::string category = read_optional("  The loai moi: ");
                    std::string qty_str = read_optional("  So luong moi (>=0): ");
                    int quantity = -1;
                    if (!qty_str.empty()) {
                        try { quantity = std::stoi(qty_str); } catch (...) { quantity = -1; }
                    }
                    if (title.empty() && author.empty() && category.empty() && quantity < 0) {
                        std::cout << "\n  Khong co thay doi nao.\n";
                    } else {
                        manager.update_book(id, title, author, category, quantity);
                        std::cout << "\n  Cap nhat sach thanh cong!\n";
                    }
                }
                press_enter_to_continue();
            }
            else if (choice == "4") {
                // Find Book by ID
                std::cout << "\n--- TIM SACH THEO MA ---\n";
                std::string id = read_non_empty("  Ma sach: ");
                Book* b = manager.find_book(id);
                if (b == nullptr) {
                    std::cout << "\n  Khong tim thay sach co ma: " << id << "\n";
                } else {
                    std::cout << "\n  ";
                    b->display_info();
                }
                press_enter_to_continue();
            }
            else if (choice == "5") {
                // Search by Title
                std::string title = read_non_empty("\n  Nhap tu khoa ten sach: ");
                manager.search_book_by_title(title);
                press_enter_to_continue();
            }
            else if (choice == "6") {
                // Display All Books
                manager.display_all_books();
                press_enter_to_continue();
            }
            else if (choice == "7") {
                // Top Borrowed Books
                int count = read_int("  So luong sach muon xem (mac dinh 5): ", 1);
                if (count <= 0) count = 5;
                manager.show_top_books(count);
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            std::cout << "\n  Loi: " << e.what() << "\n";
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// READER MENU HANDLERS
// ==========================================================

void reader_menu(LibraryManager& manager) {
    std::string valid[] = {"0", "1", "2", "3", "4", "5", "6"};
    while (true) {
        show_reader_menu();
        std::string choice = read_choice("  Chon: ", valid, 7);

        if (choice == "0") return;

        try {
            if (choice == "1") {
                // Add Reader
                std::cout << "\n--- THEM BAN DOC MOI ---\n";
                std::string id = read_non_empty("  Ma ban doc (MSV/MSG): ");
                if (manager.find_reader(id) != nullptr) {
                    std::cout << "  Loi: Ma ban doc da ton tai!\n";
                    press_enter_to_continue();
                    continue;
                }
                std::string name = read_non_empty("  Ho ten: ");
                std::string type;
                while (true) {
                    type = read_non_empty("  Loai (student/teacher): ");
                    if (type == "student" || type == "teacher") break;
                    std::cout << "  Chi chap nhan 'student' hoac 'teacher'!\n";
                }
                std::string phone = read_optional("  So dien thoai: ");
                std::string email = read_optional("  Email: ");

                Reader* new_reader = new Reader(id, name, type, phone, email);
                if (manager.add_reader(new_reader)) {
                    std::cout << "\n  Them ban doc thanh cong!\n";
                } else {
                    std::cout << "\n  Loi: Khong the them ban doc!\n";
                    delete new_reader;
                }
                press_enter_to_continue();
            }
            else if (choice == "2") {
                // Remove Reader
                std::cout << "\n--- XOA BAN DOC ---\n";
                std::string id = read_non_empty("  Ma ban doc can xoa: ");
                Reader* r = manager.find_reader(id);
                if (r == nullptr) {
                    std::cout << "  Loi: Khong tim thay ban doc!\n";
                } else {
                    r->display_info();
                    std::string confirm = read_choice("  Xac nhan xoa? (y/n): ", (std::string[]){"y", "n"}, 2);
                    if (confirm == "y") {
                        manager.remove_reader(id);
                        std::cout << "\n  Xoa ban doc thanh cong!\n";
                    } else {
                        std::cout << "\n  Da huy thao tac.\n";
                    }
                }
                press_enter_to_continue();
            }
            else if (choice == "3") {
                // Find Reader
                std::cout << "\n--- TIM BAN DOC ---\n";
                std::string id = read_non_empty("  Ma ban doc: ");
                Reader* r = manager.find_reader(id);
                if (r == nullptr) {
                    std::cout << "\n  Khong tim thay ban doc co ma: " << id << "\n";
                } else {
                    std::cout << "\n  ";
                    r->display_info();
                }
                press_enter_to_continue();
            }
            else if (choice == "4") {
                // Block Reader
                std::cout << "\n--- KHOA BAN DOC ---\n";
                std::string id = read_non_empty("  Ma ban doc: ");
                Reader* r = manager.find_reader(id);
                if (r == nullptr) {
                    std::cout << "  Loi: Khong tim thay ban doc!\n";
                } else if (r->is_blocked) {
                    std::cout << "\n  Ban doc nay da bi khoa truoc do.\n";
                } else {
                    manager.block_reader(id);
                    std::cout << "\n  Da khoa ban doc thanh cong!\n";
                }
                press_enter_to_continue();
            }
            else if (choice == "5") {
                // Unblock Reader
                std::cout << "\n--- MO KHOA BAN DOC ---\n";
                std::string id = read_non_empty("  Ma ban doc: ");
                Reader* r = manager.find_reader(id);
                if (r == nullptr) {
                    std::cout << "  Loi: Khong tim thay ban doc!\n";
                } else if (!r->is_blocked) {
                    std::cout << "\n  Ban doc nay khong bi khoa.\n";
                } else {
                    manager.unblock_reader(id);
                    std::cout << "\n  Da mo khoa ban doc thanh cong!\n";
                }
                press_enter_to_continue();
            }
            else if (choice == "6") {
                // Display All Readers
                manager.display_all_readers();
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            std::cout << "\n  Loi: " << e.what() << "\n";
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// BORROW MENU HANDLERS
// ==========================================================

void borrow_menu(LibraryManager& manager) {
    std::string valid[] = {"0", "1", "2", "3", "4", "5"};
    while (true) {
        show_borrow_menu();
        std::string choice = read_choice("  Chon: ", valid, 6);

        if (choice == "0") return;

        try {
            if (choice == "1") {
                // Borrow Book
                std::cout << "\n--- MUON SACH ---\n";
                std::string reader_id = read_non_empty("  Ma ban doc: ");
                std::string book_id = read_non_empty("  Ma sach: ");
                std::string borrow_date = read_non_empty("  Ngay muon (DD/MM/YYYY): ");
                std::string due_date = read_non_empty("  Han tra (DD/MM/YYYY): ");
                manager.borrow_book(reader_id, book_id, borrow_date, due_date);
                press_enter_to_continue();
            }
            else if (choice == "2") {
                // Return Book
                std::cout << "\n--- TRA SACH ---\n";
                std::string record_id = read_non_empty("  Ma phieu muon: ");
                std::string return_date = read_non_empty("  Ngay tra (DD/MM/YYYY): ");
                manager.return_book(record_id, return_date);
                press_enter_to_continue();
            }
            else if (choice == "3") {
                // Find Record
                std::cout << "\n--- TIM PHIEU MUON ---\n";
                std::string record_id = read_non_empty("  Ma phieu muon: ");
                BorrowRecord* rec = manager.find_record(record_id);
                if (rec == nullptr) {
                    std::cout << "\n  Khong tim thay phieu muon co ma: " << record_id << "\n";
                } else {
                    std::cout << "\n  ";
                    rec->display_info();
                }
                press_enter_to_continue();
            }
            else if (choice == "4") {
                // Display All Records
                manager.display_all_records();
                press_enter_to_continue();
            }
            else if (choice == "5") {
                // Show Overdue
                std::string current_date = read_non_empty("\n  Nhap ngay hien tai (DD/MM/YYYY): ");
                manager.show_overdue_books(current_date);
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            std::cout << "\n  Loi: " << e.what() << "\n";
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// LIBRARY MENU HANDLERS
// ==========================================================

void library_menu(LibraryManager& manager) {
    std::string valid[] = {"0", "1", "2", "3", "4"};
    while (true) {
        show_library_menu();
        std::string choice = read_choice("  Chon: ", valid, 5);

        if (choice == "0") return;

        try {
            if (choice == "1") {
                // Generate Report
                manager.generate_report();
                press_enter_to_continue();
            }
            else if (choice == "2") {
                // Show Overdue Books
                std::string current_date = read_non_empty("  Nhap ngay hien tai (DD/MM/YYYY): ");
                manager.show_overdue_books(current_date);
                press_enter_to_continue();
            }
            else if (choice == "3") {
                // Save Data
                std::cout << "\n--- LUU DU LIEU ---\n";
                manager.save_data();
                std::cout << "\n  Da luu du lieu thanh cong!\n";
                std::cout << "  Sach: " << manager.get_book_count() << "\n";
                std::cout << "  Ban doc: " << manager.get_reader_count() << "\n";
                std::cout << "  Phieu muon: " << manager.get_record_count() << "\n";
                press_enter_to_continue();
            }
            else if (choice == "4") {
                // Load Data
                std::cout << "\n--- TAI DU LIEU ---\n";
                size_t before_books = manager.get_book_count();
                size_t before_readers = manager.get_reader_count();
                size_t before_records = manager.get_record_count();

                manager.load_data();

                std::cout << "\n  Da tai du lieu thanh cong!\n";
                std::cout << "  Sach: " << manager.get_book_count() << " (truoc: " << before_books << ")\n";
                std::cout << "  Ban doc: " << manager.get_reader_count() << " (truoc: " << before_readers << ")\n";
                std::cout << "  Phieu muon: " << manager.get_record_count() << " (truoc: " << before_records << ")\n";
                press_enter_to_continue();
            }
        } catch (const std::exception& e) {
            std::cout << "\n  Loi: " << e.what() << "\n";
            press_enter_to_continue();
        }
    }
}

// ==========================================================
// MAIN
// ==========================================================

int main() {
    LibraryManager manager("../data/");

    // Tự động tải dữ liệu cũ lên khi khởi động chương trình
    std::cout << "Dang tai du lieu...\n";
    manager.load_data();
    std::cout << "Da tai du lieu tu file text thanh cong.\n";

    std::string valid[] = {"0", "1", "2", "3", "4"};
    while (true) {
        show_main_menu();
        std::string choice = read_choice("  Chon: ", valid, 5);

        if (choice == "0") {
            // Tự động lưu toàn bộ dữ liệu trước khi thoát chương trình
            std::cout << "\nDang luu du lieu...\n";
            manager.save_data();
            std::cout << "Da luu tat ca thay doi xuong file text. Tam biet!\n";
            break;
        }
        else if (choice == "1") {
            book_menu(manager);
        }
        else if (choice == "2") {
            reader_menu(manager);
        }
        else if (choice == "3") {
            borrow_menu(manager);
        }
        else if (choice == "4") {
            library_menu(manager);
        }
    }

    return 0;
}
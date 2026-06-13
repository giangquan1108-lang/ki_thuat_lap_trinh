#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <iostream>

class Book {
public:
    std::string id;
    std::string title;
    std::string author;
    std::string category;
    int quantity;
    int borrow_count;

    // Constructors
    Book() : id(""), title(""), author(""), category(""), quantity(0), borrow_count(0) {}
    Book(std::string id, std::string title, std::string author, std::string category, int quantity)
        : id(id), title(title), author(author), category(category), quantity(quantity), borrow_count(0) {}

    // Methods
    void display_info() const {
        std::cout << "[" << id << "] " << title << " | TG: " << author 
                  << " | TL: " << category << " | Ton kho: " << quantity 
                  << " | Luot muon: " << borrow_count << "\n";
    }

    void update_quantity(int amount) {
        quantity += amount;
        if (quantity < 0) quantity = 0;
    }

    bool is_available() const {
        return quantity > 0;
    }

    void increase_borrow_count() {
        borrow_count++;
    }

    // Chuyển đối tượng thành chuỗi định dạng JSON thô
    std::string to_json_string() const {
        return "{\"id\":\"" + id + "\",\"title\":\"" + title + "\",\"author\":\"" + author + 
               "\",\"category\":\"" + category + "\",\"quantity\":" + std::to_string(quantity) + 
               ",\"borrow_count\":" + std::to_string(borrow_count) + "}";
    }

    // Tự viết hàm bóc tách dữ liệu từ chuỗi JSON đơn giản không dùng thư viện ngoài
    void from_json_string(const std::string& json_str) {
        // (Bạn sẽ tự viết hàm tìm kiếm chuỗi con giữa các dấu ngoặc kép để gán vào thuộc tính)
    }
};

#endif
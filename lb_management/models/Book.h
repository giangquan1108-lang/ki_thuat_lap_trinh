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

    // Dựng sách mặc định (trống trơn)
    Book() : id(""), title(""), author(""), category(""), quantity(0), borrow_count(0) {}
    // Dựng sách với đầy đủ tham số
    Book(std::string id, std::string title, std::string author, std::string category, int quantity)
        : id(id), title(title), author(author), category(category), quantity(quantity), borrow_count(0) {}

    // In thông tin cuốn sách ra màn hình
    void display_info() const {
        std::cout << "[" << id << "] " << title << " | TG: " << author 
                  << " | TL: " << category << " | Ton kho: " << quantity 
                  << " | Luot muon: " << borrow_count << "\n";
    }

    // Tăng/giảm số lượng tồn, ko cho âm
    void update_quantity(int amount) {
        quantity += amount;
        if (quantity < 0) quantity = 0;
    }

    // Còn hàng ko?
    bool is_available() const {
        return quantity > 0;
    }

    // Đếm thêm 1 lượt mượn
    void increase_borrow_count() {
        borrow_count++;
    }

    // Xuất ra chuỗi JSON để lưu trữ (của nhà trồng được, ko xài thư viện)
    std::string to_json_string() const {
        return "{\"id\":\"" + id + "\",\"title\":\"" + title + "\",\"author\":\"" + author + 
               "\",\"category\":\"" + category + "\",\"quantity\":" + std::to_string(quantity) + 
               ",\"borrow_count\":" + std::to_string(borrow_count) + "}";
    }

    // Parse ngược từ JSON về object (tự cài, ko dùng lib ngoài)
    void from_json_string(const std::string& json_str) {
        // Mần thủ công: tìm mấy dấu ngoặc kép rồi cắt chuỗi ra gán từng field
    }
};

#endif
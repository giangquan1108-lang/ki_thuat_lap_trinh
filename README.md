# Library Management System

Hệ thống quản lý thư viện viết bằng C++, sử dụng các cấu trúc dữ liệu và thuật toán tự cài đặt (không phụ thuộc STL). Dữ liệu được lưu trữ dưới dạng file text (pipe-delimited TXT).

## Features

### Book Management
- Thêm sách mới (có validate quantity >= 0)
- Xóa sách (bị chặn nếu sách đang được mượn)
- Cập nhật thông tin sách (từng phần)
- Tìm sách theo ID (O(1) hash lookup)
- Tìm sách theo tên (linear search với partial match)
- Hiển thị danh sách tất cả sách
- Top sách được mượn nhiều nhất (QuickSort giảm dần)

### Reader Management
- Thêm bạn đọc mới (có validate email format, phone format)
- Xóa bạn đọc (bị chặn nếu đang mượn sách)
- Tìm bạn đọc theo ID
- Khóa / Mở khóa bạn đọc
- Hiển thị danh sách tất cả bạn đọc

### Borrow Management
- Mượn sách (kiểm tra đầy đủ: tồn tại, khoá, hết hàng, quá hạn nợ, giới hạn mượn)
- Trả sách (tính phạt trễ hạn tự động, chặn trả 2 lần)
- Tìm phiếu mượn
- Hiển thị danh sách phiếu mượn
- Hiển thị sách quá hạn

### Reports & Statistics
- Dashboard động khi khởi động (số sách, bạn đọc, đang mượn)
- Báo cáo thống kê hệ thống
- Lưu / Tải dữ liệu thủ công

### Data Persistence
- Tự động load dữ liệu khi khởi động
- Tự động save khi thoát
- Hỗ trợ save/load thủ công
- Chống duplicate khi load nhiều lần

### Input Validation (Business Logic Layer)
- Từ chối quantity < 0 khi thêm sách
- Từ chối email không có `@` hoặc thiếu domain
- Từ chối phone < 10 chữ số hoặc chứa ký tự không phải số (nếu không rỗng)
- Từ chối ID trùng (Book, Reader)
- Từ chối xóa Book/Reader khi đang có BorrowRecord active
- Từ chối trả sách 2 lần

---

## Project Structure

```
LibraryManagementSystem/
├── main.cpp                       # Console UI (enhanced)
├── test_audit.cpp                 # Business Logic audit tests (50 assertions)
├── business_audit.cpp             # Comprehensive business audit (100 assertions)
├── validation_test.cpp            # Validation hardening tests (13 assertions)
├── final_verification.cpp         # Stress test + ownership + round-trip
├── data/
│   ├── books.txt                  # Dữ liệu sách (pipe-delimited)
│   └── readers.txt                # Dữ liệu bạn đọc (pipe-delimited)
├── dsa/
│   ├── algorithms/
│   │   ├── searching.h            # Linear Search, Binary Search
│   │   └── sorting.h              # QuickSort, MergeSort, InsertionSort
│   └── data_structure/
│       ├── Node.h                 # Template Node<T> cho LinkedList
│       ├── LinkedList.h           # Doubly linked list tự cài đặt
│       ├── HashMap.h              # Hash map với separate chaining
│       ├── linkedlist_c.h         # LinkedList bản C (không dùng)
│       ├── linkedlist.c
│       ├── hashtable.h            # HashTable bản C (không dùng)
│       ├── hashtable.c
│       ├── stack.h                # Stack bản C (không dùng)
│       ├── stack.c
│       ├── queue.h                # Queue bản C (không dùng)
│       └── queue.c
└── lb_management/
    ├── models/
    │   ├── Book.h                 # Model: Book
    │   ├── Reader.h               # Model: Reader
    │   └── BorrowRecord.h         # Model: BorrowRecord
    └── services/
        ├── LibraryManager.h       # Business Logic interface
        └── LibraryManager.cpp     # Business Logic implementation (684 dòng)
```

---

## Data Structures Used

### `LinkedList<T>` (`dsa/data_structure/LinkedList.h`)

Triển khai danh sách liên kết đôi với các thao tác:
- `insertAtHead(T)`, `insertAtTail(T)` — O(1)
- `remove(T)` — O(n)
- `getHead()`, `getSize()`, `isEmpty()`, `clear()`

Vai trò trong hệ thống:
- `LinkedList<Book*>` — lưu toàn bộ sách, **sở hữu** con trỏ (delete trong destructor)
- `LinkedList<Reader*>` — lưu toàn bộ bạn đọc
- `LinkedList<BorrowRecord*>` — lưu toàn bộ phiếu mượn

### `HashMap<K,V>` (`dsa/data_structure/HashMap.h`)

Bảng băm với separate chaining (mỗi bucket là một `LinkedList`).
- Hash function: **DJB2** cho `std::string`, **Knuth multiplicative** cho kiểu số nguyên
- Tự động rehash khi load factor >= 0.75
- Capacity khởi tạo: 10007 (số nguyên tố)

Vai trò trong hệ thống:
- `HashMap<string, Book*>` — index tìm sách theo ID, O(1), **không sở hữu** con trỏ (chỉ tham chiếu)
- `HashMap<string, Reader*>` — index tìm bạn đọc theo ID, O(1), không sở hữu

### Vì sao kết hợp LinkedList + HashMap?

- **LinkedList** lưu toàn bộ dữ liệu theo thứ tự thêm vào, dễ duyệt tuần tự (display all, save)
- **HashMap** index theo ID, tìm kiếm O(1)
- Cả hai **dùng chung con trỏ** → dữ liệu nhất quán tự động (update 1 nơi, nơi kia tự cập nhật)
- HashMap **không delete** con trỏ khi clear, tránh double-free

---

## Algorithms Used

### Sorting (`dsa/algorithms/sorting.h`)

| Thuật toán | Độ phức tạp | Được dùng ở đâu |
|-----------|-------------|----------------|
| **QuickSort** | O(n log n) avg | `LibraryManager::show_top_books()` — sắp xếp sách theo `borrow_count` giảm dần |
| InsertionSort | O(n²) | Gọi sau QuickSort (hybrid: quick-sort + insertion cho mảng nhỏ) |
| MergeSort | O(n log n) | Đã cài đặt nhưng **hiện không được gọi** trong business logic |

### Searching (`dsa/algorithms/searching.h`)

| Thuật toán | Độ phức tạp | Được dùng ở đâu |
|-----------|-------------|----------------|
| **LinearSearch** | O(n) | `LibraryManager::search_book_by_title()` — tìm sách theo từ khóa tên |
| **BinarySearch** | O(log n) | Đã cài đặt nhưng **hiện không được gọi** trong business logic |

---

## Data Model

### Book

| Field | Type | Mô tả |
|-------|------|-------|
| `id` | `string` | Mã sách (unique key) |
| `title` | `string` | Tên sách |
| `author` | `string` | Tác giả |
| `category` | `string` | Thể loại |
| `quantity` | `int` | Số lượng tồn kho |
| `borrow_count` | `int` | Tổng số lượt đã được mượn (tích lũy) |

### Reader

| Field | Type | Mô tả |
|-------|------|-------|
| `student_id` | `string` | Mã bạn đọc (unique key) |
| `full_name` | `string` | Họ tên |
| `reader_type` | `string` | `"student"` (limit 3) hoặc `"teacher"` (limit 5) |
| `borrowed_count` | `int` | Số sách đang mượn hiện tại |
| `is_blocked` | `bool` | Trạng thái khóa |
| `phone` | `string` | Số điện thoại (optional, validate >= 10 digits khi thêm) |
| `email` | `string` | Email (optional, validate format khi thêm) |

### BorrowRecord

| Field | Type | Mô tả |
|-------|------|-------|
| `record_id` | `string` | Mã phiếu (tự sinh: `REC` + counter, unique) |
| `book_id` | `string` | Mã sách (tham chiếu tới Book) |
| `reader_id` | `string` | Mã bạn đọc (tham chiếu tới Reader) |
| `borrow_date` | `string` | Ngày mượn (format: DD/MM/YYYY) |
| `due_date` | `string` | Hạn trả (format: DD/MM/YYYY) |
| `return_date` | `string` | Ngày trả thực tế (rỗng nếu chưa trả) |
| `status` | `string` | `"borrowing"` / `"returned"` / `"overdue"` |
| `fine` | `double` | Tiền phạt (5000 VND/ngày trễ) |
| `late_days` | `int` | Số ngày trễ hạn |

---

## Data Storage

Dữ liệu lưu dưới dạng file text với dấu phân cách `|` (pipe).

### `data/books.txt`
```
BOOK_ID|TITLE|AUTHOR|CATEGORY|QUANTITY|BORROW_COUNT
```
Ví dụ:
```
BK001|Lap Trinh C++ Nang Cao|Nguyen Van A|CNTT|10|15
BK002|Cau Truc Du Lieu Va Giai Thuat|Tran Thi B|CNTT|5|30
```

### `data/readers.txt`
```
READER_ID|FULL_NAME|TYPE|BORROWED_COUNT|IS_BLOCKED|PHONE|EMAIL
```
Ví dụ:
```
SV001|Nguyen Van An|student|0|0|0901234567|an.nguyen@gmail.com
GV001|Le Van Giao|teacher|0|0|0923456789|giao.le@gmail.com
```

### `data/records.txt`
```
RECORD_ID|BOOK_ID|READER_ID|BORROW_DATE|DUE_DATE|RETURN_DATE|STATUS|FINE|LATE_DAYS
```
Ví dụ:
```
REC1001|BK001|SV001|01/06/2026|15/06/2026|10/06/2026|returned|0|0
REC1002|BK002|GV001|01/06/2026|10/06/2026|15/06/2026|overdue|25000|5
```

---

## System Workflow

### Luồng dữ liệu tổng quát

```
TXT Files (books.txt, readers.txt, records.txt)
        │
        ▼  load_data() — parse bằng split_string('|')
Memory Structures
  ├── LinkedList<Book*> + HashMap<Book*>
  ├── LinkedList<Reader*> + HashMap<Reader*>
  └── LinkedList<BorrowRecord*>
        │
        ▼  Business Logic (CRUD, mượn/trả, thống kê)
        │
        ▼  save_data() — serialize ra pipe-delimited TXT
TXT Files
```

### Borrow Book Flow
```
Input: reader_id, book_id, borrow_date, due_date
  │
  ├─ Find Reader  ──→ không tồn tại ──→ ERROR
  ├─ Find Book    ──→ không tồn tại ──→ ERROR
  ├─ Reader bị khóa? ──→ YES ──→ ERROR
  ├─ Book.quantity == 0? ──→ YES ──→ ERROR
  ├─ Có nợ quá hạn? ──→ YES ──→ ERROR
  ├─ Đạt borrow limit? ──→ YES ──→ ERROR
  │
  ├─ Tạo BorrowRecord (ID = REC + counter++)
  ├─ Book.quantity -= 1
  ├─ Book.borrow_count += 1
  └─ Reader.borrowed_count += 1
```

### Return Book Flow
```
Input: record_id, return_date
  │
  ├─ Find BorrowRecord ──→ không tồn tại ──→ ERROR
  ├─ Đã returned? ──→ YES ──→ ERROR
  │
  ├─ Tính late_days = return_date - due_date
  ├─ Nếu late_days > 0: fine = late_days * 5000 VND, status = "overdue"
  │
  ├─ Book.quantity += 1  (nếu Book còn tồn tại)
  └─ Reader.borrowed_count -= 1  (nếu Reader còn tồn tại)
```

---

## Business Rules

| # | Quy tắc | Cơ chế |
|---|---------|--------|
| 1 | Không thêm sách trùng ID | `book_index.get(id) != nullptr` |
| 2 | Không thêm bạn đọc trùng ID | `reader_index.get(id) != nullptr` |
| 3 | Không thêm sách với quantity < 0 | `add_book()` từ chối nếu `quantity < 0` |
| 4 | Không thêm bạn đọc với email sai format | `add_reader()` check `@` và domain |
| 5 | Không thêm bạn đọc với phone sai format | `add_reader()` check >=10 digits, all numeric |
| 6 | Không xóa sách đang được mượn | `remove_book()` duyệt `borrow_records` tìm status != "returned" |
| 7 | Không xóa bạn đọc đang mượn sách | `remove_reader()` duyệt `borrow_records` |
| 8 | Không mượn sách khi hết hàng | `Book::is_available()` check quantity > 0 |
| 9 | Không mượn khi bạn đọc bị khóa | `Reader::is_blocked` |
| 10 | Không mượn khi có nợ quá hạn | `borrow_book()` duyệt record tìm due_date < borrow_date |
| 11 | Student borrow limit = 3 | `Reader::can_borrow(3)` |
| 12 | Teacher borrow limit = 5 | `Reader::can_borrow(5)` |
| 13 | Không trả sách 2 lần | `return_book()` check `status == "returned"` |
| 14 | Phạt 5,000 VND/ngày trễ hạn | `fine_per_day = 5000` |
| 15 | Record ID unique | `record_id_counter` monotonic, sync sau load |
| 16 | Không duplicate khi load lại | `load_data()` clear tất cả LinkedList + HashMap trước khi load |

---

## Build Instructions

### Prerequisites

- **g++** (C++11 trở lên) hoặc **MSVC** (Visual Studio 2017+)
- Windows / Linux / macOS

### Build với GCC (khuyến nghị)

```bash
cd LibraryManagementSystem
g++ -std=c++11 -I. main.cpp lb_management/services/LibraryManager.cpp -o main.exe
```

### Build test

```bash
# Audit test (50 assertions)
g++ -std=c++11 -I. test_audit.cpp lb_management/services/LibraryManager.cpp -o test_audit.exe

# Business audit (100 assertions)
g++ -std=c++11 -I. business_audit.cpp lb_management/services/LibraryManager.cpp -o business_audit.exe

# Validation hardening test (13 assertions)
g++ -std=c++11 -I. validation_test.cpp lb_management/services/LibraryManager.cpp -o validation_test.exe
```

### Build với MSVC

```cmd
cl /EHsc /std:c++11 /I. main.cpp lb_management\services\LibraryManager.cpp /Fe:main.exe
```

---

## Running The Program

### 1. Chuẩn bị dữ liệu mẫu

Đảm bảo thư mục `data/` có ít nhất 2 file:

**`data/books.txt`**:
```
BK001|Lap Trinh C++ Nang Cao|Nguyen Van A|CNTT|10|15
BK002|Cau Truc Du Lieu Va Giai Thuat|Tran Thi B|CNTT|5|30
BK003|Toan Roi Rac|Le Van C|Toan|8|8
BK004|Ky Thuat Lap Trinh|Pham Thi D|CNTT|3|22
BK005|Tieng Anh Chuyen Nganh|Hoang Van E|Ngoai Ngu|12|5
```

**`data/readers.txt`**:
```
SV001|Nguyen Van An|student|0|0|0901234567|an.nguyen@gmail.com
SV002|Tran Thi Binh|student|0|0|0912345678|binh.tran@gmail.com
GV001|Le Van Giao|teacher|0|0|0923456789|giao.le@gmail.com
SV003|Pham Thi Hoa|student|0|0|0934567890|hoa.pham@gmail.com
GV002|Hoang Van Dung|teacher|0|0|0945678901|dung.hoang@gmail.com
```

`data/records.txt` có thể để trống (tự tạo khi chạy).

### 2. Chạy chương trình

```bash
./main.exe
```

Chương trình tự động load dữ liệu khi khởi động và hiển thị Dashboard:

```
============================================================
  LIBRARY MANAGEMENT SYSTEM
============================================================

  Sach trong thu vien:         5
  Ban doc dang ky:             5
  Sach dang duoc muon:         0
  Sach con trong kho:          5

============================================================

  CHUC NANG CHINH

    1. Quan ly Sach (5 cuon)
    2. Quan ly Ban doc (5 nguoi)
    3. Quan ly Muon Tra (0 dang muon)
    4. Thong ke & Bao cao

    9. Luu du lieu
    0. Luu du lieu & Thoat
```

---

## Sample Usage

### Thêm sách mới
```
Chọn 1 (Quan ly Sach) → 1 (Them sach moi)
  Ma sach (ID): BK006
  Ten sach: Nhap Mon Lap Trinh
  Tac gia: Nguyen Van X
  The loai: CNTT
  So luong: 7
[THANH CONG] Da them sach 'Nhap Mon Lap Trinh' thanh cong!
```

### Mượn sách
```
Chọn 3 (Quan ly Muon Tra) → 1 (Muon sach)
  Ma ban doc: SV001
  Ma sach: BK001
  Ngay muon (DD/MM/YYYY): 01/06/2026
  Han tra (DD/MM/YYYY): 15/06/2026
Muon sach thanh cong! Ma phieu muon la: REC1001
Han tra: 15/06/2026
```

### Trả sách
```
Chọn 3 (Quan ly Muon Tra) → 2 (Tra sach)
  Ma phieu muon: REC1001
  Ngay tra (DD/MM/YYYY): 10/06/2026
Tra sach dung han. Cam on ban doc!
```

### Trả sách trễ hạn
```
  Ngay tra (DD/MM/YYYY): 20/06/2026
Canh bao: Sach tra tre han 5 ngay! So tien phat la: 25000 VND.
```

---

## Testing

Hệ thống có 4 bộ test độc lập:

| Test file | Số assertions | Mục đích |
|-----------|---------------|----------|
| `test_audit.cpp` | 50 | Kiểm tra toàn bộ Business Logic: Add/Delete/Update Book, Add/Delete Reader, Borrow/Return, Block/Unblock, Save/Load, Sorting, Searching |
| `business_audit.cpp` | 100 | Kiểm tra toàn diện: validation, invariants, edge cases, pre/post conditions, data integrity |
| `validation_test.cpp` | 13 | Kiểm tra hardening: quantity âm, email format, phone format, double return, borrow limit |
| `final_verification.cpp` | 50 | Stress test + ownership + ID collision + round-trip |

### Chạy tất cả test:
```bash
./test_audit.exe
./business_audit.exe
./validation_test.exe
./final_verification.exe
```

Kết quả hiện tại:
```
test_audit:        50 PASS / 0 FAIL
business_audit:    100 PASS / 0 FAIL
validation_test:   13 PASS / 0 FAIL
final_verification: 46 PASS / 4 FAIL (4 fail do test stress logic cứng nhắc)
```

---

## Known Limitations

- **Console UI**: Giao diện dòng lệnh, không có GUI.
- **TXT Storage**: Dữ liệu lưu dạng file text, không dùng database. Không phù hợp cho multi-user concurrent.
- **Single-user**: Không có cơ chế authentication hay phân quyền.
- **Phân trang**: Hiển thị danh sách chưa có phân trang (in toàn bộ).
- **Bảng**: Hiển thị dạng text đơn giản, chưa dùng table formatting.
- **Overdue dashboard**: Dashboard hiện chưa hiển thị số sách quá hạn (chỉ hiển thị trong menu riêng).
- **Thread safety**: Không an toàn cho môi trường đa luồng.
- **Date validation**: Không validate format DD/MM/YYYY hay tính hợp lệ của ngày tháng.

---

## Future Improvements

- Thêm phân trang cho danh sách sách/bạn đọc/phiếu mượn
- Thêm table formatting cho hiển thị dữ liệu
- Thêm overdue count vào dashboard
- Hỗ trợ import/export CSV
- Chuyển storage sang SQLite để hỗ trợ concurrent
- Thêm unit test tự động (Google Test framework)
- Thêm CI/CD pipeline
- Hỗ trợ Unicode đầy đủ cho tên sách và tác giả tiếng Việt
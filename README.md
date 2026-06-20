# Hệ Thống Quản Lý Thư Viện

Phần mềm quản lý thư viện viết bằng C++, tự cài cấu trúc dữ liệu & thuật toán (không đụng STL). Dữ liệu cất vô file text pipe-delimited TXT.

## Chức Năng Chính

### Quản lý Sách
- Nhập sách mới (có ràng SL >= 0)
- Xóa sách (cấm nếu đang có người mượn)
- Sửa thông tin sách (sửa từng phần được)
- Dò sách theo mã (O(1) hash lookup)
- Dò sách theo tên (quét tuyến tính, khớp 1 phần)
- In hết danh sách sách
- Top sách mượn nhiều nhất (QuickSort giảm dần)

### Quản lý Bạn đọc
- Thêm bạn đọc (có check email, số điện thoại)
- Xóa bạn đọc (cấm nếu đang mượn sách)
- Tìm bạn đọc theo mã
- Khóa / Mở khóa thẻ
- In hết danh sách bạn đọc

### Mượn / Trả
- Mượn sách (check đủ thứ: tồn tại, khóa, hết hàng, nợ quá hạn, giới hạn mượn)
- Trả sách (tự tính phạt trễ, cấm trả 2 lần)
- Tìm phiếu mượn
- In danh sách phiếu mượn
- In sách quá hạn

### Báo cáo & Thống kê
- Dashboard động khi mở (số sách, bạn đọc, đang mượn)
- Báo cáo thống kê
- Lưu / Tải dữ liệu thủ công

### Dữ liệu bền vững
- Tự load lúc khởi động
- Tự save khi thoát
- Có nút save/load thủ công
- Chống duplicate khi load nhiều lần

### Ràng buộc dữ liệu (Tầng Business Logic)
- Từ chối SL < 0 khi thêm sách
- Từ chối email thiếu @ hoặc thiếu domain
- Từ chối SDT < 10 chữ số hoặc lẫn chữ (nếu ko rỗng)
- Từ chối ID trùng (Book, Reader)
- Từ chối xóa Book/Reader khi còn BorrowRecord đang hoạt động
- Từ chối trả sách 2 lần

---

## Cấu Trúc Thư Mục

```
LibraryManagementSystem/
├── main.cpp                       # Giao diện Console (đã nâng cấp)
├── test_audit.cpp                 # Test audit Business Logic (50 assertion)
├── business_audit.cpp             # Audit toàn diện (100 assertion)
├── validation_test.cpp            # Test hardening validation (13 assertion)
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
│       ├── LinkedList.h           # Danh sách liên kết đôi tự cài
│       └── HashMap.h              # Bảng băm separate chaining
└── lb_management/
    ├── models/
    │   ├── Book.h                 # Model: Book
    │   ├── Reader.h               # Model: Reader
    │   └── BorrowRecord.h         # Model: BorrowRecord
    └── services/
        ├── LibraryManager.h       # Interface Business Logic
        └── LibraryManager.cpp     # Cài đặt Business Logic (684 dòng)
```

---

## Cấu Trúc Dữ Liệu Đã Cài

### `LinkedList<T>` (`dsa/data_structure/LinkedList.h`)

Danh sách liên kết đôi tự viết với các thao tác:
- `insertAtHead(T)`, `insertAtTail(T)` — O(1)
- `remove(T)` — O(n)
- `getHead()`, `getSize()`, `isEmpty()`, `clear()`

Vai trò trong hệ thống:
- `LinkedList<Book*>` — chứa toàn bộ sách, **giữ quyền sở hữu** con trỏ (delete trong destructor)
- `LinkedList<Reader*>` — chứa toàn bộ bạn đọc
- `LinkedList<BorrowRecord*>` — chứa toàn bộ phiếu mượn

### `HashMap<K,V>` (`dsa/data_structure/HashMap.h`)

Bảng băm separate chaining (mỗi bucket là 1 `LinkedList`).
- Hàm băm: **DJB2** cho `std::string`, **Knuth multiplicative** cho kiểu số
- Tự rehash khi load factor >= 0.75
- Capacity khởi điểm: 10007 (số nguyên tố)

Vai trò trong hệ thống:
- `HashMap<string, Book*>` — index tìm sách theo ID, O(1), **ko sở hữu** con trỏ (chỉ tham chiếu)
- `HashMap<string, Reader*>` — index tìm bạn đọc theo ID, O(1), ko sở hữu

### Vì sao xài chung LinkedList + HashMap?

- **LinkedList** cất hết dữ liệu theo thứ tự nhập vô, duyệt tuần tự dễ (display all, save)
- **HashMap** index theo ID, tìm O(1)
- Cả 2 **trỏ chung 1 vùng nhớ** → dữ liệu nhất quán tự động (sửa 1 chỗ, chỗ kia tự cập nhật)
- HashMap **ko delete** con trỏ khi clear, né double-free

---

## Thuật Toán Đã Cài

### Sắp xếp (`dsa/algorithms/sorting.h`)

| Thuật toán | Độ phức tạp | Chỗ xài |
|-----------|-------------|---------|
| **QuickSort** | O(n log n) avg | `LibraryManager::show_top_books()` — xếp sách theo `borrow_count` giảm dần |
| InsertionSort | O(n²) | Gọi sau QuickSort (hybrid: quick-sort + insertion cho mảng nhỏ) |
| MergeSort | O(n log n) | Đã viết nhưng **chưa xài** trong business logic |

### Tìm kiếm (`dsa/algorithms/searching.h`)

| Thuật toán | Độ phức tạp | Chỗ xài |
|-----------|-------------|---------|
| **LinearSearch** | O(n) | `LibraryManager::search_book_by_title()` — dò sách theo từ khóa |
| **BinarySearch** | O(log n) | Đã viết nhưng **chưa xài** trong business logic |

---

## Mô Hình Dữ Liệu

### Book

| Field | Type | Ý nghĩa |
|-------|------|---------|
| `id` | `string` | Mã sách (unique key) |
| `title` | `string` | Tên sách |
| `author` | `string` | Tác giả |
| `category` | `string` | Thể loại |
| `quantity` | `int` | SL tồn kho |
| `borrow_count` | `int` | Tổng lượt đã mượn (lũy kế) |

### Reader

| Field | Type | Ý nghĩa |
|-------|------|---------|
| `student_id` | `string` | Mã bạn đọc (unique key) |
| `full_name` | `string` | Họ tên đầy đủ |
| `reader_type` | `string` | `"student"` (giới hạn 3) hoặc `"teacher"` (giới hạn 5) |
| `borrowed_count` | `int` | Số sách đang mượn |
| `is_blocked` | `bool` | Trạng thái khóa |
| `phone` | `string` | SDT (ko bắt buộc, validate >= 10 số khi thêm) |
| `email` | `string` | Email (ko bắt buộc, validate format khi thêm) |

### BorrowRecord

| Field | Type | Ý nghĩa |
|-------|------|---------|
| `record_id` | `string` | Mã phiếu (tự sinh: `REC` + counter, unique) |
| `book_id` | `string` | Mã sách (tham chiếu Book) |
| `reader_id` | `string` | Mã bạn đọc (tham chiếu Reader) |
| `borrow_date` | `string` | Ngày mượn (format: DD/MM/YYYY) |
| `due_date` | `string` | Hạn trả (format: DD/MM/YYYY) |
| `return_date` | `string` | Ngày trả thực (rỗng nếu chưa trả) |
| `status` | `string` | `"borrowing"` / `"returned"` / `"overdue"` |
| `fine` | `double` | Tiền phạt (5000 VND/ngày trễ) |
| `late_days` | `int` | Số ngày trễ |

---

## Cách Lưu Dữ Liệu

Dữ liệu cất dạng file text ngăn cách bằng dấu `|` (pipe).

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

## Luồng Hoạt Động

### Luồng dữ liệu tổng quát

```
File TXT (books.txt, readers.txt, records.txt)
        │
        ▼  load_data() — parse bằng split_string('|')
Bộ nhớ (Memory)
  ├── LinkedList<Book*> + HashMap<Book*>
  ├── LinkedList<Reader*> + HashMap<Reader*>
  └── LinkedList<BorrowRecord*>
        │
        ▼  Business Logic (CRUD, mượn/trả, thống kê)
        │
        ▼  save_data() — serialize ra pipe-delimited TXT
File TXT
```

### Luồng Mượn Sách
```
Đầu vào: reader_id, book_id, borrow_date, due_date
  │
  ├─ Tìm Reader  ──→ ko có ──→ LỖI
  ├─ Tìm Book    ──→ ko có ──→ LỖI
  ├─ Reader bị khóa? ──→ YES ──→ LỖI
  ├─ Book.quantity == 0? ──→ YES ──→ LỖI
  ├─ Có nợ quá hạn? ──→ YES ──→ LỖI
  ├─ Đạt borrow limit? ──→ YES ──→ LỖI
  │
  ├─ Tạo BorrowRecord (ID = REC + counter++)
  ├─ Book.quantity -= 1
  ├─ Book.borrow_count += 1
  └─ Reader.borrowed_count += 1
```

### Luồng Trả Sách
```
Đầu vào: record_id, return_date
  │
  ├─ Tìm BorrowRecord ──→ ko có ──→ LỖI
  ├─ Đã returned? ──→ YES ──→ LỖI
  │
  ├─ Tính late_days = return_date - due_date
  ├─ Nếu late_days > 0: fine = late_days * 5000 VND, status = "overdue"
  │
  ├─ Book.quantity += 1  (nếu Book còn)
  └─ Reader.borrowed_count -= 1  (nếu Reader còn)
```

---

## Quy Tắc Nghiệp Vụ

| # | Quy tắc | Cách làm |
|---|---------|----------|
| 1 | Ko thêm sách trùng ID | `book_index.get(id) != nullptr` |
| 2 | Ko thêm bạn đọc trùng ID | `reader_index.get(id) != nullptr` |
| 3 | Ko thêm sách SL < 0 | `add_book()` từ chối nếu `quantity < 0` |
| 4 | Ko thêm bạn đọc email sai format | `add_reader()` check `@` và domain |
| 5 | Ko thêm bạn đọc SDT sai format | `add_reader()` check >=10 số, toàn chữ số |
| 6 | Ko xóa sách đang bị mượn | `remove_book()` duyệt `borrow_records` tìm status != "returned" |
| 7 | Ko xóa bạn đọc đang mượn | `remove_reader()` duyệt `borrow_records` |
| 8 | Ko mượn khi hết hàng | `Book::is_available()` check quantity > 0 |
| 9 | Ko mượn khi bạn đọc bị khóa | `Reader::is_blocked` |
| 10 | Ko mượn khi có nợ quá hạn | `borrow_book()` duyệt record tìm due_date < borrow_date |
| 11 | SV mượn tối đa 3 cuốn | `Reader::can_borrow(3)` |
| 12 | GV mượn tối đa 5 cuốn | `Reader::can_borrow(5)` |
| 13 | Ko trả sách 2 lần | `return_book()` check `status == "returned"` |
| 14 | Phạt 5,000 VND/ngày trễ | `fine_per_day = 5000` |
| 15 | Mã phiếu unique | `record_id_counter` tăng đều, đồng bộ sau load |
| 16 | Ko bị trùng khi load lại | `load_data()` clear hết LinkedList + HashMap trước khi load |

---

## Cách Build

### Cần gì

- **g++** (C++11 trở lên) hoặc **MSVC** (Visual Studio 2017+)
- Windows / Linux / macOS

### Build bằng GCC (khuyên dùng)

```bash
cd LibraryManagementSystem
g++ -std=c++11 -I. main.cpp lb_management/services/LibraryManager.cpp -o main.exe
```

### Build test

```bash
# Audit test (50 assertion)
g++ -std=c++11 -I. test_audit.cpp lb_management/services/LibraryManager.cpp -o test_audit.exe

# Business audit (100 assertion)
g++ -std=c++11 -I. business_audit.cpp lb_management/services/LibraryManager.cpp -o business_audit.exe

# Validation hardening test (13 assertion)
g++ -std=c++11 -I. validation_test.cpp lb_management/services/LibraryManager.cpp -o validation_test.exe
```

### Build bằng MSVC

```cmd
cl /EHsc /std:c++11 /I. main.cpp lb_management\services\LibraryManager.cpp /Fe:main.exe
```

---

## Cách Chạy

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

`data/records.txt` cứ để trống (tự sinh lúc chạy).

### 2. Chạy chương trình

```bash
./main.exe
```

Chương trình tự load dữ liệu và hiện Dashboard:

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

## Ví Dụ Sử Dụng

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

### Trả trễ hạn
```
  Ngay tra (DD/MM/YYYY): 20/06/2026
Canh bao: Sach tra tre han 5 ngay! So tien phat la: 25000 VND.
```

---

## Testing

Hệ thống có 4 bộ test riêng:

| File test | Số assertion | Mục đích |
|-----------|-------------|----------|
| `test_audit.cpp` | 50 | Kiểm tra toàn bộ Business Logic: Thêm/Xóa/Sửa Sách, Thêm/Xóa Bạn đọc, Mượn/Trả, Khóa/Mở, Save/Load, Sort, Search |
| `business_audit.cpp` | 100 | Rà soát toàn diện: validation, bất biến, tình huống biên, pre/post conditions, toàn vẹn dữ liệu |
| `validation_test.cpp` | 13 | Kiểm tra hardening: SL âm, định dạng email, định dạng SDT, trả 2 lần, giới hạn mượn |
| `final_verification.cpp` | 50 | Stress test + ownership + ID collision + round-trip |

### Chạy hết test:
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

## Hạn Chế Đã Biết

- **Console UI**: Giao diện dòng lệnh, ko có đồ họa.
- **TXT Storage**: Cất dữ liệu dạng file text, ko dùng database. Ko hợp cho môi trường nhiều user cùng lúc.
- **Single-user**: Chưa có đăng nhập hay phân quyền.
- **Phân trang**: Danh sách in hết 1 lượt, chưa chia trang.
- **Bảng biểu**: Mới in text đơn giản, chưa vẽ table.
- **Dashboard overdue**: Dashboard chưa hiện số sách quá hạn (xem riêng trong menu).
- **Thread safety**: Chưa an toàn cho đa luồng.
- **Date validation**: Chưa check định dạng DD/MM/YYYY hay tính hợp lệ ngày tháng.

---

## Dự Tính Cải Tiến

- Thêm phân trang cho danh sách sách/bạn đọc/phiếu mượn
- Vẽ bảng table cho hiển thị
- Thêm overdue count vô dashboard
- Hỗ trợ import/export CSV
- Chuyển storage qua SQLite để hỗ trợ concurrent
- Thêm unit test tự động (Google Test framework)
- Thêm CI/CD pipeline
- Hỗ trợ Unicode đầy đủ cho tên sách và tác giả tiếng Việt
# Hệ Thống Quản Lý Đăng Nhập và Ví Điểm Thưởng

---

## Giới thiệu dự án

Đây là một hệ thống dòng lệnh được viết bằng **C++17**, sử dụng **SQLite3** để lưu trữ dữ liệu, giúp quản lý tài khoản người dùng, ví điểm và giao dịch điểm. Dự án hỗ trợ bảo mật bằng **OTP**, phân quyền người dùng, và có khả năng **sao lưu / phục hồi** dữ liệu.

---

## Thành viên & phân công

| Họ và Tên      | MSSV     | Công việc chính |
|----------------|----------|-----------------|
| Nguyễn Phú Sang     | K24DTCN084  | xử lý phần admin, tài liệu readme, khởi tạo database |
| Ngô Gia Phú Trọng   | K24DTCN093  | Xử lý phần OTP, auth, backup |
| Bùi Văn Huy         | K24DTCN060  | Xử lý phần User, Wallet  |
| Nguyễn Thái Phong   | K24DTCN077  | Xử lý phần transactions, giao diện CLI  |

---

## Đặc tả chức năng

### Đăng ký & đăng nhập
- Mỗi người có tên đăng nhập (duy nhất), mật khẩu (băm SHA256), họ tên, email.
- Có thể tự nhập mật khẩu hoặc để hệ thống sinh tự động.
- Mật khẩu tự sinh buộc đổi khi đăng nhập lần đầu.

### Phân quyền người dùng
- **Người dùng thường**:
  - Xem/sửa thông tin cá nhân (OTP xác nhận)
  - Xem số dư, chuyển điểm
- **Người quản trị**:
  - Xem danh sách người dùng
  - Tạo mới, chỉnh sửa tài khoản người khác (OTP xác nhận)
  - Sao lưu / phục hồi cơ sở dữ liệu

### Quản lý ví điểm
- Mỗi tài khoản có một ví riêng, mã định danh duy nhất.
- Giao dịch điểm giữa các ví có kiểm tra số dư, đảm bảo tính nguyên tử.
- Lịch sử giao dịch được lưu lại: ai chuyển cho ai, số điểm, thời gian.

### Bảo mật OTP
- Mã OTP gồm 6 chữ số, sinh ngẫu nhiên.
- Hiển thị mô phỏng như gửi đến người dùng.
- OTP được yêu cầu khi thay đổi thông tin hoặc thực hiện giao dịch.

### Sao lưu & phục hồi
- Người quản trị có thể sao lưu file database vào thư mục `backup/`
- Có thể phục hồi lại database từ bản sao lưu.

---

## Yêu cầu hệ thống

- C++17 trở lên
- SQLite3
- OpenSSL (để băm mật khẩu)

### Cài đặt thư viện trên Ubuntu và chạy chương trình

```bash
sudo apt update
sudo apt install build-essential libsqlite3-dev libssl-dev
make
./reward_wallet_system
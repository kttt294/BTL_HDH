=================================
Calculator OS – BTL Hệ Điều Hành
=================================

1. Giới thiệu
Đây là dự án bài tập lớn môn Hệ Điều Hành của sinh viên Trường Công nghệ Thông tin – Đại học Phenikaa.
Mục tiêu của dự án là xây dựng một hệ điều hành dạng mini với giao diện console, hỗ trợ bàn phím và những chức năng tính toán cơ bản.

2. Kiến trúc tổng quát
- kernel.cpp : giữ vai trò hạt nhân chính, chịu trách nhiệm khởi tạo GDT, hệ thống ngắt và điều khiển bàn phím.
- console.* : cung cấp giao diện văn bản, màu sắc và các hàm nhập/xuất ký tự.
- keyboard.* : xử lý ngắt bàn phím và cung cấp dữ liệu cho kernel.
- calculator.* : hiện thực các phép toán cơ bản và giải phương trình.

3. Tính năng nổi bật
- Menu chính gồm 2 mục:
  1. Calculations: hỗ trợ các phép +, -, *, / với parser biểu thức.
  2. Equations: giải phương trình bậc nhất và bậc hai, có kiểm tra đầu vào.
- Hỗ trợ màu sắc console để phân biệt thông điệp, cảnh báo và kết quả.
- Kiến trúc modular, tách biệt giữa giao diện, xử lý bàn phím và logic tính toán.

4. Yêu cầu môi trường
- Trình biên dịch hỗ trợ C++17 (g++, clang++).
- Trình giả lập hoặc môi trường boot như QEMU/Bochs để kiểm thử hệ điều hành.
- Công cụ tạo ảnh đĩa (nếu cần) như nasm, grub-mkrescue.

5. Hướng dẫn build & chạy
Biên dịch toàn bộ mã nguồn bằng chuỗi công cụ cross-compile (ví dụ i686-elf-g++).
Liên kết thành kernel ELF, sau đó đóng gói cùng GRUB để tạo ISO bootable.
Chạy ISO bằng QEMU: qemu-system-i386 -cdrom CalculatorOS.iso.

6. Định hướng phát triển
- Bổ sung thêm chức năng giải hệ phương trình, ma trận.
- Hỗ trợ lưu lịch sử tính toán.
- Cải thiện driver thiết bị (chuột, VGA nâng cao).
- Tích hợp đơn vị đo lường hoặc module chuyển đổi.

7. Liên hệ
- Nhóm sinh viên BTL Hệ Điều Hành – Trường CNTT, Đại học Phenikaa gồm:
    24100093: Kiều Thị Thu Trang
    24100012: Trần Minh Sang
    24102651: Ngô Quang Thiện
    : Trương Việt Thành
    
- Email liên hệ: 24100093@st.phenikaa-uni.edu.vn


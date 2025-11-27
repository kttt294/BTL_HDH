=================================
Calculator OS – BTL Hệ Điều Hành
=================================

1. Giới thiệu
Đây là dự án bài tập lớn môn Hệ Điều Hành của sinh viên Trường Công nghệ Thông tin – Đại học Phenikaa, gồm:
    24100093: Kiều Thị Thu Trang
    24100012: Trần Minh Sang
    24102651: Ngô Quang Thiện
    24106898: Trương Việt Thành
Calculator OS là một mini OS chạy trực tiếp trên phần cứng x86, có giao diện console, hỗ trợ bàn phím PS/2 và cung cấp 2 chế độ tính toán (biểu thức cơ bản và giải phương trình).

2. Kiến trúc tổng quát
- loader.s: bộ nạp Multiboot thiết lập stack, gọi các hàm khởi tạo C++ rồi chuyển quyền điều khiển cho kernelMain.
- gdt.*: khởi tạo Global Descriptor Table tối giản với phân đoạn code/data 4 GB để CPU vào chế độ bảo vệ.
- interrupts.* & interrupts_asm.s: thiết lập IDT, ánh xạ lại PIC, định nghĩa handler timer/keyboard và chuyển ngữ cảnh về C++ để xử lý.
- keyboard.*: driver bàn phím PS/2 đọc scancode, áp dụng keymap thường/Shift và đẩy phím vào buffer vòng.
- console.* & port.*: thao tác trực tiếp với bộ nhớ VGA text mode 0xb8000, quản lý màu sắc, cuộn màn hình, in số thực với độ chính xác tùy chỉnh và đọc chuỗi từ bàn phím.
- calculator.*: hiện thực các phép toán +, -, *, / (có kiểm tra chia 0), giải phương trình bậc nhất/bậc hai, cùng một parser biểu thức tuyến tính.
- memory.cpp: bộ cấp phát operator new/delete tối giản với heap tĩnh 8 KB cho các đối tượng C++.
- kernel.cpp: hạt nhân chính khởi tạo console, GDT, bộ ngắt, driver bàn phím và vòng lặp menu tương tác.

3. Tính năng nổi bật
- Menu chính gồm 2 mục rõ ràng:
    1. Basic Calculations: nhập biểu thức dạng a op b (hỗ trợ số âm, số thực) và nhận kết quả hoặc thông báo lỗi cú pháp/chia 0.
    2. Solve Equations: lựa chọn 1 (phương trình bậc nhất ax + b = 0) hoặc 2 (bậc hai ax^2 + bx + c = 0), có kiểm tra dữ liệu và thông báo trường hợp vô nghiệm/vô số nghiệm.
- Console sử dụng màu để phân tách tiêu đề, cảnh báo, kết quả, giúp trải nghiệm gần với ứng dụng thực tế.
- Input line editor hỗ trợ Backspace, buffer giới hạn và xuống dòng sạch, đảm bảo thao tác trên phần cứng thật hoặc trình giả lập.
- Toàn bộ hệ thống hoạt động mà không cần thư viện chuẩn, phù hợp cho môi trường OS Dev.

4. Yêu cầu môi trường
- Toolchain cross-compile i686-elf (gcc/g++ + binutils) với các cờ 32-bit, không runtime chuẩn (xem Makefile).
- Trình lắp mã as (GAS) hoặc nasm tương thích để biên dịch các file .s.
- GNU Make, ld (i386-elf) và grub-mkrescue để tạo ISO khởi động chuẩn Multiboot.
- Trình giả lập/máy ảo hỗ trợ boot từ ISO như QEMU, Bochs, VirtualBox (target run có sẵn cho VirtualBox).

5. Hướng dẫn build & chạy
- Trong Ubuntu:
    sudo apt update
    sudo apt install build-essential grub-pc-bin xorriso
- Trong thư mục dự án:
    make clean (tùy chọn) để xoá sản phẩm cũ.
    make mykernel.iso biên dịch tất cả .cpp/.s vàvà đóng gói kernel vào cấu trúc GRUB (iso/boot/mykernel.bin) và tạo ảnh ISO bootable.
- Chạy thử với QEMU: qemu-system-i386 -cdrom mykernel.iso.
- Hoặc dùng target make run (cần cấu hình sẵn VM tên "My OS" trong VirtualBox).

6. Định hướng phát triển
- Mở rộng parser biểu thức hỗ trợ ưu tiên toán tử, ngoặc và nhiều toán hạng.
- Bổ sung solver cho hệ phương trình/ma trận, tích hợp thư viện số học ma trận cơ bản.
- Ghi log hoặc lịch sử phép tính vào bộ nhớ để người dùng xem lại.
- Tăng cường driver thiết bị: hỗ trợ chuột PS/2, đồ họa VGA nâng cao, bộ đệm bàn phím lớn hơn.
- Thêm module chuyển đổi đơn vị hoặc các công cụ khoa học (sin/cos, log).

7. Liên hệ
- Email liên hệ: 24100093@st.phenikaa-uni.edu.vn


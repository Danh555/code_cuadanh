#Ghi chú kết nối phần cứng:
/*
máy in Tm-T88III:
*/
PC-->usbtoserial-->box (BP9)
Printer (Dp25)-->dp25todp9 (màu xám) -->dp9todp25-->box (dp25)

/*
máy in Xprinter-XP-80C:
*/

PC-->usbtoserial-->box (BP9)
Printer (Dp9)-->Dp9toDp9-->dp9todp25 (màu đen) box (dp25)

#Ghi chú các phiên bản code

Serial_Printer_Dev_1.zip:
-In, up cho máy Serial Tm-t88ii lên server ok
-Chưa decode ra hình được


Serial_Printer_Dev - Copy (8)
-thêm inmainstatus(); send_satatus();

Serial_Printer_Dev - Copy (9)
-update cubeide

Serial_Printer_Dev - Copy (10)
-đổi cách thức chuyển tiếp qua printer thành nhiều buffer con (request_dang_guitin)
-in testpage:  sau khi in xong, Printer status: idle-->có thể in bill tiếp theo
-in chữ "hello": sau khi in xong, Printer status: printing-->không in được bill tiếp theo

Serial_Printer_Dev - Copy (12)
-thêm delay sau khi in bill xong
-đã fix lỗi Printer status: printing

Serial_Printer_Dev - Copy (13)
-thêm trạng thái đèn led báo có thiết bị gắn vào (chớp nhanh)

Serial_Printer_Dev - Copy (14)
-thay đổi cách chuẩn bị buffer nhận từ PC

Serial_Printer_Dev - Copy (15)
-đổi giá trị thiết lập buffer nhận data từ PC

Serial_Printer_Dev - Copy (16)
-thêm hàm sosanhlenhfct

Serial_Printer_Dev - Copy (17)
-sửa lại format printf

Serial_Printer_Dev - Copy (18)
-sắp xếp lại biến

Serial_Printer_Dev - Copy (19)
-chuyển tốc độ lên 115200
-In Qua box bị khựng
-Xprinter-XP-80C khi in trực tiếp (PC-->USB2Serial-->Printer): in vẫn bị khựng
the same

Serial_Printer_Dev - Copy (20)
-dùng chatGPT đổi cách so sánh lệnh máy in

Serial_Printer_Dev - Copy (21)
-thêm default printer 64 64
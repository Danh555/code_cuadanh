###------------------USB_Projek--------------------------------

# USB_Dev_1M - Copy:
	Generate tự động
	chưa code
	
#USB_Dev_1M - Copy (2)
	copy code  từ USB_Dev - Copy (28) vào
	-test in TM-T82II ok
	
#USB_Dev_1M - Copy (3)
	tăng bộ nhớ lên 1M
	
#USB_Dev_1M - Copy (5)
	copy của file hôm nay
	
#USB_Dev_1M - Copy (6)
	sửa gói truyền data in qua ESP 
	
#USB_Dev_1M - Copy (7)
	đang test thí dụ ghi đọc file
	
#USB_Dev_1M - Copy (8)
	thêm hàm ghi theo byte
	
#USB_Dev_1M - Copy (9)
	in bill dài
	up bill dài
	đang lỗi: khi in hơi sượng do chuyển cách lưu thành byte, chuyển thành word thì đang bị lưu số lẻ 4 sẽ bị lỗi.
	
#USB_Dev_1M - Copy (10)
	chuyển lưu theo word
	
#USB_Dev_1M - Copy (11)
#USB_Dev_1M - Copy (12)
	xử lý truyền qua esp
#USB_Dev_1M - Copy (16)
	xử lý in file cùng lúc
#USB_Dev_1M - Copy (18)
	xử lý truyền ngắt quá esp
#USB_Dev_1M - Copy (19)
	xử lý in liên tiếp 2 file
	xử lý truyền qua esp song song
#USB_Dev_1M - Copy (20)
	xử lý uart theo gói
#USB_Dev_1M - Copy (21)
	thêm xóa flash sau khi truyền
#USB_Dev_1M - Copy (22)
	thêm lệnh get model, protduct
#USB_Dev_1M - Copy (23)
	thêm xử lý phản hồi theo gói
#USB_Dev_1M - Copy (24)
	thêm xử lý phản hồi theo gói
#USB_Dev_1M - Copy (25)
	sửa tên phiên bản 1.2.0
#USB_Dev_1M - Copy (26)
	thay đổi cách so sánh bắt đầu/ kết thúc file in
	
	đang giới hạn so sánh gói là 128*1024
	in file dài: đang bị lỗi về bộ nhớ
	
#USB_Dev_1M - Copy (27)
	thêm lệnh format disk khi khởi động app (7s để xóa hết vùng nhớ)
	test máy TM-T88IV: 
		in được file 15 bardcode, và up lên server ok
		in cùng lúc 2 bill, và up lên server thành công
	test máy TM-T88II:
		in testpage ok, up lên server giải mã ok
		in file khác từ word : thì đang báo thiếu phần trăm gói
		
#USB_Dev_1M - Copy (28)
	thêm so sánh lệnh cho máy in TM-T82X
#USB_Dev_1M - Copy (29)
	thêm so sánh lệnh cho máy in TM-T82X
	test in, up file lên server ok
	decode file chưa đúng
	
#USB_Dev_1M - Copy (30)
	thêm xóa flash cho chip 256k
	
#USB_Dev_1M - Copy (31)
	xử lý so sánh bộ nhớ trống khi lưu file
#USB_Dev_1M - Copy (32)

#USB_Dev_1M - Copy (33)
	fix lỗi nhận gói cuối cho máy in TM-T82X
	
#testing again copy 27: để xác định lại gói in cho máy in TM-T88II	: 
nay test lại copy 33 giải mã ok cho máy in TM-T82II
????
lỗi ở đâu

#USB_Dev_1M - Copy (33)
	in bil dài đang lỗi (barcode Muoi hai bị sai)
	
#USB_Dev_1M - Copy (34)
	chỉnh lại định dạng version
	
#USB_Dev_1M - Copy (35)
	xử lý in bill dài bị sai data
	
#USB_Dev_1M - Copy (36)
xử lý in bill dài bị sai data

#USB_Dev_1M - Copy (37)
	xử lý in bill dài bị sai data
	
#USB_Dev_1M - Copy (38)
	fix lỗi gói lưu gói data in cuối cùng: dòng 645 package_index
	
#USB_Dev_1M - Copy (39)
#USB_Dev_1M - Copy (40)
	chỉnh lại mở endpoint
#USB_Dev_1M - Copy (41)
	test lại máy in TM-T88IV : ok
	usb profilix không hoạt động
	
	nếu không có dòng sau:
	if(my_var_user.ui8_chophep_docdata_tu_realdevice ==0U) return;
	 thì usb profilix hoạt động được
	 
#USB_Dev_1M - Copy (42)
	 fix để chạy được với usb profilix
#USB_Dev_1M - Copy (43)
	= #USB_Dev_1M - Copy (45)
	tạo thêm file này, để khi copy nó chạy theo thứ tự đúng

#USB_Dev_1M - Copy (43)_from Danh Sing
	code testing tại Singapore: chỉnh trên máy Danh
	
#USB_Dev_1M - Copy (44)
	xử lý lại gói bắt đầu, kết thúc data in
	
#USB_Dev_1M - Copy (45)
	xử lý lưu chiều dài gói data dài
	
	chỉ đúng cho TM_UNKNOW

#USB_Dev_1M - Copy (46)
	xử lý lưu chiều dài gói data dài: thêm cập nhật buffer chieudaitungfile[] mỗi khi có data mới
	
	
#USB_Dev_1M - Copy (47)
	thêm sniffer log
	
	
#USB_Dev_1M - Copy (48)
	fix lỗi lưu chiều dài buffer sniffer
	
#USB_Dev_1M - Copy (49)
	thêm phần cứng HW3
	
#USB_Dev_1M - Copy (50)
	thêm reset chip nếu có thiết bị gắn vào, mà không khởi tạo được
	
#USB_Dev_1M - Copy (51)
	đổi biến sử dụng theo copy 50
	ghi chú: máy in TM-T82X: phải mở dòng 1761: USBD_LL_StallEP(&hUsbDeviceHS, t); thì in bill dài mới được
	
#USB_Dev_1M - Copy (53)
	mở dòng 1761: USBD_LL_StallEP(&hUsbDeviceHS, t); thì máy in TM-T82X mới chạy được
	
#USB_Dev_1M - Copy (54)
	máy in Tm-82X: để loại máy in #define PRINTER_MODEL		TM_UNKNOW mới upload data lên server được
	đang lắp đợt 1 bên Sing
	
	
#USB_Dev_1M - Copy (55):
	giống 56: thêm để cho copy theo thứ tự
	
#USB_Dev_1M - Copy (56)	
	thêm chức năng tự dừng ghi log khi size >1K
#include <string.h> // Cần thiết cho hàm memcpy

// Định nghĩa các macro cho các lệnh bắt đầu in
#define START_PRINT_COMMAND_1  "\x1b\x40"	/* máy in Xprinter */
#define START_PRINT_COMMAND_2  "\x1b\x3d\x01"	/* máy in Epson TM88iii*/
#define START_PRINT_COMMAND_3  "\x1b\x42\x43\x44"
// ... tiếp tục định nghĩa các lệnh khác với độ dài khác nhau
#define START_PRINT_COMMAND_20 "\x11\x12\x13"

// Định nghĩa các macro cho các lệnh kết thúc in
#define END_PRINT_SIGNATURE_1  "\x1d\x54"
#define END_PRINT_SIGNATURE_2  "\x1d\x53\x54"
#define END_PRINT_SIGNATURE_3  "\x1d\x54\x55\x56"
// ... tiếp tục định nghĩa các lệnh khác với độ dài khác nhau
#define END_PRINT_SIGNATURE_20 "\x11\x12\x13"

// Mảng các con trỏ tới macro lệnh bắt đầu và độ dài tương ứng
const char* START_PRINT_COMMANDS[] = {
    START_PRINT_COMMAND_1, START_PRINT_COMMAND_2, START_PRINT_COMMAND_3,
    // ... tiếp tục liệt kê các lệnh khác
    START_PRINT_COMMAND_20
};

const int START_COMMAND_LENGTHS[] = {
    2, 3, 4, // ... tiếp tục độ dài của các lệnh
    3
};

// Mảng các con trỏ tới macro lệnh kết thúc và độ dài tương ứng
const char* END_PRINT_SIGNATURES[] = {
    END_PRINT_SIGNATURE_1, END_PRINT_SIGNATURE_2, END_PRINT_SIGNATURE_3,
    // ... tiếp tục liệt kê các lệnh khác
    END_PRINT_SIGNATURE_20
};

const int END_SIGNATURE_LENGTHS[] = {
    2, 3, 4, // ... tiếp tục độ dài của các lệnh
    3
};

// Mảng các chỉ số kết thúc tương ứng cho mỗi cặp lệnh
const int END_SIGNATURE_INDICES[] = {
    2, 3, 2, // ... tiếp tục chỉ số của các lệnh
    1
};


#define START_PRINT_COMMANDS_DEFAULT	"\x11\x12\x13"
#define END_PRINT_SIGNATURES_DEFAULT  	"\x11\x12\x13"
#define START_COMMAND_LENGTHS_DEFAULT  	3
#define END_SIGNATURE_LENGTHS_DEFAULT  	3
#define END_SIGNATURE_INDICES_DEFAULT  	0

// Số lượng lệnh bắt đầu và kết thúc
#define NUM_START_COMMANDS (sizeof(START_PRINT_COMMANDS) / sizeof(START_PRINT_COMMANDS[0]))
#define NUM_END_SIGNATURES (sizeof(END_PRINT_SIGNATURES) / sizeof(END_PRINT_SIGNATURES[0]))

// Biến toàn cục để lưu trữ cặp lệnh được chọn
const char *selected_start_cmd = NULL;
int selected_start_len = 0;
const char *selected_end_sig = NULL;
int selected_end_len = 0;
int selected_end_index = 0;

// Hàm chọn cặp lệnh bắt đầu và kết thúc
void select_command_pair(int index)
{
    if (index < 0 || index >= NUM_START_COMMANDS || index >= NUM_END_SIGNATURES) {
        // Xử lý khi index không hợp lệ, có thể log lỗi hoặc thiết lập giá trị mặc định
        selected_start_cmd = START_PRINT_COMMANDS_DEFAULT;
        selected_end_sig = END_PRINT_SIGNATURES_DEFAULT;
        selected_start_len = START_COMMAND_LENGTHS_DEFAULT;
        selected_end_len = END_SIGNATURE_LENGTHS_DEFAULT;
        selected_end_index = END_SIGNATURE_INDICES_DEFAULT;
        return;
    }
    // Lấy lệnh bắt đầu và kết thúc cùng độ dài từ các mảng macro
    selected_start_cmd = START_PRINT_COMMANDS[index];
    selected_end_sig = END_PRINT_SIGNATURES[index];
    selected_start_len = START_COMMAND_LENGTHS[index];
    selected_end_len = END_SIGNATURE_LENGTHS[index];
    selected_end_index = END_SIGNATURE_INDICES[index];  // Cập nhật chỉ số kết thúc từ mảng
}

// Hàm kiểm tra điều kiện bắt đầu
static int check_start_condition(const uint8_t *data, int length)
{
    if (length >= selected_start_len && memcmp(data, selected_start_cmd, selected_start_len) == 0) {
        return 1; // Điều kiện bắt đầu in thỏa mãn
    }
    return 0;
}

// Hàm kiểm tra điều kiện kết thúc
static int check_end_condition(const uint8_t *data, int length)
{
    if (length >= selected_end_len && memcmp(&data[length - selected_end_index], selected_end_sig, selected_end_len) == 0) {
        return 1; // Điều kiện kết thúc in thỏa mãn
    }
#if (0)
    else if (length >= 0 && length < 64 && memcmp(END_PRINT_SIGNATURES_DEFAULT, selected_end_sig, selected_end_len) == 0) {
        return 1; // Điều kiện kết thúc in thỏa mãn
    }
#endif
    return 0;
}


// Gọi hàm select_command_pair tại một điểm khởi tạo trong chương trình
void initialize_command_pair(int command_pair_index)
{
    select_command_pair(command_pair_index);
}

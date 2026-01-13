/*
  FontMaker.h - Thư viện Font tương thích utf-8
  Created by Dao Nguyen - IOT47.com
  Liên hệ: daonguyen20798@gmail.com
  Người dùng toàn quyền sử dụng cho mục đích cá nhân
*/
#ifndef __FontMaker_h___
#define __FontMaker_h___

#include "IOT47_UTF8.h"
#include "MyFontMaker.h"

// typedef void (*set_px_typedef)(int16_t x,int16_t y,uint16_t color);  
typedef void (*set_px_typedef)(int16_t x,int16_t y,uint16_t color,uint8_t *state);
typedef void (*set_px2_typedef)(int16_t x,int16_t y,uint8_t cR,uint8_t cG,uint8_t cB); 
class MakeFont
{
    public:   	  
   	unsigned char read(int16_t x, int16_t y,  uint16_t txt);
   	unsigned char putChar(int16_t x,int16_t y,uint16_t txt,uint16_t color,uint16_t backcolor);
    unsigned char putChar_noBackColor(int16_t x,int16_t y,uint16_t txt,uint16_t color);
    unsigned char getCharLength(uint16_t txt);
   	void print(int16_t x,int16_t y,unsigned char *s,uint16_t color,uint16_t backcolor);
   	void print(int16_t x,int16_t y,char *s,uint16_t color,uint16_t backcolor);
    void print(int16_t x,int16_t y,String str,uint16_t color,uint16_t backcolor);
    uint16_t getLength(unsigned char *s);
    uint16_t getLength(char *s);
    uint16_t getLength(String str);
    void print_noBackColor(int16_t x,int16_t y,unsigned char *s,uint16_t color);
    void print_noBackColor(int16_t x,int16_t y,char *s,uint16_t color);
    void print_noBackColor(int16_t x,int16_t y,String str,uint16_t color);
    void set_font(const MyFont_typedef myfont);

    unsigned char putChar(int16_t x,int16_t y,uint16_t txt,uint8_t cR,uint8_t cG,uint8_t cB,uint8_t cBR,uint8_t cBG,uint8_t cBB);
    void print(int16_t x,int16_t y,unsigned char *s,uint8_t cR,uint8_t cG,uint8_t cB,uint8_t cBR,uint8_t cBG,uint8_t cBB);
    void print(int16_t x,int16_t y,char *s,uint16_t color,uint8_t cR,uint8_t cG,uint8_t cB,uint8_t cBR,uint8_t cBG,uint8_t cBB);
    void print(int16_t x,int16_t y,String str,uint8_t cR,uint8_t cG,uint8_t cB,uint8_t cBR,uint8_t cBG,uint8_t cBB);
    unsigned char putChar_noBackColor(int16_t x,int16_t y,uint16_t txt,uint8_t cR,uint8_t cG,uint8_t cB);
    void print_noBackColor(int16_t x,int16_t y,unsigned char *s,uint8_t cR,uint8_t cG,uint8_t cB);
    void print_noBackColor(int16_t x,int16_t y,char *s,uint8_t cR,uint8_t cG,uint8_t cB);
    void print_noBackColor(int16_t x,int16_t y,String str,uint8_t cR,uint8_t cG,uint8_t cB);
	
	void print_text(int16_t x,int16_t y,String str,int len1,int len2,int len3,int len4,int len5,int len6,uint16_t color,uint16_t color1,uint16_t color2,uint16_t color3,uint16_t color4,uint16_t color5);    
	
	unsigned char putChar_index_noBackColor(int16_t x,int16_t y,int16_t shift,uint16_t txt,uint16_t color);
	void print_text1(int16_t x,int16_t y,String str,uint16_t *len,uint16_t *color);
	
	/**
	print_shift_text()
	MrCong them vao
	index: chứa vị trí hiển thị trong chuỗi text string
	shift: chứa cột hiển thị trước đó.
	y: vị trí y trong bảng led
	len, color: mã màu
	*/	
	void print_shift_text(uint16_t *index,unsigned char *shift,int16_t y,String str,uint16_t *len,uint16_t *color);

   	MakeFont(set_px_typedef set_px);
    MakeFont(set_px2_typedef set_px);
   	private:
    set_px_typedef FontMaker_setpx;
    set_px2_typedef FontMaker_setpx2;
    const uint16_t *_f_map;
    const uint8_t  *_f_name;
	uint8_t cong123;
};

#endif //__FontMaker_h___

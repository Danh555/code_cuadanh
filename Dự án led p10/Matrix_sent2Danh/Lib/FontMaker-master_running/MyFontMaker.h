/*
  MyFontMaker.h - Thư viện Font tương thích utf-8 (v2)
  Created by Dao Nguyen - IOT47.com
  Liên hệ: daonguyen20798@gmail.com
  Không sửa file này
*/
#ifndef __MY_FontMaker_h___
#define __MY_FontMaker_h___

#include "Arduino.h"

typedef struct 
{
  const uint8_t  *f_name;
  const uint16_t *f_map;
}MyFont_typedef;

//Khai báo extern các loại font mà bạn sử dụng

extern const MyFont_typedef MakeFont_Font1;
extern const MyFont_typedef MYFONT1616;
extern const MyFont_typedef timenewsroman16;
//auto add here




#endif //__MY_FontMaker_h___
//---------------------------------------------------------------------Kết thúc file ----------------------------------------------//

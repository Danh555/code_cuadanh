/**
   @file 	mymemory.h
   @version     1.0.0
   @date        2023.12.24
   @author 	Nguyen Thanh Cong
   @contact     thanhcong402@gmail.com
   @contribution

   @description


   @license

*/


#ifndef _mymemory__
#define _mymemory__

#define DF_PAGE_ 256

#define DF_ADDR_INFO 2*DF_PAGE_
#define DF_ADDR_DATA 13*1024
#define DF_ADDR_INFO_MAX (DF_ADDR_DATA - DF_ADDR_INFO -1)

#ifndef max_file_save
#define max_file_save 130		//hỗ trợ lưu tối đa 130 file
#endif 

/*-----------------------thông in lưu bill-------------*/

struct HEADER_FILE {
   char SOF[4];   /*đánh dấu bắt đầu file*/
   char brand_name[23];
   char model[23];
   char name[24];//BIL_20231224102923 /*tên data*/
   uint32_t addr; /*địa chỉ data*/
   uint32_t length; /*chiều dài gói data*/
} ;

struct INFO_FILE {
   HEADER_FILE    billinfo;
   uint8_t status1;
   uint8_t status2;
   uint8_t status3;   
};
/*-----------------------end thông in lưu bill-------------*/

HEADER_FILE header_file_doing;

uint32_t get_addr_header(int index_);

int erase_flash(uint32_t addr, uint32_t lem)
{   
      Serial.print("Erase @");
      Serial.println(addr);
    if (flash.eraseSection(addr, lem)) 
    {      
      Serial.println("Ok");
      return 0;
    }
    else 
    {
      Serial.println("Fail");
      return 1;
    }
    
}
int write_header(uint32_t addr, HEADER_FILE reg_)
{
   Serial.print("write header @addr \t =");
   Serial.println(addr);
   Serial.print("name \t =");
   Serial.println(reg_.name);
   Serial.print("addr \t =");
   Serial.println(reg_.addr);
   Serial.print("leng \t =");
   Serial.println(reg_.length);   
   
   if (flash.writeAnything(addr, reg_))
   {
      Serial.println("OK");
      return 0;
   }
   else
   {
      Serial.println("Fail");
      return 1;
   }
}
int write_header_index(int index_, HEADER_FILE reg_)
{
   
   uint32_t addr = get_addr_header(index_);

   // Serial.print("write header file \t = ");
   // Serial.println(index_);

   Serial.print("addr header \t @");
   Serial.println(addr);

   Serial.print("name \t =");
   Serial.println(reg_.name);
   
   Serial.print("addr file \t @");
   Serial.println(reg_.addr);
   
   Serial.print("leng \t =");
   Serial.println(reg_.length);
   
   if (flash.writeAnything(addr, reg_))
   {
      Serial.println("OK");
      return 0;
   }
   else
   {
      Serial.println("Fail");
      return 1;
   }
}

int read_header(uint32_t addr, HEADER_FILE *reg_)
{
   // Serial.print("read header @addr \t =");
   // Serial.println(addr);

   if (addr > DF_ADDR_DATA) return 1;
   HEADER_FILE tam;
   if (flash.readAnything(addr, tam))
   {      
      strncpy(reg_->SOF, tam.SOF, sizeof(tam.SOF));
      strncpy(reg_->brand_name, tam.brand_name, sizeof(tam.brand_name));
      strncpy(reg_->model, tam.model, sizeof(tam.model));
      strncpy(reg_->name, tam.name, sizeof(tam.name));
      reg_->addr = tam.addr;
      reg_->length = tam.length;   
      // Serial.println("OK");
      return 0;
   }
   else
   {
      Serial.println("Fail");
      return 1;
   }
}
uint32_t get_addr_header(int index_)
{
   uint32_t addr = DF_ADDR_INFO;
   for (int i =0; i<max_file_save;i++)
   {
      if(index_ == i)
      {
         // Serial.print("stop @");
         // Serial.println(i);
         break;
      }
      
      addr += sizeof(INFO_FILE);

      if(addr > DF_ADDR_INFO_MAX)
      {
        Serial.print("range limit @");
        Serial.println(i); 
        addr =0;       
        break;
      }
  }
  return    addr;
}

/*
read_header_index:
index_   : số thứ tự file
*/
int read_header_index(int index_, HEADER_FILE *reg_)
{
   // Serial.print("read header @addr \t =");
   // Serial.println(addr);
   uint32_t addr = get_addr_header(index_);
   
   Serial.print("addr file = ");
   Serial.println(addr);
      
   if (addr > DF_ADDR_INFO_MAX) return 1;
   HEADER_FILE tam;
   if (flash.readAnything(addr, tam))
   {      
      strncpy(reg_->SOF, tam.SOF, sizeof(tam.SOF));        
      /*kiểm tra xem có đúng file không*/
      if (strncmp(reg_->SOF,"SOF",3) == 0)
      {
         Serial.println("is file");
         strncpy(reg_->brand_name, tam.brand_name, sizeof(tam.brand_name));
         strncpy(reg_->model, tam.model, sizeof(tam.model));
         strncpy(reg_->name, tam.name, sizeof(tam.name));
         reg_->addr = tam.addr;
         reg_->length = tam.length; 
         return 0;
      }
      else
      {
         
         reg_->SOF[0]='\0';
         reg_->brand_name[0]='\0';
         reg_->model[0]='\0';
         reg_->name[0]='\0';
         reg_->addr = 0;
         reg_->length = 0;         
         
         return 1;
      }              
      // Serial.println("OK");
      
   }
   else
   {
      Serial.println("Fail");
      return 1;
   }

   return 1;
}


uint8_t read_status_bill(uint32_t addr, uint8_t *reg_)
{
      
}
int write_data(uint32_t addr, uint8_t *reg_, uint32_t size_)
{   
   int kq = 0;

   //kiểm tra bộ nhớ
   uint32_t maxAddr = flash.getCapacity();
   if (addr + size_ > maxAddr)
   {
      Serial.println(" :Out of memory");
      return 2;
   }

   /* chia nhỏ vùng nhớ ra từng 256 byte để lưu vào*/
    uint16_t length = size_;    
    uint16_t writeBufSz;
    uint16_t data_offset = 0;
    uint16_t maxBytes = 256;
    uint32_t addr_new = addr;
    bool ghi;
   
   do {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;
            
      ghi = flash.writeByteArray(addr_new, (reg_ + data_offset), writeBufSz);
            
      if (ghi == 0)
      {      
         kq = 1;
      }

      addr_new += writeBufSz;
      data_offset += writeBufSz;
      length -= writeBufSz;
      maxBytes = 256;   // Now we can do up to 256 bytes per loop

   } while (length > 0);

   return kq;
}
#if (0) 
int write_data(uint32_t addr, uint8_t *reg_, uint32_t size_)
{   
   Serial.print("write data @");
   Serial.print(addr);
   Serial.print(", size = ");
   Serial.print(addr);

   //kiểm tra bộ nhớ
   uint32_t maxAddr = flash.getCapacity();
   if (addr + size_ > maxAddr)
   {
      Serial.println(" :Out of memory");
      return 2;
   }

   if (flash.writeByteArray(addr, &(*reg_), size_))
   {
      Serial.println(" :OK");
      return 0;
   }
   else
   {
      Serial.println(" :Fail");
      return 1;
   }
}
#endif 

int read_data(uint32_t addr, uint8_t *reg_, uint32_t size_)
{
   int kq = 0;
    uint16_t length = size_;    
    uint16_t writeBufSz;
    uint16_t data_offset = 0;
    uint16_t maxBytes = 256;
    uint32_t addr_new = addr;
    bool ghi;
   
   do {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;
                  
      ghi = flash.readByteArray(addr_new, (reg_+ data_offset), writeBufSz);
            
      if (ghi == 0)
      {      
         kq = 1;
      }

      addr_new += writeBufSz;
      data_offset += writeBufSz;
      length -= writeBufSz;
      maxBytes = 256;   // Now we can do up to 256 bytes per loop

   } while (length > 0);
   return kq;
}

#if (0) 
int read_data(uint32_t addr, uint8_t *reg_, uint32_t size_)
{
   // Serial.print("read data @");
   // Serial.print(addr);
   // Serial.print(" \t");
   if (flash.readByteArray(addr, &(*reg_), size_))
   {
      // Serial.println("OK");
      return 0;
   }
   else
   {
      Serial.println("Fail");
      return 1;
   }
}
#endif 

void inval(HEADER_FILE *reg_)
{
   Serial.println("--------------------infomation of BIL---------------");
   Serial.print("Sign. \t =");
   Serial.println(reg_->SOF);
   Serial.print("Mnf. \t =");
   Serial.println(reg_->brand_name);
   Serial.print("model \t =");
   Serial.println(reg_->model);
   Serial.print("name \t =");
   Serial.println(reg_->name);
   Serial.print("addr \t =");
   Serial.println(reg_->addr);
   Serial.print("leng \t =");
   Serial.println(reg_->length);
   Serial.println("--------------------end infomation of BIL---------------");
}
int find_bill(char *findbill)
{
   Serial.println("----------------find_bill start-----------------------");
   char nem[24];
   sprintf(nem,"%s",findbill);
   
   
   uint32_t u32_addr_header=0;
   HEADER_FILE head_file_tempread;
   int erro_;
   Serial.print("find bill: ");
   Serial.println(nem);
   Serial.print("size : ");
   Serial.println(sizeof(nem));

   u32_addr_header= DF_ADDR_INFO;

   Serial.println("----------------start find-----------------------");
   
   for(int i = 0; i < max_file_save; i++)
   {
      // Serial.print("\n find @: ");
      // Serial.println(i);
      
      erro_ = read_header(u32_addr_header, &head_file_tempread);

      if(erro_)
      {
         Serial.print("---erro @: ");
         Serial.println(i);
         i =200;
         return -1;
      }
      else
      {         
         if (strncmp(head_file_tempread.SOF,"SOF",3) == 0)
         {
            Serial.println("find file");
            inval(&head_file_tempread);
            /*so sánh tên bill*/
            if (strncmp(head_file_tempread.name,nem,sizeof(nem)) == 0)
            {
               Serial.print("Bill find @");
               Serial.println(i);
               return i;
            }
         }         
         // else
         // {
            // Serial.println("no file > next");
            // return 2;
         // }
      }
      u32_addr_header += sizeof(INFO_FILE);

      if (u32_addr_header > DF_ADDR_INFO_MAX )
      {
         i=200;
         Serial.println("range limit");
         return -1;
      }
   }
   return -1;
}

void clear_header(HEADER_FILE *reg_)
{   
   memset(reg_->name,'\0', sizeof(reg_->name));
   reg_->addr = 0;   
   reg_->length =0;   
}
//Prints hex/dec formatted data from page reads - for debugging
void _printPageBytes(uint8_t *data_buffer, uint8_t outputType, uint32_t lem) {
  char buffer[10];
  int b =0;
  for (int a = 0; a < lem; a++) {

      if (outputType == 1) {
        sprintf(buffer, "%02x", data_buffer[a]);
        Serial.print(buffer);
      }
      else if (outputType == 2) {
        uint8_t x = data_buffer[a];
        if (x < 10) Serial.print("0");
        if (x < 100) Serial.print("0");
        Serial.print(x);
        Serial.print(',');
      }      
      b++;
      if(b == 256/4)
      {
         Serial.println();
         b =0;
      }
  }
  Serial.println();
}

#endif
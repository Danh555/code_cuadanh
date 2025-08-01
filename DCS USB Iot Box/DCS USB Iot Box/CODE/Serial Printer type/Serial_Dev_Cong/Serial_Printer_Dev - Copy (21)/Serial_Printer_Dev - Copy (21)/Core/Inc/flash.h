/*
 * flash.h
 *
 *  Created on: Jul 5, 2024
 *      Author: homin
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32f2xx_hal.h"

/* Base address of the Flash sectors */

#define ADDR_FLASH_SECTOR_0    ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1    ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2    ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3    ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4    ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5    ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6    ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7    ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8    ((uint32_t)0x08080000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08100000) /* Base @ of Sector 12, 128 Kbytes */



void Flash_Erase_sector(uint32_t StartSectorAddress);
void Flash_Erase_sector_leng(uint32_t StartSectorAddress, uint16_t numberofwords);
uint32_t Flash_Write_Data(uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords);
uint32_t Flash_Write_byte(uint32_t StartSectorAddress, uint8_t *Data, uint16_t numberofwords);
void Flash_Read_Data(uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords);
void flash_unlock(void);
void flash_lock(void);
void convertdata(uint32_t data, uint8_t *pData);

#endif /* INC_FLASH_H_ */

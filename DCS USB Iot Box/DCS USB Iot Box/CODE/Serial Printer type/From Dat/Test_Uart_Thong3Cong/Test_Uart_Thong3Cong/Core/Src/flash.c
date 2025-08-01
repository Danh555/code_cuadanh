/*
 * flash.c
 *
 *  Created on: Jul 5, 2024
 *      Author: homin
 */

#include "flash.h"
void flash_unlock()
{
	HAL_FLASH_Unlock();
}
void flash_lock()
{
	HAL_FLASH_Lock();
}

/**
 * @brief Gets the sector of a given address
 * @param None
 * @retval The sector of a given address
 */
uint32_t GetSector(uint32_t Address)
{
 uint32_t sector = 0;

 if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
 {
   sector = FLASH_SECTOR_0;
 }
 else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
 {
   sector = FLASH_SECTOR_1;
 }
 else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
 {
   sector = FLASH_SECTOR_2;
 }
 else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
 {
   sector = FLASH_SECTOR_3;
 }
 else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
 {
   sector = FLASH_SECTOR_4;
 }
 else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
 {
   sector = FLASH_SECTOR_5;
 }
 else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
 {
   sector = FLASH_SECTOR_6;
 }
 else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
 {
   sector = FLASH_SECTOR_7;
 }
 else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
 {
   sector = FLASH_SECTOR_8;
 }
 else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
 {
   sector = FLASH_SECTOR_9;
 }
 else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
 {
   sector = FLASH_SECTOR_10;
 }
 else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
 {
   sector = FLASH_SECTOR_11;
 }

 return sector;
}

/**
 * flash_erease_sector
 * xóa hết 1 sector
 */
void Flash_Erase_sector(uint32_t StartSectorAddress)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;

	uint32_t SECTORError;
	 /* Unlock the Flash to enable the flash control register access *************/
	  HAL_FLASH_Unlock();
	  /* Erase the user Flash area*/
	   /* (xác định bởi FLASH_USER_START_ADDR và FLASH_USER_END_ADDR)*/

	  /* Get the number of sector to erase from 1st sector */

      uint32_t StartSector = GetSector(StartSectorAddress);
//	  uint32_t EndSectorAddress = StartSectorAddress + numberofwords*4;
//	  uint32_t EndSector = GetSector(EndSectorAddress);


	  /* Fill EraseInit structure*/
	  EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	  EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	  EraseInitStruct.Sector        = StartSector;
//	  EraseInitStruct.NbSectors     = (EndSector - StartSector) + 1;
	  EraseInitStruct.NbSectors     = 1;

	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register. */
	  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	  {
//		  USBD_UsrLog("HAL_FLASH_GetError");
		  return HAL_FLASH_GetError ();
	  }

	  HAL_FLASH_Lock();
}
/**
 * flash_erease_sector
 * xóa hết sector  chứa chiều dài
 */
void Flash_Erase_sector_leng(uint32_t StartSectorAddress, uint16_t numberofwords)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;

	uint32_t SECTORError;
	 /* Unlock the Flash to enable the flash control register access *************/
	  HAL_FLASH_Unlock();
	  /* Erase the user Flash area*/
	   /* (xác định bởi FLASH_USER_START_ADDR và FLASH_USER_END_ADDR)*/

	  /* Get the number of sector to erase from 1st sector */

      uint32_t StartSector = GetSector(StartSectorAddress);
	  uint32_t EndSectorAddress = StartSectorAddress + numberofwords*4;
	  uint32_t EndSector = GetSector(EndSectorAddress);


	  /* Fill EraseInit structure*/
	  EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	  EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	  EraseInitStruct.Sector        = StartSector;
	  EraseInitStruct.NbSectors     = (EndSector - StartSector) + 1;
//	  EraseInitStruct.NbSectors     = 1;

	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register. */
	  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	  {
//		  USBD_UsrLog("HAL_FLASH_GetError");
		  return HAL_FLASH_GetError ();
	  }

	  HAL_FLASH_Lock();
}
//uint8_t a[] = {1,2,3,4,5,6,7,8


//void flash_write_arr(uint32_t addr, uint8_t *data, uint16_t len)
//{
//	for(uint16_t i = 0 ; i<len; i+=2)
//	{
//		//HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD  , addr + i , data[i]|((uint16_t)data[i+1]<<8));
//	}
//}

uint32_t Flash_Write_Data(uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
{
	int sofar=0;


	 /* Unlock the Flash to enable the flash control register access *************/
	  HAL_FLASH_Unlock();

	  /* Program the user Flash area word by word
	    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
//	  __HAL_FLASH_CLEAR_FLAG(HAL_FLASH_ERROR_PGP);
	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 uint32_t e =  HAL_FLASH_GetError ();
	    	 return e;
	     }
	   }

	  /* Lock the Flash to disable the flash control register access (recommended
	     to protect the FLASH memory against possible unwanted operation) *********/
	  HAL_FLASH_Lock();

	   return 0;
}

uint32_t Flash_Write_byte(uint32_t StartSectorAddress, uint8_t *Data, uint16_t numberofwords)
{
	int sofar=0;


	 /* Unlock the Flash to enable the flash control register access *************/
	  HAL_FLASH_Unlock();

	  /* Program the user Flash area word by word
	    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
//	  __HAL_FLASH_CLEAR_FLAG(HAL_FLASH_ERROR_PGP);
	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, StartSectorAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartSectorAddress += 1;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 uint32_t e =  HAL_FLASH_GetError ();
	    	 return e;
	     }
	   }

	  /* Lock the Flash to disable the flash control register access (recommended
	     to protect the FLASH memory against possible unwanted operation) *********/
	  HAL_FLASH_Lock();

	   return 0;
}


void Flash_Read_Data(uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartSectorAddress;
		StartSectorAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}

void convertdata(uint32_t data, uint8_t *pData)
{
	//xx yy zz tt
	uint32_t datatam = data;

	pData[0] =(uint8_t) (datatam  & 0x000000ff);
	datatam = datatam>>8;
	pData[1] =(uint8_t) (datatam  & 0x000000ff);
	datatam = datatam>>8;
	pData[2] =(uint8_t) (datatam  & 0x000000ff);
	datatam = datatam>>8;
	pData[3] =(uint8_t) (datatam  & 0x000000ff);

}

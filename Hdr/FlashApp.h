/*
 * FlashApp.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */

#ifndef FLASHAPP_H_
#define FLASHAPP_H_

//Defines//
#define FLS_RD_STTREG1_INST		0x05
#define FLS_RD_STTREG2_INST		0x35
#define FLS_RD_STTREG3_INST		0x33
#define FLS_WRT_ENABLE_INST		0x06
#define FLS_WRT_DISABLE_INST		0x04
#define FLS_ERASE_SECTOR_INST		0x20
#define FLS_WRT_STT_REG_INST		0x01
#define FLS_RD_MNDV_ID_INST		0x90
#define FLS_PRG_PAGE_INST			0x02
#define FLS_RD_DATA_INST			0x03

// flash device ID
#define MANUFACTURER_ID             239  //  0x01
#define DEVICE_ID					22 // 0x15

#define SEM_FLS_IO_TIMOUT			500//70//2000

ErrorStatus Flash_Init(void);
ErrorStatus Flash_Erase_Sector(u32 sctAddr);
ErrorStatus Flash_Check(void);



#endif /* FLASHAPP_H_ */

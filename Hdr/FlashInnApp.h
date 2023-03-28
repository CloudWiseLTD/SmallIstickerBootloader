/*
 * FlashInnApp.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#ifndef FLASHINNAPP_H_
#define FLASHINNAPP_H_

u16 CRC16_Clc(u8 * ptrPct, u16 pctLen, u16	crc16);
ErrorStatus Transfer_Version(u32 verByteLn, u32 extFlsAddr, u8 * dataBuff, u32 inFlsDstAdd);
ErrorStatus Inner_Flash_Erase(void);
ErrorStatus Inner_Flash_Write_Version(u8 * inFlsBuff, u32 inFlsBLen, u32 inFlsDstAdd);
ErrorStatus Inner_Flash_Write_Version_Mod(u8 * inFlsBuff, u32 inFlsBLen, u32 inFlsDstAdd);
ErrorStatus Inner_Flash_ClcCrc16(u32 verByteLn, u32 innFlsAdd, u16* pCrc16, u8* dataBuff, u32 buffLen);

#define INNFLS_STR_APP_ADD		/*0x8008000*/     0x800A000
#define INNFLS_END_APP_ADD		0x8040000

//#define INNFLS_MAX_APP_SIZE		245760
//#define INNFLS_MAX_APP_SIZE		(INNFLS_END_APP_ADD - INNFLS_STR_APP_ADD)
//#define INNFLS_MAX_APP_SIZE			(48*FLASH_SECTOR_SIZE)
#define INNFLS_MAX_APP_SIZE			(216*1024)

#endif /* FLASHINNAPP_H_ */

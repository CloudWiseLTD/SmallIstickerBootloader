/*
 * FlashDrv.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#ifndef FLASHDRV_H_
#define FLASHDRV_H_

ErrorStatus Flash_Write_Data(u8* wrtBuf,u16 wrtBufLen);
ErrorStatus Flash_Read_Data(u8* instBuf,u16 instBufLen,u8* readBuf,u16 readBufLen);
ErrorStatus SPI_Flash_SendByte(u8 wrtByte, u8* rdByte);

#endif /* FLASHDRV_H_ */

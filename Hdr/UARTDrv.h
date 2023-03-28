/*
 * UARTDrv.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */

#ifndef UARTDRV_H_
#define UARTDRV_H_

u32 SerialKeyPressedCh(u8* key);
u32 SerialKeyPressed(u8* key);
void SerialPutChar(u8 c);
void WaitForEndTx(void);

#endif /* UARTDRV_H_ */

/*
 * HostApp.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael Nesher
 */

#ifndef HOSTAPP_H_
#define HOSTAPP_H_

u8 GetKey(void);
u8 Get_Key_TimOut(void);
void Serial_PutString(u8* s);

//#define SerialPutString(x) Serial_PutString((uint8_t*)(x))

#endif /* HOSTAPP_H_ */

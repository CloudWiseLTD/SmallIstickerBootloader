/*
 * Ymodem.c
 *
 *  Created on: May 8, 2017
 *      Author: Michael Nesher
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "Defines.h"
#include "FlashInnApp.h"
#include "UARTDrv.h"
#include "Common.h"
#include "Ymodem.h"

uint8_t file_name[FILE_NAME_LENGTH];
uint8_t fileNam[50];

uint32_t FlashDestination = INNFLS_STR_APP_ADD; /* Flash user program offset */

uint16_t appVerNum = 0;
uint16_t appBldNum = 0;

#if 0
u32 RamSource = 0;
#else
u32* RamSource = NULL;
#endif

extern uint8_t tab_1024[1024];

/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *         -1: Timeout
  */
static  int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
	while (timeout-- > 0)
	{
		if (SerialKeyPressed(c) == 1)
		{
			return 0;
		}
	}

	return -1;
}

static uint32_t Send_Byte (uint8_t c, uint8_t f)
{
	if(f)
	{
		//GPIOC->BSRR |= GPIO_Pin_8;//	ISL3175_DE_ENABLE;
		//GPIOC->BSRR|= GPIO_Pin_7;//RS422_Tx_EN_RX_DIS

		SerialPutChar(c);

		WaitForEndTx();
	}
	else
	{
		SerialPutChar(c);
	}

	return 0;
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static int32_t Receive_Packet (uint8_t *data, int32_t *length, uint32_t timeout)
{
	uint16_t i, packet_size;

	uint8_t c;

	*length = 0;

	if (Receive_Byte(&c, timeout) != 0)
	{
		return -1;
	}

	switch (c)
	{
		case SOH:

			packet_size = PACKET_SIZE;

		break;

		case STX:

			packet_size = PACKET_1K_SIZE;

		break;

		case EOT:

			return 0;

		case CA:

			if ((Receive_Byte(&c, timeout) == 0) && (c == CA))
			{
				*length = -1;

				return 0;
			}
			else
			{
				return -1;
			}

		case ABORT1:
		case ABORT2:

			return 1;

		default:

			return -1;
	}

	*data = c;

	for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
	{
		if (Receive_Byte(data + i, timeout) != 0)
		{
			return -1;
		}
	}

	if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
	{
		return -1;
	}

	*length = packet_size;

	return 0;
}

int32_t Ymodem_Receive (uint8_t *buf)
{
	uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH], *file_ptr, *buf_ptr;
	int32_t i, j, packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;

	uint8_t *filePtrHd = NULL;
	uint8_t *filePtrTl = NULL;

	uint8_t num1, num2, num3;

	/* Initialize FlashDestination variable */
	FlashDestination = INNFLS_STR_APP_ADD;

	for (session_done = 0, errors = 0, session_begin = 0; ;)
	{
		for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
		{
			switch(Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))
			{
				case 0:

					errors = 0;

					switch (packet_length)
					{
						/* Abort by sender */
						case - 1:

							Send_Byte(ACK,1);

						return 0;

						/* End of transmission */
						case 0:

							Send_Byte(ACK,1);

							file_done = 1;

						break;

						/* Normal packet */
						default:

							if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
							{
								Send_Byte(NAK,1);
							}
							else
							{
								if (packets_received == 0)
								{
									/* Filename packet */
									if (packet_data[PACKET_HEADER] != 0)
									{
										/* Filename packet has valid data */
										for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
										{
											file_name[i++] = *file_ptr++;
										}

										file_name[i++] = '\0';


										memcpy(fileNam, file_name, 50);

										filePtrHd = fileNam;

										//if((filePtrTl = (u8*)strchr((const char*)filePtrHd, '.')) != NULL)
										if((filePtrTl = (u8*)strchr((const char*)filePtrHd, '-')) != NULL)
										{
											*filePtrTl = '\0';
											filePtrHd = filePtrTl+1;

											if((filePtrTl = (u8*)strchr((const char*)filePtrHd, '.')) != NULL)
											{
												*filePtrTl = '\0';
												num1 /*appVerNum*/ = (u16)atoi((const char*)filePtrHd);

												//filePtrHd = ++filePtrTl;
												filePtrHd = filePtrTl+1;

												if((filePtrTl = (u8*)strchr((const char*)filePtrHd, '.')) != NULL)
												{
													*filePtrTl = '\0';
													num2 /*appBldNum*/ = (u16)atoi((const char*)filePtrHd);
													filePtrHd = filePtrTl+1;

													appVerNum = num1*256 + num2;

													if((filePtrTl = (u8*)strchr((const char*)filePtrHd, '.')) != NULL)
													{
														*filePtrTl = '\0';
														num3 /*appBldNum*/ = (u16)atoi((const char*)filePtrHd);
														filePtrHd = filePtrTl+1;

														appBldNum = num3;
													}
												}
											}
										}

										for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
										{
											file_size[i++] = *file_ptr++;
										}

										file_size[i++] = '\0';

										Str2Int(file_size, &size);

										/* Test the size of the image to be sent */
										/* Image size is greater than Flash size */
										if (size > (FLASH_IMAGE_SIZE - 1)/*(FLASH_SIZE - 1)*/)
										{
											/* End session */
											Send_Byte(CA,1);

											Send_Byte(CA,1);

											return -1;
										}
										
										WD_Refresh();
										Inner_Flash_Erase();

										Send_Byte(ACK,1);

										Send_Byte(CRC16,1);
									}

									/* Filename packet is empty, end session */
									else
									{
										Send_Byte(ACK,1);

										file_done = 1;

										session_done = 1;

										break;
									}
								}
								/* Data packet */
								else
								{
									memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);

									RamSource = (u32*)buf;

									for (j = 0; (j < packet_length) && (FlashDestination <  INNFLS_STR_APP_ADD + size) ;j += 8)
									{
										if((((FlashDestination) < INNFLS_STR_APP_ADD) || ((FlashDestination) > INNFLS_END_APP_ADD)))
										{
											while(1);
										}

										/* Program the data received into STM32L433 Flash */
										if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlashDestination, *(u64*)RamSource) != HAL_OK)
										{

											/* End session */

											Send_Byte(CA,1);

											Send_Byte(CA,1);

											return -2;
										}

										WD_Refresh();

										if (*(u64*)FlashDestination != *(u64*)RamSource)
										{
											/* End session */

											Send_Byte(CA,1);

											Send_Byte(CA,1);

											return -2;
										}

										FlashDestination += 8;

										RamSource += 2;
									}

									Send_Byte(ACK,1);
								}

								packets_received ++;

								session_begin = 1;
							}
						}
				break;

				case 1:

					Send_Byte(CA,1);

					Send_Byte(CA,1);

					return -3;

				default:

					if (session_begin > 0)
					{
						errors ++;
					}
					if (errors > MAX_ERRORS)
					{
						Send_Byte(CA,1);

						Send_Byte(CA,1);

						return 0;
					}

					Send_Byte(CRC16,1);

				break;
			}

			if (file_done != 0)
			{
				break;
			}
		}

		if (session_done != 0)
		{
			break;
		}
	}

	return (int32_t)size;

}

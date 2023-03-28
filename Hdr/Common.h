/*
 * Common.h
 *
 *  Created on: May 8, 2017
 *      Author: Michael Nesher
 */

#ifndef COMMON_H_
#define COMMON_H_

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);
#define STM32F10X_MD

/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE       128

//!!!IAP #define ApplicationAddress    0x8003000
//#define ApplicationAddress    0x8008000  //!!!IAP
//#define ApplicationAddress    0x8000000  //!!!IAP
//#define ApplicationAddress    0x8004000 //

//#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 //#define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 //#define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
//#elif defined STM32F10X_CL
 //#define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE_APP                        (0x40000)  /* 256 KBytes */
//#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
// #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
// #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
//#elif defined STM32F10X_XL
// #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
// #define FLASH_SIZE                        (0x100000) /* 1 MByte */
//#else
// #error "Please select first the STM32 device to be used (in stm32f10x.h)"
//#endif

/* Compute the FLASH upload image size */
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE_APP - (INNFLS_STR_APP_ADD - 0x08000000))

/* Exported macro ------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);
void Int2Str(uint8_t* str, int32_t intnum);
// void Main_Menu_NoApp(void);
// void Main_Menu(void);
void Going_To_APP(void);


#endif /* COMMON_H_ */

/*
 * DvcPrm.h
 *
 *  Created on: May 7, 2017
 *      Author: Michael
 */

#ifndef DVCPRM_H_
#define DVCPRM_H_

#define FLS_TSK_FRT_ADD			0x40000


// This structure and app structure should be identical
typedef struct
{
	// 0
	char dpSrvIp[32];

	char ssIdName1[32];
	char ssIdName2[32];
	char ssIdName3[32];
	char ssIdName4[32];

	char ssIdPswrd1[32];
	char ssIdPswrd2[32];
	char ssIdPswrd3[32];
	char ssIdPswrd4[16];

	unsigned char Rev1;
	unsigned char Rev2;
	unsigned char Rev3;
	unsigned char Rev4;
	unsigned char Rev5;
	unsigned char Rev6;
	unsigned char Rev7;
	unsigned char Rev8;
	unsigned char Rev9;
	unsigned char Rev10;
	unsigned char Rev11;
	unsigned char Rev12;
	unsigned char Rev13;
	unsigned char Rev14;
	unsigned char GsmMode;
	unsigned char SecurityMode;

	char dpDvcID[16];

	u16 dpSrvUport;
	u16 dpSrvTDprt;

	u16 dpVersion;
	u16 dpBuild;
	u16 dpVersionR;
	u16 dpBuildR;
	u16 dpVerRcrc;
	u32 dpVerRdLn;

	u8 HardwareType;

	u32 dpCrc;
}
sDevice_Params;




ErrorStatus DevParms_Program_Flash(u8 * flsWrtBf, u16 flsWrtBfLn, u32 flsAddr);
ErrorStatus DevParms_Read_Flash(sDevice_Params * P_DevicePrms, u32 dpFlsAddr);
u32 CRC_Calc_Flash_Data(u8* pRdFlsbf, u16 dataBtSz);
ErrorStatus DevParms_InitBurn_Flash(sDevice_Params * P_DevicePrms);
ErrorStatus DevParms_Burn_Flash(sDevice_Params * P_DevicePrms);
void DevParms_Copy_Buffer(u8* pFlsbf, sDevice_Params * P_DevicePrms);
ErrorStatus Flash_Calc_Version_CRC16(u32 verByteLn, u32 extFlsAdd,u8 * dataBuff, u16 * pCrc16);
void DevParms_Set_Default(sDevice_Params * P_DevicePrms);
u32 Get_Param_Size_Without_CRC(void);


#endif /* DVCPRM_H_ */

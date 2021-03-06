/*
 * SRIO_drv.c
 *
 *  Created on: 2013-11-14
 *      Author: castMemory
 */

#include <stdio.h>
#include <SRIO_drv.h>
//#include <TSC_module.h>   //cs 14/11/05
#include <SRIO_error.h>
#include "TSC.h"  //cs 14/11/05

#define SRIO_LSU_REG4_ID_SIZE					0		//0表示采用8bit的ID号；	1表示采用16bit的ID号
#define SRIO_LSU_REG4_INTERRUPT_REQ_ENABLE		0		//0表示禁用中断
#define SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE		0		//0表示禁用中断

//转换成大端模式,数据位数32bit
#define DATA_IN_BIG_ENDIAN(data)				((data & 0x000000FF)<<24)	|		\
												((data & 0x0000FF00) << 8)	|		\
												((data & 0x00FF0000) >> 8)	|		\
												((data & 0xFF000000) >> 24)


extern CSL_SrioHandle      hSrio;		//这是个指针，指向的SRIO的寄存器

CSL_CgemRegs * CGEM_regs = (CSL_CgemRegs *)CSL_CGEM0_5_REG_BASE_ADDRESS_REGS;
//CSL_BootcfgRegs * gpBootCfgRegs = (CSL_BootcfgRegs *)CSL_BOOT_CFG_REGS;

//中断初始化，开启了中断4（for doorbell）和中断14（for Timer）
#pragma CODE_SECTION (Device_Interrupt_init, ".text:srio_section:dsp_srio");
void Device_Interrupt_init()
{
	/*map SRIO doorbell interrupts to INT4.
	map message descriptor accumulation low priority channel 0 interrupt
	to INT5*/
	CGEM_regs->INTMUX1 = CSL_GEM_INTDST_N_PLUS_16<<CSL_CGEM_INTMUX1_INTSEL4_SHIFT;

	CGEM_regs->INTMUX2 = 0;
	//map local timer interrupt to INT14	将定时器中断映射到了INT14上，中断号64，表示采用的是“核专属定时器”
	CGEM_regs->INTMUX3 = CSL_GEM_TINT4L<< CSL_CGEM_INTMUX3_INTSEL14_SHIFT;

	 /*Clear all DSP core events*/
	CGEM_regs->EVTCLR[0]= 	0xFFFFFFFF;
	CGEM_regs->EVTCLR[1]= 	0xFFFFFFFF;
	CGEM_regs->EVTCLR[2]= 	0xFFFFFFFF;
	CGEM_regs->EVTCLR[3]= 	0xFFFFFFFF;

	//clear DSP core interrupt flag
	ICR= IFR;

	//enable INT4, 5, 14
	IER = 3|(1<<4)|(1<<14);

	/*Interrupt Service Table Pointer to begining of LL2 memory*/
	ISTP= 0x800000;

	//enable GIE
	CSR = CSR|1;
}

/* SRIO Doorbell Interrupt flag */
Uint8 g_ucDoorbellIntrFlag = 0;

#pragma INTERRUPT (SRIO_Doorbell_ISR);			/* doorbell中断服务子函数 */
void SRIO_Doorbell_ISR()
{
	printf("DoorBell Interrupt received\n");
	g_ucDoorbellIntrFlag =  1;
}

/**********************************************************************
 ************************* DirectIO Driver   **************************
 **********************************************************************/
#pragma CODE_SECTION (DoLsuTransferSync, ".text:srio_section:dsp_srio");
inline Uint32 DoLsuTransferSync (							//此函数有返回值，用于判断是否重做
					SRIO_LSU_TRANSFER 	*pLsuConfig,
					Uint8 				LsuNum,
					SrioCompCode 		*pCompleteCode )
{
	Uint32 				uiTimeOut = 0;
	Uint8     			context,transID;
	Uint8				compCode,contextBit;

	/* Make sure there is space in the Shadow registers to write and Busy is not SET */
	do
	{
		if ( (hSrio->LSU_CMD[LsuNum].RIO_LSU_REG6) & 0xC0000000 )
			//CSL_SRIO_IsLSUBusy (hSrio, LsuNum) && CSL_SRIO_IsLSUFull (hSrio, LsuNum)
		{
			/* Update Timeout variable ，最大延时200us，即在这时间内LSU仍不可用，就会报Timeout		*/
			if ( uiTimeOut++ > SRIO_SYNC_XFER_TIMEOUT_INUS )
			{
				return SRIO_LSU_BUSY_FULL_INVALID;
			}

			/* delay 10us*/
			CycleDelay(10000);
		}
		else
			uiTimeOut = 0;
	}while (uiTimeOut != 0);

	// Get the LSU Context and Transaction Information. 获取LCB和LTID的值
	CSL_SRIO_GetLSUContextTransaction(hSrio,LsuNum,&context, &transID);

	// LSU的发送
	CSL_SRIO_SetLSUTransfer (hSrio, LsuNum, pLsuConfig);

	// Wait around till the transfer is completed.
	while (1)
	{
		if (CSL_SRIO_IsLSUBusy (hSrio, LsuNum) == FALSE)	//等待事务发送完成。
			break;
	}

    /* Initialise Completion code */
    *pCompleteCode = SRIO_TRANS_NO_ERR;

    /* Get LSU transfer completion code */
	uiTimeOut = 0;
	do
	{
		CSL_SRIO_GetLSUCompletionCode( hSrio,LsuNum,transID,&compCode,&contextBit);

		if(context == contextBit)
			uiTimeOut = 0;
		else
		{
			/* Update Timeout variable */
			if ( ++uiTimeOut > SRIO_SYNC_XFER_TIMEOUT_INUS )
			{
				return SRIO_GET_COMPLETECODE_TIMEOUT;
			}

			/* delay 10us  */
			CycleDelay(10000);;
		}

	}while (uiTimeOut != 0);

	CSL_SRIO_CancelLSUTransaction (hSrio, LsuNum, DNUM);

	*pCompleteCode = (SrioCompCode)compCode;

    return FUNC_SUCCESS;
}

#pragma CODE_SECTION (DoLsuTransferASync, ".text:srio_section:dsp_srio");
inline Uint32 DoLsuTransferASync (							//此函数对CC码没做处理
					SRIO_LSU_TRANSFER 	*pLsuConfig,
					Uint8 				LsuNum,
					SrioCompCode 		*pCompleteCode )
{
	Uint32 				uiTimeOut = 0;
	Uint8     			context,transID;

	/* Make sure there is space in the Shadow registers to write and Busy is not SET */
	do
	{
		if ( (hSrio->LSU_CMD[LsuNum].RIO_LSU_REG6) & 0xC0000000 )
			//CSL_SRIO_IsLSUBusy (hSrio, LsuNum) && CSL_SRIO_IsLSUFull (hSrio, LsuNum)
		{
			/* Update Timeout variable ，最大延时200us，即在这时间内LSU仍不可用，就会报Timeout		*/
			if ( uiTimeOut++ > SRIO_SYNC_XFER_TIMEOUT_INUS )
			{
				return SRIO_LSU_BUSY_FULL_INVALID;
			}

			/* delay 10us*/
			CycleDelay(10000);
		}
		else
			uiTimeOut = 0;
	}while (uiTimeOut != 0);

	// Get the LSU Context and Transaction Information. 获取LCB和LTID的值
	CSL_SRIO_GetLSUContextTransaction(hSrio,LsuNum,&context, &transID);

	// LSU的发送
	CSL_SRIO_SetLSUTransfer (hSrio, LsuNum, pLsuConfig);

	// Wait around till the transfer is completed.
	while (1)
	{
		if (CSL_SRIO_IsLSUBusy (hSrio, LsuNum) == FALSE)	//等待事务发送完成。
			break;
	}

	CSL_SRIO_CancelLSUTransaction (hSrio, LsuNum, DNUM);

    /* Initialise Completion code */
    *pCompleteCode = SRIO_TRANS_NO_ERR;

    return FUNC_SUCCESS;
}

#pragma CODE_SECTION (SRIO_SWriteWithDoorbell, ".text:srio_section:dsp_srio");
Uint32 SRIO_SWriteWithDoorbell (
					SRIO_Trans_Config 		*transPara,
                    Uint16 					usDstDevId,
                    char                   doorbell,
                    Uint16 					usDoorbellInfo,
                    SrioCompCode 			*pCompleteCode )
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;

    #ifdef _DEBUG
    /* Validate Pointer to source buffer */
    if(transPara->remote_addrL == NULL)
        return GEN_FUNC_ARG_INVALID;
    /* Validate length */
    if(transPara->byte_count > SRIO_LSU_MAX_DATA_XFER)
        return GEN_FUNC_ARG_INVALID;
    #endif

    /* usLen = 0 means nothing to do, return */
    if(transPara->byte_count <= 0)
        return GEN_FUNC_ARG_INVALID;

    if(transPara->byte_count >= SRIO_LSU_MAX_DATA_XFER )
    	transPara->byte_count = 0;

    /*    Configure SRIO Module
     */
    LsuConfigObj.dspAddress = transPara->local_addr;
    LsuConfigObj.rapidIOMSB = transPara->remote_addrH;
    LsuConfigObj.rapidIOLSB = transPara->remote_addrL;
    LsuConfigObj.bytecount = transPara->byte_count;
	LsuConfigObj.doorbellValid = doorbell;
    LsuConfigObj.idSize = SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = transPara->srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
	LsuConfigObj.ftype = 6;
	LsuConfigObj.ttype = 0;
    LsuConfigObj.hopCount = 0;
    LsuConfigObj.doorbellInfo = usDoorbellInfo;
	LsuConfigObj.supInt = 0;
	LsuConfigObj.srcIDMap = transPara->srioPortNum;

    /* Do the LSU transfer synchronously*/
    Err |= DoLsuTransferSync(&LsuConfigObj,transPara->srioLsuNum,pCompleteCode);

    return (Err);
}

#pragma CODE_SECTION (SrioMaintWrSync, ".text:srio_section:dsp_srio");
Uint32 SrioMaintWrSync (
					SRIO_Maint_Config 		maintPara,
					Uint16 					usDstDevId,							//目标设备ID
					Uint8 					ucHopCount,
					SrioCompCode 			*pCompleteCode )					//事务包的CC码
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;
    Uint32 uiTempData, uiTempDataGlobalAddress;

    /*    Configure SRIO Module
     */
    uiTempData =
#ifdef _LITTLE_ENDIAN
    	DATA_IN_BIG_ENDIAN(*(Uint32 *)maintPara.local_addr);
#else
	   *(Uint32 *)maintPara.local_addr;
#endif

	/* Need to calculate the global address of the temporary valiable */
    uiTempDataGlobalAddress = GET_C64X_GLOBAL_ADDR( (Uint32)&uiTempData );

    LsuConfigObj.dspAddress = uiTempDataGlobalAddress;
    LsuConfigObj.rapidIOMSB = 0;
    LsuConfigObj.rapidIOLSB = maintPara.remote_addrL;
    LsuConfigObj.bytecount = 4;
	LsuConfigObj.doorbellValid = 0;
    LsuConfigObj.idSize = SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = maintPara.srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
    LsuConfigObj.hopCount = (Uint8)ucHopCount;
    LsuConfigObj.doorbellInfo = 0;
	LsuConfigObj.ftype = 8;
	LsuConfigObj.ttype = 1;
	LsuConfigObj.supInt = 1;
	LsuConfigObj.srcIDMap = 0;

    /* Do the LSU transfer synchronously */
    Err |= DoLsuTransferSync(&LsuConfigObj,maintPara.srioLsuNum,pCompleteCode);

    return (Err);
}

#pragma CODE_SECTION (SrioMaintRdSync, ".text:srio_section:dsp_srio");
Uint32 SrioMaintRdSync (
					SRIO_Maint_Config 		maintPara,
					Uint16 					usDstDevId,
					Uint8 					ucHopCount,
					SrioCompCode 			*pCompleteCode )
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;

    /*    Configure SRIO Module
     */
    LsuConfigObj.dspAddress = (Uint32)maintPara.local_addr;
    LsuConfigObj.rapidIOMSB = 0;
    LsuConfigObj.rapidIOLSB = (Uint32)maintPara.remote_addrL;
    LsuConfigObj.bytecount = 0x4;
	LsuConfigObj.doorbellValid = 0;
    LsuConfigObj.idSize = SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = (Uint8)maintPara.srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
	LsuConfigObj.ftype = 8;
	LsuConfigObj.ttype = 0;
    LsuConfigObj.hopCount = ucHopCount;
    LsuConfigObj.doorbellInfo = 0;
	LsuConfigObj.supInt = 1;
	LsuConfigObj.srcIDMap = 0;

    /* Do the LSU transfer synchronously */
    Err |= DoLsuTransferSync(&LsuConfigObj,maintPara.srioLsuNum,pCompleteCode);

#ifdef _LITTLE_ENDIAN
    /* swap the endianness of the returned value */
    *(Uint32 *)maintPara.local_addr = DATA_IN_BIG_ENDIAN(*(Uint32 *)maintPara.local_addr);
#endif
    return (Err);
}

#pragma CODE_SECTION (SRIO_NWrite, ".text:BSLC667X_section:dsp_srio");
Uint32 SRIO_NWrite (
					SRIO_Trans_Config *transPara,
                    Uint16 usDstDevId,
                    SrioCompCode *pCompleteCode )
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;

    #ifdef _DEBUG
    /* Validate Pointer to source buffer */
    if(transPara->remote_addrL == NULL)
        return GEN_FUNC_ARG_INVALID;
    /* Validate length */
    if(transPara->byte_count > BSLC667X_SRIO_LSU_MAX_DATA_XFER)
        return GEN_FUNC_ARG_INVALID;
    #endif

    /* usLen = 0 means nothing to do, return */
    if(transPara->byte_count == 0)
        return GEN_FUNC_ARG_INVALID;

    if(transPara->byte_count == SRIO_LSU_MAX_DATA_XFER )
    	transPara->byte_count = 0;

    /*    Configure SRIO Module
     */
    LsuConfigObj.dspAddress = transPara->local_addr;
    LsuConfigObj.rapidIOMSB = transPara->remote_addrH;
    LsuConfigObj.rapidIOLSB = transPara->remote_addrL;
    LsuConfigObj.bytecount = transPara->byte_count;
	LsuConfigObj.doorbellValid = 0;
    LsuConfigObj.idSize = (Uint8)SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = transPara->srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = (Uint8)SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
	LsuConfigObj.ftype = 5;
	LsuConfigObj.ttype = 4;
    LsuConfigObj.hopCount = 0;
    LsuConfigObj.doorbellInfo = 0;
	LsuConfigObj.supInt = 0;
	LsuConfigObj.srcIDMap = transPara->srioPortNum;

    /* Do the LSU transfer synchronously*/
    Err |= DoLsuTransferASync(&LsuConfigObj,transPara->srioLsuNum,pCompleteCode);

    return (Err);
}

#pragma CODE_SECTION (SRIO_NWriteWithDoorbell, ".text:BSLC667X_section:dsp_srio");
Uint32 SRIO_NWriteWithDoorbell (
					SRIO_Trans_Config *transPara,
                    Uint16 usDstDevId,
                    Uint16 usDoorbellInfo,
                    SrioCompCode *pCompleteCode )
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;

    #ifdef _DEBUG
    /* Validate Pointer to source buffer */
    if(transPara->remote_addrL == NULL)
        return GEN_FUNC_ARG_INVALID;
    /* Validate length */
    if(transPara->byte_count > BSLC667X_SRIO_LSU_MAX_DATA_XFER)
        return GEN_FUNC_ARG_INVALID;
    #endif

    /* usLen = 0 means nothing to do, return */
    if(transPara->byte_count == 0)
        return GEN_FUNC_ARG_INVALID;

    if(transPara->byte_count == SRIO_LSU_MAX_DATA_XFER )
    	transPara->byte_count = 0;

    /*    Configure SRIO Module
     */
    LsuConfigObj.dspAddress = transPara->local_addr;
    LsuConfigObj.rapidIOMSB = transPara->remote_addrH;
    LsuConfigObj.rapidIOLSB = transPara->remote_addrL;
    LsuConfigObj.bytecount = transPara->byte_count;
	LsuConfigObj.doorbellValid = 1;
    LsuConfigObj.idSize = (Uint8)SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = transPara->srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = (Uint8)SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
	LsuConfigObj.ftype = 5;
	LsuConfigObj.ttype = 4;
    LsuConfigObj.hopCount = 0;
    LsuConfigObj.doorbellInfo = usDoorbellInfo;
	LsuConfigObj.supInt = 0;
	LsuConfigObj.srcIDMap = transPara->srioPortNum;

    /* Do the LSU transfer synchronously*/
    Err |= DoLsuTransferASync(&LsuConfigObj,transPara->srioLsuNum,pCompleteCode);

    return (Err);
}


#pragma CODE_SECTION (SRIO_NRead, ".text:BSLC667X_section:dsp_srio");
Uint32 SRIO_NRead (
					SRIO_Trans_Config *transPara,
                    Uint16 usDstDevId,
                    SrioCompCode *pCompleteCode )
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;

    #ifdef _DEBUG
    /* Validate Pointer to source buffer */
    if(transPara->remote_addrL == NULL)
        return GEN_FUNC_ARG_INVALID;
    /* Validate length */
    if(transPara->byte_count > SRIO_LSU_MAX_DATA_XFER)
        return GEN_FUNC_ARG_INVALID;
    #endif

    /* usLen = 0 means nothing to do, return */
    if(transPara->byte_count == 0)
        return GEN_FUNC_ARG_INVALID;

    if(transPara->byte_count == SRIO_LSU_MAX_DATA_XFER )
    	transPara->byte_count = 0;

    /*    Configure SRIO Module
     */
    LsuConfigObj.dspAddress = transPara->local_addr;
    LsuConfigObj.rapidIOMSB = transPara->remote_addrH;
    LsuConfigObj.rapidIOLSB = transPara->remote_addrL;
    LsuConfigObj.bytecount = transPara->byte_count;
	LsuConfigObj.doorbellValid = 0;
    LsuConfigObj.idSize = (Uint8)SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = transPara->srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = (Uint8)SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
	LsuConfigObj.ftype = 2;
	LsuConfigObj.ttype = 4;
    LsuConfigObj.hopCount = 0;
    LsuConfigObj.doorbellInfo = 0;
	LsuConfigObj.supInt = 0;
	LsuConfigObj.srcIDMap = transPara->srioPortNum;

    /* Do the LSU transfer synchronously*/
    Err |= DoLsuTransferASync(&LsuConfigObj,transPara->srioLsuNum,pCompleteCode);

    return (Err);
}

#pragma CODE_SECTION (SRIO_Doorbell, ".text:BSLC667X_section:dsp_srio");
Uint32 SRIO_Doorbell (
					SRIO_Trans_Config *transPara,
                    Uint16 usDstDevId,
                    Uint16 usDoorbellInfo,
                    SrioCompCode *pCompleteCode )
{
    Uint32 Err = FUNC_SUCCESS;
    SRIO_LSU_TRANSFER LsuConfigObj;

    #ifdef _DEBUG
    /* Validate Pointer to source buffer */
    if(transPara->remote_addrL == NULL)
        return GEN_FUNC_ARG_INVALID;
    /* Validate length */
    if(transPara->byte_count > SRIO_LSU_MAX_DATA_XFER)
        return GEN_FUNC_ARG_INVALID;
    #endif

    /* usLen = 0 means nothing to do, return */
    if(transPara->byte_count == 0)
        return GEN_FUNC_ARG_INVALID;

    if(transPara->byte_count == SRIO_LSU_MAX_DATA_XFER )
    	transPara->byte_count = 0;

    /*    Configure SRIO Module
     */
    LsuConfigObj.dspAddress = 0;
    LsuConfigObj.rapidIOMSB = 0;
    LsuConfigObj.rapidIOLSB = 0;
    LsuConfigObj.bytecount = 0;
	LsuConfigObj.doorbellValid = 0;
    LsuConfigObj.idSize = (Uint8)SRIO_LSU_REG4_ID_SIZE;
    LsuConfigObj.outPortID = transPara->srioPortNum;
    LsuConfigObj.xambs = 0;
    LsuConfigObj.priority = (Uint8)SRIO_LSU_REG4_PRIORITY_L0_H3_2;
    LsuConfigObj.dstID = usDstDevId;
    LsuConfigObj.intrRequest = (Uint8)SRIO_LSU_REG4_INTERRUPT_REQ_DISABLE;
	LsuConfigObj.ftype = 0xA;
	LsuConfigObj.ttype = 0;
    LsuConfigObj.hopCount = 0;
    LsuConfigObj.doorbellInfo = usDoorbellInfo;
	LsuConfigObj.supInt = 0;
	LsuConfigObj.srcIDMap = transPara->srioPortNum;

    /* Do the LSU transfer synchronously*/
    Err |= DoLsuTransferSync(&LsuConfigObj,transPara->srioLsuNum,pCompleteCode);

    return (Err);
}

//ackID值匹配
#pragma CODE_SECTION (match_ACKID_module, ".text:srio_section:dsp_srio");
Uint32 match_ACKID_module (SrioPortNum portNum, Uint16 usDstDevId)
{
	Uint32 Err = FUNC_SUCCESS;
	SRIO_Maint_Config 		maintPara;
	Uint8 					ucHopCount;
	SrioCompCode 			pCompleteCode;
	Uint32 uiMaintenanceValue, uiResult,uiGlobalAddress;
	Uint8 uiLocal_In_ACK_ID, uiLocal_out_ACK_ID, uiLocal_Outstanding_ACK_ID;
	Uint8 uiRemote_In_ACK_ID, uiRemote_out_ACK_ID;
	Uint8 linkStatus;

	CSL_SRIO_SendPortLinkMaintRequest(hSrio,portNum,0x3);

	while( !CSL_SRIO_IsLinkResponseReceived(hSrio,portNum) );
	CSL_SRIO_GetLinkResponseStatusInfo(hSrio,portNum,&uiRemote_In_ACK_ID,&linkStatus);
	printf("uiRemote_In = %d, linkStatus = 0x%x\n",uiRemote_In_ACK_ID,linkStatus);

	CSL_SRIO_SendPortLinkMaintRequest(hSrio,portNum,0x4);

	while( !CSL_SRIO_IsLinkResponseReceived(hSrio,portNum) );
	CSL_SRIO_GetLinkResponseStatusInfo(hSrio,portNum,&uiRemote_In_ACK_ID,&linkStatus);
	printf("uiRemote_In = %d, linkStatus = 0x%x\n",uiRemote_In_ACK_ID,linkStatus);

	CSL_SRIO_GetACKStatus(hSrio,portNum,
					&uiLocal_In_ACK_ID,&uiLocal_Outstanding_ACK_ID,&uiLocal_out_ACK_ID);
	printf("In = 0x%x,  Outstanding = 0x%x,  out= 0x%x\n",
					uiLocal_In_ACK_ID,uiLocal_Outstanding_ACK_ID,uiLocal_out_ACK_ID);

	CSL_SRIO_SetACKStatus(hSrio,portNum,uiLocal_In_ACK_ID,uiRemote_In_ACK_ID);

//	while( !CSL_SRIO_IsLinkResponseReceived(hSrio,portNum) );		//待确定
//	CSL_SRIO_GetLinkResponseStatusInfo(hSrio,portNum,&uiRemote_In_ACK_ID,&linkStatus);
	CycleDelay(359);

	do
	{
		uiMaintenanceValue= (  (uiRemote_In_ACK_ID+1)<<CSL_SRIO_RIO_SP_ACKID_STAT_INB_ACKID_SHIFT  )
						|uiLocal_In_ACK_ID;

		uiGlobalAddress = GET_C64X_GLOBAL_ADDR( (Uint32)&uiMaintenanceValue );

		maintPara.remote_addrL = 0x148+(0x20*portNum);
		maintPara.local_addr = uiGlobalAddress;
		maintPara.srioLsuNum = (SrioLsuNum)portNum;
		maintPara.srioPortNum = portNum;
		ucHopCount = 1;

		CSL_SRIO_GetACKStatus(hSrio,portNum,
					&uiLocal_In_ACK_ID,&uiLocal_Outstanding_ACK_ID,&uiLocal_out_ACK_ID);
		printf("In = 0x%x,  Outstanding = 0x%x,  out= 0x%x\n",
					uiLocal_In_ACK_ID,uiLocal_Outstanding_ACK_ID,uiLocal_out_ACK_ID);

		printf("LSU_Reg6---->0x%x\n",hSrio->LSU_CMD[0].RIO_LSU_REG6);
		Err = SrioMaintWrSync(maintPara,usDstDevId,ucHopCount,&pCompleteCode);
		if(Err)
		{
			printf("Err = %x\n",Err);
			continue;
		}

		CSL_SRIO_GetACKStatus(hSrio,portNum,
					&uiLocal_In_ACK_ID,&uiLocal_Outstanding_ACK_ID,&uiLocal_out_ACK_ID);
		printf("In = 0x%x,  Outstanding = 0x%x,  out= 0x%x\n",
					uiLocal_In_ACK_ID,uiLocal_Outstanding_ACK_ID,uiLocal_out_ACK_ID);
		printf("uiMaintenanceValue = 0x%x\n",uiMaintenanceValue);

		//maintPara.local_addr = (Uint32)&uiMaintenanceValue;
		Err = SrioMaintRdSync(maintPara,usDstDevId,ucHopCount,&pCompleteCode);

		CSL_SRIO_GetACKStatus(hSrio,portNum,
					&uiLocal_In_ACK_ID,&uiLocal_Outstanding_ACK_ID,&uiLocal_out_ACK_ID);
		printf("In = 0x%x,  Outstanding = 0x%x,  out= 0x%x\n",
					uiLocal_In_ACK_ID,uiLocal_Outstanding_ACK_ID,uiLocal_out_ACK_ID);
		printf("uiMaintenanceValue = 0x%x\n",uiMaintenanceValue);

		uiRemote_out_ACK_ID= uiMaintenanceValue & CSL_SRIO_RIO_SP_ACKID_STAT_OUTB_ACKID_MASK;
	}while( Err|(uiLocal_In_ACK_ID+1 != uiRemote_out_ACK_ID) );

	printf("match ACKID has completed!\n");


	return Err;

}

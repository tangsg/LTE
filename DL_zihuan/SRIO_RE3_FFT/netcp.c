#include "netcp.h"

extern netcphostDesc[SIZE_HOST_DESC * 32];
Uint32 network_device_init()
{
	BSLC667X_ErrorCode Err;
    if ((BSLC667X_DspCoreId_e)BSLC667X_GetDspCoreId() == BSLC667X_DSP_CORE_ID_0)
    {
    /* Default configuration will only be used if FRU read fails from DSP */
	BSLC667X_Libconfig_s LibConfig = {BSLC667X_CPU_CLK_FREQ_AUTO,BSLC667X_BOARD_CFG_DEFAULT_AMC2C6670,TRUE,TRUE,TRUE};

    /* Initialise PLLC, DDR3 and BSLC667X Library */
	Err = BSLC667X_Initialise(&LibConfig);
	if (Err)
	{
		printf("BSLC667X_Initialise() failed!!, ErrorCode = 0x%x\n",Err);
		return;
	}
    }

    return 0;
}


/* Copy Rx IP packet into uIP buffer if any */
Uint32 network_device_read(void* pPayloadData, Uint16* pPayloadLen)
{
	Uint32 poped_desc;
	Uint32 count = 0;

	MNAV_HostPacketDescriptor *pPkt;

	/***** Received Packets *****/
	count = GET_Q_ENTRY_COUNT(HOST_PA_RX_Q);
	if(count)
	{
		poped_desc = POP_QUEUE(HOST_PA_RX_Q);
		pPkt = (MNAV_HostPacketDescriptor *)(poped_desc & (Uint32)0xFFFFFFF0);
		memcpy((void*)pPayloadData,(void*)pPkt->buffer_ptr,pPkt->buffer_len);

		/* Remove 4 bytes of padding from DSP MNAV */
		*pPayloadLen = pPkt->buffer_len-4;

		//PUSH_QUEUE(HOST_PA_RX_FDQ, (Uint32)BSLC667X_C64X_GLOBAL_ADDR(CSL_chipReadReg(CSL_CHIP_DNUM),(Uint32 )pPkt));
		push_queue(HOST_PA_RX_FDQ, 1, 0, (unsigned int) (pPkt));
		return 0;
	}

	return 1;
}

/* Send IP packet */
Uint32 network_device_send(Int32 TXEmac, void* payloadData, Uint16 payloadLen )
{
	Uint32 count = 0, i = 0;
	Uint32 pop_q;

	MNAV_HostPacketDescriptor *host_pkt;

	/* Ensure packet size is 60 or more */
	if(payloadLen < 60)
		payloadLen += (60-payloadLen);

	/*get free queue*/
	count = GET_Q_ENTRY_COUNT(HOST_PA_TX_RETURN_Q);
	if(count) {
		pop_q = POP_QUEUE(HOST_PA_TX_RETURN_Q);
		pop_q &= 0xFFFFFFF0;

		host_pkt = (MNAV_HostPacketDescriptor *)pop_q;

		/***** Prepare Descriptors for Tx Free Descriptor Queue *****/
		memcpy((void *)host_pkt->orig_buff0_ptr, (void *)payloadData, payloadLen);
		//host_pkt->buffer_ptr = (Uint32)payloadData;
		host_pkt->buffer_len = payloadLen;
		host_pkt->packet_length = payloadLen;

			//PUSH_QUEUE(HOST_PA_TX_CH8_QUEUE, (Uint32)BSLC667X_C64X_GLOBAL_ADDR(CSL_chipReadReg(CSL_CHIP_DNUM),(Uint32 )host_pkt));
			push_queue(HOST_PA_TX_CH8_QUEUE, 1, 0, (unsigned int) (host_pkt));
		for (i=0; i<1000; i++)
			asm(" NOP 5");

		return 0;
	}

	return 1;
}

void netcp_main()
{
	unsigned int setupVal;
   	BSLC667X_ErrorCode Err;
    Int8 i,ij=0;
    Int8 cRxPtr=0;


    Uint16 PktSize, checksum;
    Uint8 temp;

	/* Initialise SGMII interface */
	Err = BSLC667X_NetcpInitialise(NULL);
	if (Err)
	{
		printf("BSLC667X_NetcpInitialise() failed!!, ErrorCode = 0x%x\n",Err);
		return ;
	}



    /***** Program the QM Link RAM Registers *****/
    /***** Programming Region 0 is optional *****/
	set_link_ram(0, QM_LRAM_REGION, QM_LRAM_SIZE);
	//set_link_ram(0, 0x90000000, QM_LRAM_SIZE);
	setupVal = (SIZE_HOST_DESC / 16 - 1) << 16;
	/***** Program Descriptor Memory Setup *****/
    /***** Only Region 0 is required to program *****/

	//set_memory_region(3, (Uint32) (Uint32)BSLC667X_C64X_GLOBAL_ADDR(CSL_chipReadReg(CSL_CHIP_DNUM),(Uint32 )netcphostDesc), 128, 0x30006); //Max number of descriptors in region#0 and 64B of each descriptor size
	set_memory_region(3, l2_global_address(0, (unsigned int) netcphostDesc), 160, setupVal);
	/*Setup the Rx & Tx queue*/

	{
		Uint16	idx = 0;
		MNAV_HostPacketDescriptor *host_pkt;

		/* Push 10 Host packets into Tx Completion Queue */
		for (idx = 0; idx < TX_HOST_DESC; idx ++)
		 {
		   host_pkt = (MNAV_HostPacketDescriptor *)(netcphostDesc + (idx * SIZE_HOST_DESC));
		   host_pkt->type_id = MNAV_DESC_TYPE_HOST;
		   host_pkt->ps_reg_loc = 0;
		   host_pkt->psv_word_count = 1;
		   host_pkt->epib = 0;										   // Extended packet info present
		   host_pkt->src_tag_hi = 0x01;
		   host_pkt->src_tag_lo = 0x01;

		   host_pkt->pkt_return_qmgr = 0; // Must be set to zero as only one q manager is supported
		   host_pkt->pkt_return_qnum = HOST_PA_TX_RETURN_Q;

		   host_pkt->orig_buff0_len = TX_BUFF_SIZE;
		   host_pkt->orig_buff0_ptr = (Uint32)(Netcp_TxBuff + (idx * TX_BUFF_SIZE));
		   host_pkt->next_desc_ptr = 0;
		   host_pkt->buffer_ptr = host_pkt->orig_buff0_ptr;

		   PUSH_QUEUE(HOST_PA_TX_RETURN_Q, (Uint32)BSLC667X_C64X_GLOBAL_ADDR(CSL_chipReadReg(CSL_CHIP_DNUM),(Uint32 )host_pkt));
		  // push_queue(HOST_PA_TX_RETURN_Q, 1, 0, (unsigned int) (host_pkt));
		 }


#if 1



		/* Push 10 Host packets into Rx FDQ */
		for (idx = TX_HOST_DESC; idx < TOTAL_DESC_NETCP; idx ++)
		{
		   host_pkt = (MNAV_HostPacketDescriptor *)(netcphostDesc + (idx * SIZE_HOST_DESC));

		   /* Set non-Rx overwrite fields */
		   host_pkt->type_id = MNAV_DESC_TYPE_HOST;
		   host_pkt->orig_buff0_len = RX_BUFF_SIZE;
		   host_pkt->orig_buff0_ptr = (Uint32)(Netcp_RxBuff + ((idx - TX_HOST_DESC) * RX_BUFF_SIZE));
		   host_pkt->next_desc_ptr = 0;
		   host_pkt->psv_word_count = 1;
		   host_pkt->epib = 1;
		   host_pkt->buffer_len = RX_BUFF_SIZE;
		   host_pkt->buffer_ptr = host_pkt->orig_buff0_ptr;
		   host_pkt->next_desc_ptr = 0; //don't link Host buffers in Rx FDQ
		   host_pkt->src_tag_hi = 1;
		   host_pkt->src_tag_lo = 1;

		  // PUSH_QUEUE(HOST_PA_RX_FDQ, (Uint32)BSLC667X_C64X_GLOBAL_ADDR(CSL_chipReadReg(CSL_CHIP_DNUM),(Uint32 )host_pkt));
		   push_queue(HOST_PA_RX_FDQ, 1, 0, (unsigned int) (host_pkt));
		}
	}

    /***** Rx Flow configuration *****/
	/***** Rx Flow # 22 *****/
	for(i=0;i<32;i++)
	{
	    PA_RX_FLOW_REG_A(i) = 0x00000000 + HOST_PA_RX_Q;	//Host Descriptors with no SOP offset
	    PA_RX_FLOW_REG_B(i) = 0;
	    PA_RX_FLOW_REG_C(i) = 0;	//0x24000000; // RX_SRC_TAG_HI_SEL = 2 = overwrite with flow_id[7-0] from back end application
	    										  // RX_SRC_TAG_LO_SEL = 4 = overwrite with dest_tag[7-0] from back end application
	    PA_RX_FLOW_REG_D(i) = (HOST_PA_RX_FDQ << 16);
	    PA_RX_FLOW_REG_E(i) = 0;
	    PA_RX_FLOW_REG_F(i) = 0;
	    PA_RX_FLOW_REG_G(i) = 0;		//Optional fields 0x80
	    PA_RX_FLOW_REG_H(i) = 0;
	}

    /***** TX Channel Configuration *****/
    /***** Enable all 10 Tx channels *****/
    for(i=0;i<10;i++)
    {
		PA_TX_CHA_REG_A(i) = 0x80000000;		// Enable the Tx Channel
    }

    /***** Enabled PA Rx Channel 22 *****/
	for(i=0;i<24;i++)
   		PA_RX_CHA_REG_A(i) = 0x80000000;		// Enable the Rx Channel

    /***** Program the PA CDMA Loopback Disable *****/
    PA_CDMA_EMU_CNTL = 0x00000000;	// Program for Disable Loopback operation



#if 0
	//network_device_init();

	/* Store TI-e-fuse MAC address */
	ucMacAddr[0] = ((hBootCfg->EMAC_ID2) >>  8) & 0xFF;
	ucMacAddr[1] = ((hBootCfg->EMAC_ID2) >>  0) & 0xFF;
	ucMacAddr[2] = ((hBootCfg->EMAC_ID1) >> 24) & 0xFF;
	ucMacAddr[3] = ((hBootCfg->EMAC_ID1) >> 16) & 0xFF;
	ucMacAddr[4] = ((hBootCfg->EMAC_ID1) >>  8) & 0xFF;
	ucMacAddr[5] = ((hBootCfg->EMAC_ID1) >>  0) & 0xFF;

	printf("Execute \"arp -s 192.168.1.%d %02x-%02x-%02x-%02x-%02x-%02x\" on command prompt\n", (g_BoardInfo.DspId+1), ucMacAddr[0], ucMacAddr[1], ucMacAddr[2], ucMacAddr[3], ucMacAddr[4], ucMacAddr[5]);
	printf("try ping to above IP address\n");
#endif
#if 0
	while(1)
	{
		if( network_device_read(g_RxDataBuff, &PktSize) == 0 )
		{
			/* Manually filter MAC address for now */
			if( (g_RxDataBuff[0] == ucMacAddr[0]) &&
					(g_RxDataBuff[1] == ucMacAddr[1]) &&
					(g_RxDataBuff[2] == ucMacAddr[2]) &&
					(g_RxDataBuff[3] == ucMacAddr[3]) &&
					(g_RxDataBuff[4] == ucMacAddr[4]) &&
					(g_RxDataBuff[5] == ucMacAddr[5]))
			{
				/* Process the packet */
				uiPktRcvd++;
				memcpy((void *)type1[ij],g_RxDataBuff+42,PktSize-42);
				ij++;
				if(ij==5)
					ij=0;
				//printf("Pkt Received of size %d - %d\n", PktSize, uiPktRcvd++);
#if 1
				/* reverse src to dst mac addr */
				for (i=0;i<6;i++)
				{
					temp = g_RxDataBuff[i];
					g_RxDataBuff[i]=g_RxDataBuff[i+6];
					g_RxDataBuff[i+6]=temp;
				}

				/* reverse src to dst ip addr */
				for (i=0;i<4;i++)
				{
					temp = g_RxDataBuff[26+i];
					g_RxDataBuff[26+i] = g_RxDataBuff[30+i];
					g_RxDataBuff[30+i] = temp;
				}
     #if 1
				//this is ping response
				g_RxDataBuff[34]=0x00;
				//this is check sum
				g_RxDataBuff[36]=0x00;
				g_RxDataBuff[37]=0x00;
				// generate new check sum
				checksum = PingCheckSum(&(g_RxDataBuff[34]),PktSize-34);
				//fill check sum
				g_RxDataBuff[36]=checksum&0xFF;
				g_RxDataBuff[37]=(checksum&0xFF00)>>8;
    #endif
				if(network_device_send(0,g_RxDataBuff,PktSize))
					//printf("Failed to send packet\n");
					uiPktsend++;
				else
					//printf("Pkt Sent of size %d\n",PktSize);
					uiPktsend++;
    #endif
			}
		}
	}
#endif
#endif
}

/* IP Checksum Generator */
Uint16 PingCheckSum(Uint8 *pSendBuffer, Int32 iCheckSumLen)
{
	Uint32 sum=0;
	Uint32 checksum;
    Int32 iIndex;
    Uint32 temp;
	for ( iIndex = 0; iIndex < iCheckSumLen; iIndex++)
    {
        temp = (pSendBuffer[iIndex+1] << 8)+pSendBuffer[iIndex];
		sum += temp;
        iIndex++;
    }
    checksum = sum &0x0000FFFF;
    checksum += (sum & 0xFFFF0000)>>16;
	return ((Uint16)~checksum);
}


 
#ifndef _CSL_SRIO_H_
#define _CSL_SRIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"
#include "csl.h"
#include "cslr_srio.h"

/**
@defgroup CSL_SRIO_SYMBOL  SRIO Symbols Defined
@ingroup CSL_SRIO_API
*/
/**
@defgroup CSL_SRIO_DATASTRUCT  SRIO Data Structures
@ingroup CSL_SRIO_API
*/
/**
@defgroup CSL_SRIO_FUNCTION  SRIO Functions
@ingroup CSL_SRIO_API
*/

/** @addtogroup CSL_SRIO_DATASTRUCT
 @{ */
        
/** @brief This is the handle to the SRIO instance */
typedef volatile CSL_SrioRegs*   CSL_SrioHandle;

/** @brief SRIO Message Description.
 *
 * This structure is used to define a SRIO message and is used by the
 * SRIO module to compare all received RIO packets to determine the 
 * receive queue where the packet is pushed to.
 */
typedef struct SRIO_MESSAGE
{
    unsigned short  srcId;
    unsigned char   mbx;
    unsigned char   ltr;
    unsigned char   mbxMask;
    unsigned char   ltrMask;
    unsigned char   segMap;
    unsigned char   srcProm;
    unsigned char   tt;
    unsigned char   dstProm;
    unsigned short  dstId;
    unsigned char   flowId;
}SRIO_MESSAGE;

/** @brief SRIO Type 9 Message Description.
 *
 * This structure is used to define the TYPE9 Message. This is then
 * used to MAP Type9 messages to a specific destination queue.
 */
typedef struct SRIO_TYPE9_MESSAGE
{
    unsigned short  srcId;
    unsigned char   cos;
    unsigned char   cosMask;
    unsigned char   srcProm;
    unsigned char   tt;
    unsigned char   dstProm;
    unsigned short  dstId;
    unsigned short  streamId;
    unsigned short  streamMask;
    unsigned char   flowId;
}SRIO_TYPE9_MESSAGE;

/** @brief SRIO AMU Window
 *
 * This structure is used to define the SRIO AMU Window
 * properties.
 */
typedef struct SRIO_AMU_WINDOW
{
    unsigned char   xambs;
    unsigned char   paneCount;
    unsigned short  paneSize;
    unsigned char   winSize;
    unsigned int  rapidIOMsb;
    unsigned int  rapidIOLsb;
}SRIO_AMU_WINDOW;

/** @brief SRIO AMU Pane
 *
 * This structure is used to define the SRIO AMU Pane
 * properties.
 */
typedef struct SRIO_AMU_PANE
{
    unsigned char   cmdEnc;
    unsigned char   portId;
    unsigned char   idSize;
    unsigned short  dstID;
}SRIO_AMU_PANE;

/** @brief SRIO LSU Transfer
 *
 * This structure is used to configure LSU module for Transfer
 */
typedef struct SRIO_LSU_TRANSFER 
{
    unsigned int  rapidIOMSB;
    unsigned int  rapidIOLSB;
    unsigned int  dspAddress;
    unsigned int  bytecount;
    unsigned char   doorbellValid;
    unsigned char   intrRequest;
    unsigned char   supInt;
    unsigned char   xambs;
    unsigned char   priority;
    unsigned char   outPortID;
    unsigned char   idSize;
    unsigned char   srcIDMap;
    unsigned short  dstID;
    unsigned char   ttype;
    unsigned char   ftype;
    unsigned char   hopCount;
    unsigned short  doorbellInfo;
}SRIO_LSU_TRANSFER;

/** @brief SRIO Processing Element Features
 *
 * This structure is used to configure the Tundra 
 * Processing Element Features.
 */
typedef struct SRIO_PE_FEATURES 
{
    unsigned char   isBridge;
    unsigned char   isEndpoint;
    unsigned char   isProcessor;
    unsigned char   isSwitch;
    unsigned char   isMultiport;
    unsigned char   isFlowArbiterationSupported;
    unsigned char   isMulticastSupported;
    unsigned char   isExtendedRouteConfigSupported;
    unsigned char   isStandardRouteConfigSupported;
    unsigned char   isFlowControlSupported;
    unsigned char   isCRFSupported;
    unsigned char   isCTLSSupported;
    unsigned char   isExtendedFeaturePtrValid;
    unsigned char   numAddressBitSupported;
}SRIO_PE_FEATURES;

/** @brief SRIO Operation Capability Register
 *
 * This structure is used to store the operation capability
 * configuration. 
 */
typedef struct SRIO_OP_CAR
{
    unsigned char   gsmRead;
    unsigned char   gsmInstrnRead;
    unsigned char   gsmReadOwn;
    unsigned char   gsmDataCacheInvalidate;
    unsigned char   gsmCastout;
    unsigned char   gsmDataCacheFlush;
    unsigned char   gsmIORead;
    unsigned char   gsmInstrnCacheInvalidate;
    unsigned char   gsmTLBInvalidate;
    unsigned char   gsmTLBSync;
    unsigned char   dataStreamingTM;
    unsigned char   dataStreamingSupport;
    unsigned char   implnDefined;
    unsigned char   readSupport;
    unsigned char   writeSupport;
    unsigned char   streamWriteSupport;
    unsigned char   writeResponseSupport;
    unsigned char   dataMessageSupport;
    unsigned char   doorbellSupport;
    unsigned char   atomicCompareSwapSupport;
    unsigned char   atomicTestSwapSupport;
    unsigned char   atomicIncSupport;
    unsigned char   atomicDecSupport;
    unsigned char   atomicSetSupport;
    unsigned char   atomicClearSupport;
    unsigned char   atomicSwapSupport;
    unsigned char   portWriteOperationSupport;
    unsigned char   implnDefined2;
}SRIO_OP_CAR;

/** @brief SRIO Error Rate 
 *
 * This structure is used to store the error rate CSR configuration
 * which is used to monitor and control the port physical layer
 * errors. 
 */
typedef struct SRIO_ERR_RATE
{
    unsigned char   errRateBias;
    unsigned char   errRateRecovery;
    unsigned char   peak;
    unsigned char   errRateCnt;
}SRIO_ERR_RATE;

/** @brief SRIO Lane Status
 *
 * This structure is used to store the lane status information.
 */
typedef struct SRIO_LANE_STATUS
{
    unsigned char   portNum;
    unsigned char   laneNum;
    unsigned char   txType;
    unsigned char   txMode;
    unsigned char   rxType;
    unsigned char   rxInv;
    unsigned char   rxTrn;
    unsigned char   rxSync;
    unsigned char   rxReady;
    unsigned char   errCnt;
    unsigned char   chgSync;
    unsigned char   chgTrn;
    unsigned char   stat1;
    unsigned char   stat2_7;
    unsigned char   idle2;
    unsigned char   infoOk;
    unsigned char   chg;
    unsigned char   implSpecific;
    unsigned char   lpRxTrn;
    unsigned char   lpWidth;
    unsigned char   lpLaneNum;
    unsigned char   lpTapM1;
    unsigned char   lpTapP1;
    unsigned char   lpScrm;
}SRIO_LANE_STATUS;

/** @brief SRIO Lane Status
 *
 * This structure is used to store the PLM Implementation specific
 * control register
 */
typedef struct SRIO_PLM_IMPL_CONTROL
{
    unsigned char   payloadCapture;
    unsigned char   useIdle2;
    unsigned char   useIdle1;
    unsigned char   dlbEn;
    unsigned char   forceReinit;
    unsigned char   softRstPort;
    unsigned char   txBypass;
    unsigned char   lineLoopbackMode;
    unsigned char   portSelfReset;
    unsigned char   selfReset;
    unsigned char   swapTx;
    unsigned char   swapRx;
    unsigned char   dltThresh;
}SRIO_PLM_IMPL_CONTROL;

/** @brief SRIO PLM VMin Exponent
 *
 * This structure is used to store the PLM VMin Exponent configuration
 */
typedef struct SRIO_PLM_VMIN_EXPONENT
{
    unsigned char   vminExp;
    unsigned char   imax;
    unsigned char   mmax;
}SRIO_PLM_VMIN_EXPONENT;

/** @brief SRIO PLM Polarity Control
 *
 * This structure is used to store the PLM Polarity Control
 */
typedef struct SRIO_PLM_POLARITY_CONTROL
{
    unsigned char   tx3Pol;
    unsigned char   tx2Pol;
    unsigned char   tx1Pol;
    unsigned char   tx0Pol;
    unsigned char   rx3Pol;
    unsigned char   rx2Pol;
    unsigned char   rx1Pol;
    unsigned char   rx0Pol;
}SRIO_PLM_POLARITY_CONTROL;

/** @brief SRIO PLM Control Symbol Configuration
 *
 * This structure is used to store the PLM Control Symbol configuration
 */
typedef struct SRIO_PLM_CONTROL_SYMBOL
{
    unsigned char   stype0;
    unsigned char   par0;
    unsigned char   par1;
    unsigned char   csEmb;
    unsigned char   stype1;
    unsigned char   cmd;
    unsigned char   stype2;
    unsigned char   parm;
}SRIO_PLM_CONTROL_SYMBOL;

/** @brief SRIO TLM Control Configuration
 *
 * This structure is used to store the SRIO TLM Control configuration
 */
typedef struct SRIO_TLM_CONTROL
{
    unsigned char   portGroupSelect;
    unsigned char   voqSelect;
    unsigned char   tgtIDDis;
    unsigned char   mtcTgtIDDis;
    unsigned char   length;
}SRIO_TLM_CONTROL;

/**
@}
*/

/* Device specific API which opens the SRIO instance and returns a handle used in all subsequent calls */
extern CSL_SrioHandle CSL_SRIO_Open (signed int instNum);

#ifdef __cplusplus
}
#endif

#endif /* _CSL_SRIO_H_ */



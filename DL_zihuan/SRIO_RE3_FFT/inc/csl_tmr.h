

#ifndef _CSL_TMR_H_
#define _CSL_TMR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"
#include "csl.h"
#include "cslr_tmr.h"

/**
@defgroup CSL_TIMER_SYMBOL  TIMER Symbols Defined
@ingroup CSL_TIMER_API
*/
/**
@defgroup CSL_TIMER_DATASTRUCT  TIMER Data Structures
@ingroup CSL_TIMER_API
*/
/**
@defgroup CSL_TIMER_FUNCTION  TIMER Functions
@ingroup CSL_TIMER_API
*/
/**
@defgroup CSL_TIMER_ENUM TIMER Enumerated Data Types
@ingroup CSL_TIMER_API
*/

/**
@addtogroup CSL_TIMER_ENUM
@{
*/

/**
 * General purpose global type def declarations
 */

/**
 *  @brief This enum describes the commands used to control the GP timer through
 *  CSL_tmrHwControl()
 */
typedef enum {
    /**
     * @brief   Loads the GP Timer Period Register Low
     * @param   unsigned int *
     */
    CSL_TMR_CMD_LOAD_PRDLO = 0,

    /**
     * @brief   Loads the GP Timer Period Register High
     * @param   unsigned int *
     */
    CSL_TMR_CMD_LOAD_PRDHI = 1,

    /**
     * @brief   Loads the GP Timer Pre-scalar value for CNTHI
     * @param   unsigned char *
     */
    CSL_TMR_CMD_LOAD_PSCHI = 2,

    /**
     * @brief   Enable the GP timer Low
     * @param   CSL_TmrEnamode
     */
    CSL_TMR_CMD_START_TIMLO = 3,

    /**
     * @brief   Enable the GP timer High
     * @param   CSL_TmrEnamode
     */
    CSL_TMR_CMD_START_TIMHI = 4,

    /**
     * @brief   Stop the GP timer Low
     * @param   None
     */
    CSL_TMR_CMD_STOP_TIMLO = 5,

    /**
     * @brief   Stop the GP timer High
     * @param   None
     */
    CSL_TMR_CMD_STOP_TIMHI = 6,

    /**
     * @brief   Reset the GP timer Low
     * @param   None
     */
    CSL_TMR_CMD_RESET_TIMLO = 7,

    /**
     * @brief   Reset the GP timer High
     * @param   None
     */
    CSL_TMR_CMD_RESET_TIMHI = 8,

    /**
     * @brief   Start the timer in GPtimer64 OR Chained mode
     * @param   None
     */
    CSL_TMR_CMD_START64 = 9,

    /**
     * @brief   Stop the timer of GPtimer64 OR Chained
     * @param   CSL_TmrEnamode
     */
    CSL_TMR_CMD_STOP64 = 10,

    /**
     * @brief   Reset the timer of GPtimer64 OR Chained
     * @param   None
     */
    CSL_TMR_CMD_RESET64 = 11,

    /**
     * @brief	Enable the timer in watchdog mode
     * @param	CSL_TmrEnamode
     */
    CSL_TMR_CMD_START_WDT = 12,

    /**
     * @brief	Loads the watchdog key
     * @param	unsigned short
     */
    CSL_TMR_CMD_LOAD_WDKEY = 13
} CSL_TmrHwControlCmd;

/**
 *  @brief This enum describes the commands used to get status of various parameters of the
 *  GP timer. These values are used in CSL_tmrGetHwStatus()
 */
typedef enum {
    /**
     * @brief   Gets the current value of the GP timer CNTLO register
     * @param   unsigned int *
     */
    CSL_TMR_QUERY_COUNT_LO = 0,

    /**
     * @brief   Gets the current value of the GP timer CNTHI register
     * @param   unsigned int *
     */
    CSL_TMR_QUERY_COUNT_HI = 1,
    /**
     * @brief   This query command returns the status
     *          about whether the CNTLO is running or stopped
     *
     * @param   CSL_TmrTstat
     */
    CSL_TMR_QUERY_TSTAT_LO = 2,

    /**
     * @brief   This query command returns the status
     *          about whether the CNTHI is running or stopped
     *
     * @param   CSL_TmrTstat
     */
    CSL_TMR_QUERY_TSTAT_HI = 3,

    /**
     * @brief	This query command returns the status about whether the timer
     *		is in watchdog mode or not
     * @param	CSL_WdflagBitStatus
     */
    CSL_TMR_QUERY_WDFLAG_STATUS = 4
} CSL_TmrHwStatusQuery;

/**
 *  @brief This enum describes whether the Timer Clock input is gated or not gated.
 */
typedef enum {
    /** timer input not gated */
    CSL_TMR_CLOCK_INP_NOGATE = 0,

    /** timer input gated */
    CSL_TMR_CLOCK_INP_GATE = 1
} CSL_TmrIpGate;

/**
 *  @brief This enum describes the Timer Clock source selection.
 */
typedef enum {
    /** timer clock INTERNAL source selection */
    CSL_TMR_CLKSRC_INTERNAL = 0,

    /** timer clock Timer input pin source selection */
    CSL_TMR_CLKSRC_TMRINP = 1
} CSL_TmrClksrc;

/**
 *  @brief This enum describes the enabling/disabling of Timer.
 */
typedef enum {
    /** The timer is disabled and maintains current value */
    CSL_TMR_ENAMODE_DISABLE = 0,

    /**  The timer is enabled one time */
    CSL_TMR_ENAMODE_ENABLE = 1,

    /**  The timer is enabled continuously */
    CSL_TMR_ENAMODE_CONT = 2
} CSL_TmrEnamode;

/**
 *  @brief This enum describes the Timer Clock cycles (1/2/3/4).
 */
typedef enum {
    /** One timer clock cycle */
    CSL_TMR_PWID_ONECLK = 0,

    /** Two timer clock cycle */
    CSL_TMR_PWID_TWOCLKS = 1,

    /** Three timer clock cycle */
    CSL_TMR_PWID_THREECLKS = 2,

    /** Four timer clock cycle */
    CSL_TMR_PWID_FOURCLKS = 3
} CSL_TmrPulseWidth;

/**
 *  @brief This enum describes the mode of Timer Clock (Pulse/Clock).
 */
typedef enum {
    /** Pulse mode */
    CSL_TMR_CP_PULSE = 0,

    /** Clock mode */
    CSL_TMR_CP_CLOCK = 1
} CSL_TmrClockPulse;

/**
 *  @brief This enum describes the Timer input inverter control
 */
typedef enum {
    /** Un inverted timer input drives timer */
    CSL_TMR_INVINP_UNINVERTED = 0,

    /** Inverted timer input drives timer */
    CSL_TMR_INVINP_INVERTED = 1
} CSL_TmrInvInp;

/**
 *  @brief This enum describes the Timer output inverter control
 */
typedef enum {
    /** Un inverted timer output */
    CSL_TMR_INVOUTP_UNINVERTED = 0,

    /** Inverted timer output */
    CSL_TMR_INVOUTP_INVERTED = 1
} CSL_TmrInvOutp;

/**
 *  @brief This enum describes the mode of Timer (GPT/WDT/Chained/Unchained).
 */
typedef enum {
    /** The timer is in 64-bit GP timer mode */
    CSL_TMR_TIMMODE_GPT = 0,

    /** The timer is in dual 32-bit timer, unchained mode */
    CSL_TMR_TIMMODE_DUAL_UNCHAINED = 1,

    /** The timer is in 64-bit Watchdog timer mode */
    CSL_TMR_TIMMODE_WDT = 2,

    /** The timer is in dual 32-bit timer, chained mode */
    CSL_TMR_TIMMODE_DUAL_CHAINED = 3
} CSL_TmrMode;

/**
 *  @brief This enum describes the reset condition of Timer (ON/OFF).
 */
typedef enum {
    /** timer TIMxx is in reset */
    CSL_TMR_TIMxxRS_RESET_ON = 0,

    /** timer CNTHI is not in reset. CNTHI can be used as a 32-bit timer */
    CSL_TMR_TIMxxRS_RESET_OFF = 1
} CSL_TmrState;

/**  @brief This enum describes the status of Timer.
 */
typedef enum {
    /** Timer status drives High */
    CSL_TMR_TSTAT_HIGH = 1,

    /** Timer status drives Low */
    CSL_TMR_TSTAT_LOW = 0
} CSL_TmrTstat;

/** @brief This enumeration describes the flag bit status of the timer in watchdog mode
 */
typedef enum {
	/** No watchdog timeout occurred */
	CSL_TMR_WDFLAG_NOTIMEOUT = 0,

	/** Watchdog timeout occurred */
	CSL_TMR_WDFLAG_TIMEOUT = 1
} CSL_TmrWdflagBitStatus;

/**
@}
*/

/** @addtogroup CSL_TIMER_DATASTRUCT
 @{ */

/**
 * @brief Hardware setup structure.
 */
typedef struct {
    /** 32 bit load value to be loaded to Timer Period Register low */
    unsigned int tmrTimerPeriodLo;

    /** 32 bit load value to be loaded to Timer Period Register High */
    unsigned int tmrTimerPeriodHi;

    /** 32 bit load value to be loaded to Timer Counter Register Low */
    unsigned int tmrTimerCounterLo;

    /** 32 bit load value to be loaded to Timer Counter Register High */
    unsigned int tmrTimerCounterHi;

    /** TIEN determines if the timer clock is gated by the timer input.
     *  Applicable only when CLKSRC=0
     */
    CSL_TmrIpGate tmrIpGateHi;

    /** CLKSRC determines the selected clock source for the timer */
    CSL_TmrClksrc tmrClksrcHi;

    /** Pulse width. used in pulse mode (C/P_=0) by the timer */
    CSL_TmrPulseWidth tmrPulseWidthHi;

    /** Clock/Pulse mode for timerHigh output */
    CSL_TmrClockPulse tmrClockPulseHi;

    /** Timer input inverter control. Only affects operation
     *  if CLKSRC=1, Timer Input pin
     */
    CSL_TmrInvInp tmrInvInpHi;

    /** Timer output inverter control */
    CSL_TmrInvOutp tmrInvOutpHi;

    /** TIEN determines if the timer clock is gated by the timer input.
     *  Applicable only when CLKSRC=0
     */
    CSL_TmrIpGate tmrIpGateLo;

    /** CLKSRC determines the selected clock source for the timer */
    CSL_TmrClksrc tmrClksrcLo;

    /** Pulse width. used in pulse mode (C/P_=0) by the timer */
    CSL_TmrPulseWidth tmrPulseWidthLo;

    /** Clock/Pulse mode for timerLow output */
    CSL_TmrClockPulse tmrClockPulseLo;

    /** Timer input inverter control. Only affects operation
     *  if CLKSRC=1, Timer Input pin
     */
    CSL_TmrInvInp tmrInvInpLo;

    /** Timer output inverter control */
    CSL_TmrInvOutp tmrInvOutpLo;

    /** CNTHI pre-scalar counter specifies the count for CNTHI */
    unsigned char tmrPreScalarCounterHi;

    /** Configures the GP timer in GP mode or in
     *  general purpose timer mode or Dual 32 bit timer mode
     */
    CSL_TmrMode tmrTimerMode;
} CSL_TmrHwSetup;

/**
 *  @brief Config-structure Used to configure the GP timer using CSL_tmrHwSetupRaw()
 */
typedef struct {
    /** Timer Counter Register Low */
    unsigned int CNTLO;
    /** Timer Counter Register High */
    unsigned int CNTHI;
    /** Timer Period Register Low */
    unsigned int PRDLO;
    /** Timer Period Register High */
    unsigned int PRDHI;
    /** Timer Control Register */
    unsigned int TCR;
    /** Timer Global Control Register */
    unsigned int TGCR;
    /** Watchdog Timer Control Register */
    unsigned int WDTCR;
} CSL_TmrConfig;

/**
 * @brief Module specific context information. Present implementation of GP
 *        timer CSL doesn't have any context information.
 */
typedef struct {
    /** Context information of GP timer CSL.
     *  The below declaration is just a place-holder for future implementation.
     */
    unsigned short contextInfo;
} CSL_TmrContext;

/** @brief Module specific parameters. Present implementation of GP timer CSL
 *         doesn't have any module specific parameters.
 */
typedef struct {
    /** Bit mask to be used for module specific parameters. The below
     *  declaration is just a place-holder for future implementation.
     */
    CSL_BitMask16 flags;
} CSL_TmrParam;

/** @brief This structure contains the base-address information for the
 *         peripheral instance
 */
typedef struct {
    /** Base-address of the configuration registers of the peripheral
     */
    CSL_TmrRegsOvly regs;
} CSL_TmrBaseAddress;

/**
 * @brief  Watchdog timer object structure.
 */
typedef struct {
    /** Pointer to the register overlay structure of the GP timer */
    CSL_TmrRegsOvly regs;

    /** Instance of GP timer being referred by this object  */
    CSL_InstNum tmrNum;
} CSL_TmrObj;


/**
 * @brief This data type is used to return the handle to the CSL of the GP timer
 */
typedef CSL_TmrObj *CSL_TmrHandle;

/**
@}
*/

/**
@addtogroup CSL_TIMER_SYMBOL
@{
*/

/** @brief Default hardware setup parameters */
#define CSL_TMR_HWSETUP_DEFAULTS { \
    CSL_TMR_PRDLO_RESETVAL, \
    CSL_TMR_PRDHI_RESETVAL, \
    CSL_TMR_CNTLO_RESETVAL, \
    CSL_TMR_CNTHI_RESETVAL, \
    (CSL_TmrIpGate)CSL_TMR_TCR_TIEN_HI_RESETVAL, \
    (CSL_TmrClksrc)CSL_TMR_TCR_CLKSRC_HI_RESETVAL, \
    (CSL_TmrPulseWidth)CSL_TMR_TCR_PWID_HI_RESETVAL, \
    (CSL_TmrClockPulse)CSL_TMR_TCR_CP_HI_RESETVAL, \
    (CSL_TmrInvInp)CSL_TMR_TCR_INVINP_HI_RESETVAL, \
    (CSL_TmrInvOutp)CSL_TMR_TCR_INVOUTP_HI_RESETVAL, \
    (CSL_TmrIpGate)CSL_TMR_TCR_TIEN_LO_RESETVAL, \
    (CSL_TmrClksrc)CSL_TMR_TCR_CLKSRC_LO_RESETVAL, \
    (CSL_TmrPulseWidth)CSL_TMR_TCR_PWID_LO_RESETVAL, \
    (CSL_TmrClockPulse)CSL_TMR_TCR_CP_LO_RESETVAL, \
    (CSL_TmrInvInp)CSL_TMR_TCR_INVINP_LO_RESETVAL, \
    (CSL_TmrInvOutp)CSL_TMR_TCR_INVOUTP_LO_RESETVAL, \
    CSL_TMR_TGCR_PSCHI_RESETVAL, \
    (CSL_TmrMode)CSL_TMR_TGCR_TIMMODE_RESETVAL \
}

/** @brief Default values for config structure */
#define CSL_TMR_CONFIG_DEFAULTS { \
    CSL_TMR_CNTLO_RESETVAL, \
    CSL_TMR_CNTHI_RESETVAL, \
    CSL_TMR_PRDLO_RESETVAL, \
    CSL_TMR_PRDHI_RESETVAL, \
    CSL_TMR_TCR_RESETVAL, \
    CSL_TMR_TGCR_RESETVAL, \
    CSL_TMR_WDTCR_RESETVAL \
}

/**
@}
*/

/** @addtogroup CSL_TIMER_FUNCTION
 @{ */

/*******************************************************************************
 * Timer global function declarations
 ******************************************************************************/

extern CSL_Status CSL_tmrInit (
    CSL_TmrContext *pContext
);

extern CSL_TmrHandle CSL_tmrOpen (
    CSL_TmrObj   *tmrObj,
    CSL_InstNum  tmrNum,
    CSL_TmrParam *pTmrParam,
    CSL_Status   *status
);

extern CSL_Status CSL_tmrGetBaseAddress (
    CSL_InstNum        tmrNum,
    CSL_TmrParam       *pTmrParam,
    CSL_TmrBaseAddress *pBaseAddress
);

extern CSL_Status CSL_tmrClose (
    CSL_TmrHandle hTmr
);

extern CSL_Status CSL_tmrHwSetup (
    CSL_TmrHandle  hTmr,
    CSL_TmrHwSetup *hwSetup
);

extern CSL_Status CSL_tmrHwControl(
    CSL_TmrHandle       hTmr,
    CSL_TmrHwControlCmd cmd,
    void                *cmdArg
);

extern CSL_Status CSL_tmrGetHwStatus (
    CSL_TmrHandle        hTmr,
    CSL_TmrHwStatusQuery query,
    void                 *response
);

extern CSL_Status CSL_tmrHwSetupRaw (
    CSL_TmrHandle hTmr,
    CSL_TmrConfig *config
);

extern CSL_Status CSL_tmrGetHwSetup (
    CSL_TmrHandle  hTmr,
    CSL_TmrHwSetup *hwSetup
);

/**
@}
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CSL_TMR_H_ */

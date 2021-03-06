
#ifndef _CSLR_CRC_HDR_H_
#define _CSLR_CRC_HDR_H_

#include "cslr.h"


/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* Word 0 */
#define CSL_CRC_HDR_WORD0_LOCAL_HDR_LEN_MASK (0x000000FFu)
#define CSL_CRC_HDR_WORD0_LOCAL_HDR_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD0_LOCAL_HDR_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD0_MOD_ID_MASK    (0x00000F00u)
#define CSL_CRC_HDR_WORD0_MOD_ID_SHIFT   (0x00000008u)
#define CSL_CRC_HDR_WORD0_MOD_ID_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD0_FILLER_BITS_MASK (0x00ff0000u)
#define CSL_CRC_HDR_WORD0_FILLER_BITS_SHIFT (0x00000010u)
#define CSL_CRC_HDR_WORD0_FILLER_BITS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD0_LTE_ORDER_MASK (0x01000000u)
#define CSL_CRC_HDR_WORD0_LTE_ORDER_SHIFT (0x00000018u)
#define CSL_CRC_HDR_WORD0_LTE_ORDER_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD0_DTX_FORMAT_MASK (0x02000000u)
#define CSL_CRC_HDR_WORD0_DTX_FORMAT_SHIFT (0x00000019u)
#define CSL_CRC_HDR_WORD0_DTX_FORMAT_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD0_RESETVAL       (0x00000000u)

/* Word 1 */
#define CSL_CRC_HDR_WORD1_NUM_SCRAMBLE_SYS_MASK (0xFFFFFFFFu)
#define CSL_CRC_HDR_WORD1_NUM_SCRAMBLE_SYS_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD1_NUM_SCRAMBLE_SYS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD1_RESETVAL       (0x00000000u)

/* Word 2 */
#define CSL_CRC_HDR_WORD2_METHOD2_ID_MASK (0xFFFFFFFFu)
#define CSL_CRC_HDR_WORD2_METHOD2_ID_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD2_METHOD2_ID_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD2_RESETVAL       (0x00000000u)

/* Word 3 */
#define CSL_CRC_HDR_WORD3_VA_BLK_LEN_MASK (0x0003FFFFu)
#define CSL_CRC_HDR_WORD3_VA_BLK_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD3_VA_BLK_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD3_VA_CRC_MASK    (0x00700000u)
#define CSL_CRC_HDR_WORD3_VA_CRC_SHIFT   (0x00000014u)
#define CSL_CRC_HDR_WORD3_VA_CRC_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD3_VA_BLKS_MASK   (0xFF000000u)
#define CSL_CRC_HDR_WORD3_VA_BLKS_SHIFT  (0x00000018u)
#define CSL_CRC_HDR_WORD3_VA_BLKS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD3_RESETVAL       (0x00000000u)

/* Word 4 */
#define CSL_CRC_HDR_WORD4_VB_BLK_LEN_MASK (0x0003FFFFu)
#define CSL_CRC_HDR_WORD4_VB_BLK_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD4_VB_BLK_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD4_VB_CRC_MASK    (0x00700000u)
#define CSL_CRC_HDR_WORD4_VB_CRC_SHIFT   (0x00000014u)
#define CSL_CRC_HDR_WORD4_VB_CRC_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD4_VB_BLKS_MASK   (0xFF000000u)
#define CSL_CRC_HDR_WORD4_VB_BLKS_SHIFT  (0x00000018u)
#define CSL_CRC_HDR_WORD4_VB_BLKS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD4_RESETVAL       (0x00000000u)

/* Word 5 */
#define CSL_CRC_HDR_WORD5_VC_BLK_LEN_MASK (0x0003FFFFu)
#define CSL_CRC_HDR_WORD5_VC_BLK_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD5_VC_BLK_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD5_VC_CRC_MASK    (0x00700000u)
#define CSL_CRC_HDR_WORD5_VC_CRC_SHIFT   (0x00000014u)
#define CSL_CRC_HDR_WORD5_VC_CRC_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD5_VC_BLKS_MASK   (0xFF000000u)
#define CSL_CRC_HDR_WORD5_VC_BLKS_SHIFT  (0x00000018u)
#define CSL_CRC_HDR_WORD5_VC_BLKS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD5_RESETVAL       (0x00000000u)

/* Word 6 */
#define CSL_CRC_HDR_WORD6_D1_BLK_LEN_MASK (0x0003FFFFu)
#define CSL_CRC_HDR_WORD6_D1_BLK_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD6_D1_BLK_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD6_D1_CRC_MASK    (0x00700000u)
#define CSL_CRC_HDR_WORD6_D1_CRC_SHIFT   (0x00000014u)
#define CSL_CRC_HDR_WORD6_D1_CRC_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD6_D1_BLKS_MASK   (0xFF000000u)
#define CSL_CRC_HDR_WORD6_D1_BLKS_SHIFT  (0x00000018u)
#define CSL_CRC_HDR_WORD6_D1_BLKS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD6_RESETVAL       (0x00000000u)

/* Word 7 */
#define CSL_CRC_HDR_WORD7_D2_BLK_LEN_MASK (0x0003FFFFu)
#define CSL_CRC_HDR_WORD7_D2_BLK_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD7_D2_BLK_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD7_D2_CRC_MASK    (0x00700000u)
#define CSL_CRC_HDR_WORD7_D2_CRC_SHIFT   (0x00000014u)
#define CSL_CRC_HDR_WORD7_D2_CRC_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD7_D2_BLKS_MASK   (0xFF000000u)
#define CSL_CRC_HDR_WORD7_D2_BLKS_SHIFT  (0x00000018u)
#define CSL_CRC_HDR_WORD7_D2_BLKS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD7_RESETVAL       (0x00000000u)

/* Word 8 */
#define CSL_CRC_HDR_WORD8_DC_BLK_LEN_MASK (0x0003FFFFu)
#define CSL_CRC_HDR_WORD8_DC_BLK_LEN_SHIFT (0x00000000u)
#define CSL_CRC_HDR_WORD8_DC_BLK_LEN_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD8_DC_CRC_MASK    (0x00700000u)
#define CSL_CRC_HDR_WORD8_DC_CRC_SHIFT   (0x00000014u)
#define CSL_CRC_HDR_WORD8_DC_CRC_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD8_DC_BLKS_MASK   (0xFF000000u)
#define CSL_CRC_HDR_WORD8_DC_BLKS_SHIFT  (0x00000018u)
#define CSL_CRC_HDR_WORD8_DC_BLKS_RESETVAL (0x00000000u)

#define CSL_CRC_HDR_WORD8_RESETVAL       (0x00000000u)

#endif

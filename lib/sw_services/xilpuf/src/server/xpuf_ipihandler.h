/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpuf_ipihandler.h
* @addtogroup xpuf_apis XilPuf Versal APIs
* @{
* @cond xpuf_internal
* This file contains the xilpuf IPI handler declaration.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  kpt  01/04/2022 Initial release
*
* </pre>
*
* @note
* @endcond
*
******************************************************************************/

#ifndef XPUF_IPIHANDLER_H_
#define XPUF_IPIHANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xplmi_config.h"
#ifdef PLM_PUF
#include "xplmi_cmd.h"

/************************** Constant Definitions *****************************/
int XPuf_IpiHandler(XPlmi_Cmd *Cmd);

#endif /* PLM_PUF */

#ifdef __cplusplus
}
#endif

#endif /* XPUF_IPIHANDLER_H_ */
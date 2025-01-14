/******************************************************************************
* Copyright (c) 2021 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xsem_client_api.c
*
* @addtogroup xsem_client_apis XilSEM Versal Client APIs
* @{
* This file provides XilSEM client interface to send IPI requests from the
* user application to XilSEM server on PLM. This provides APIs to Init,
* Start, Stop, Error Injection, Event notification registration, get Status
* details for both CRAM and NPI.
*
* @cond xsem_internal
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who  Date         Changes
* ----  ---  ----------   --------------------------------------------------
* 0.1   gm   01/04/2021   Initial creation : Cram IPI commands
* 0.2   hb   01/06/2021   Added Npi IPI commands
* 0.3   rb   01/18/2021   Added Npi PMC RAM status read API and corrected
*                         goto label
* 0.4   rb   01/25/2021   API for event registration to SEM Error Manager
* 0.5   rb   03/04/2021   Get NPI Error Information from PMC RAM
* 0.6   hv   03/11/2021   Doxygen changes
* 0.7   hb   03/15/2021   MISRA fixes and formatted code
* 0.8   rb   04/07/2021   Doxygen changes
* 0.9	hv   05/04/2021   Updated Doxygen comments
* 1.0	hv   10/08/2021   Added user interface to Get SEM configuration
* 1.1   hb   12/15/2021   Fixed compilation errors when prints are enabled
* 1.2	hv   01/06/2022   Replaced library specific utility functions and
* 			  standard lib functions with Xilinx maintained
* 			  functions
* 1.3   hb   01/07/2022   Added user interface to get golden SHA and descriptor
*                         information
* 1.4	hv   01/11/2022   Added interface for reading Frame ECC
* 1.5   rama 01/14/2022   Added user interface to get golden CRC & total frames
* 1.6   hb   02/07/2022   Updated API information for Doxygen
* 1.7   hb   03/07/2022   Fixed MISRA-C violations and added comments
* </pre>
*
* @note
* @endcond
*
******************************************************************************/
#include "xsem_client_api.h"

/****************************************************************************/
/**
 * @brief	This function is used to initialize CRAM scan from user
 *		 application.
 *		Primarily this function sends an IPI request to PLM to start CRAM
 *		Scan Initialization, waits for PLM to process the request and reads
 *		 the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of CRAM Initialization(0x10301)
 *		- Resp->RespMsg2: Status of CRAM Initialization
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On CRAM Initialization failure
 *		- XST_SUCCESS: On CRAM Initialization success
 *****************************************************************************/
XStatus XSem_CmdCfrInit(XIpiPsu *IpiInst, XSemIpiResp *Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: Resp is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_ID_CFR_INIT);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}

	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to start CRAM scan from user application.
 *		Primarily this function sends an IPI request to PLM to invoke SEM
 *		CRAM StartScan, waits for PLM to process the request and reads
 *		the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of CRAM start scan(0x10302)
 *		- Resp->RespMsg2: Status of CRAM start scan
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On CRAM start scan failure
 *		- XST_SUCCESS: On CRAM start scan success
 *****************************************************************************/
XStatus XSem_CmdCfrStartScan(XIpiPsu *IpiInst, XSemIpiResp *Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: Resp is NULL\n\r", __func__);
		goto END;
	}

	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_ID_CFR_START_SCAN);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to stop CRAM scan from user application.
 *		Primarily this function sends an IPI request to PLM to invoke SEM
 *		CRAM StopScan, waits for PLM to process the request and reads
 *		the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of CRAM stop scan(0x10303)
 *		- Resp->RespMsg2: Status of CRAM stop scan
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On CRAM stop scan failure
 *		- XST_SUCCESS: On CRAM stop scan success
 *****************************************************************************/
XStatus XSem_CmdCfrStopScan(XIpiPsu *IpiInst, XSemIpiResp *Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: Resp is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_ID_CFR_STOP_SCAN);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status){
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status){
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to inject an error at a valid location in
 *		CRAM from user application.
 *		Primarily this function sends an IPI request to PLM to perform
 *		error injection in CRAM with user provided arguments in
 *		*ErrDetail, waits for PLM to process the request and reads
 *		the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[in]	ErrDetail	Structure Pointer with Error Injection details
 *		- ErrDetail->Row   : Row Number
 *		(Min: 0 , Max: (value at CFU_ROW_RANGE)-1)
 *		- ErrDetail->Efar  : Frame Address
 *					- Frame Number [0:19] (Refer note)
 *					- Block Type [20:22]
 *		- ErrDetail->Qword : Quad Word(Min: 0, Max: 24)
 *		- ErrDetail->Bit   : Bit Position(Min: 0, Max: 127)
 * @param[out]	Resp	Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of CRAM error injection(0x10304)
 *		- Resp->RespMsg2: Status of CRAM error injection
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On CRAM error injection failure
 *		- XST_SUCCESS: On CRAM error injection success
 * @note
 *	- Total number of frames in a row is not same for all rows.
 *
 *	- XSem_CmdCfrGetTotalFrames API is provided to know the total number of
 * frames in a row for each block. Output param (FrameCntPtr) of
 * XSem_CmdCfrGetTotalFrames API is updated with total number of frames of each
 * block type for the input row. If a particular block in a row has 0 frames,
 * then error injection shall not be performed. Range of Frame number: 0 to
 * (FrameCntPtr[n] - 1) where n is block type with range 0 to 6.
 *
 *	- The safe location to perform error injection is QWORD 12 which has
 * ECC bits. The error injection will not change the design behaviour.
 *****************************************************************************/
XStatus XSem_CmdCfrNjctErr (XIpiPsu *IpiInst, \
			XSemCfrErrInjData *ErrDetail, \
			XSemIpiResp *Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate ErrDetail structure pointer */
	if (NULL == ErrDetail) {
		XSem_Dbg("[%s] ERROR: ErrDetailStruct is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: Resp is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD5(Payload, CMD_ID_CFR_NJCT_ERR, ErrDetail->Efar,
			ErrDetail->Qword, ErrDetail->Bit, ErrDetail->Row);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status){
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status){
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to read all CRAM Status registers from
 *		PMC RAM and send to user application
 *
 * @param[out]	CfrStatusInfo Structure Pointer with CRAM Status details
 *		- CfrStatusInfo->Status: Provides details about CRAM scan
 *			- Bit [31-25]: Reserved
 *			- Bit [24:20]: CRAM Error codes
 *				- 00001: Unexpected CRC error when CRAM is
 *				not in observation state
 *				- 00010: Unexpected ECC error when CRAM is
 *				not in Observation or Initialization state
 *				- 00011: Safety write error in SEU handler
 *				- 00100: ECC/CRC ISR not found in any Row
 *				- 00101: CRAM Initialization is not done
 *				- 00110: CRAM Start Scan failure
 *				- 00111: CRAM Stop Scan failure
 *				- 01000: Invalid Row for Error Injection
 *				- 01001: Invalid QWord for Error Injection
 *				- 01010: Invalid Bit for Error Injection
 *				- 01011: Invalid Frame Address for Error
 *				Injection
 *				- 01100: Unexpected Bit flip during Error
 *				Injection
 *				- 01101: Masked Bit during Injection
 *				- 01110: Invalid Block Type for Error
 *				Injection
 *				- 01111: CRC or Uncorrectable Error or
 * 				correctable error(when correction is disabled)
 *				is active in CRAM
 *				- 10000: ECC or CRC Error detected during
 *				CRAM Calibration in case of SWECC
 *			- Bit [19-18]: Reserved
 *			- Bit [17]: CRAM scan is disabled in design
 *			- Bit [16]: CRAM Initialization is completed
 *			- Bit [15-14]: CRAM Correctable ECC error status
 *				- 00: No Correctable error encountered
 *				- 01: Correctable error detected and
 *				corrected
 *				- 10: Correctable error detected but not
 *				corrected (Correction is disabled)
 *				- 11: Reserved
 *			- Bit [13]: CRAM Scan internal error
 *			- Bit [12]: CRAM Invalid Error Location detected
 *			- Bit [11]: CRAM Correctable ECC error detected
 *			- Bit [10]: CRAM CRC error detected
 *			- Bit [09]: CRAM Uncorrectable ECC error detected
 *			- Bit [08]: CRAM Start-up test failure
 *			- Bit [07]: CRAM Calibration Timeout error
 *			- Bit [06]: CRAM Fatal/Error State
 *			- Bit [05]: CRAM Error Injection State
 *			- Bit [04]: CRAM Idle State
 *			- Bit [03]: CRAM Correction State
 *			- Bit [02]: CRAM Observation State
 *			- Bit [01]: CRAM Initialization State
 *			- Bit [00]: CRAM Scan is included in design
 *		- CfrStatusInfo->ErrAddrL: This stores the low address of
 *		the last 7 corrected error details if correction is enabled
 *		in design.
 *			- Bit [31:28]: Reserved
 *			- Bit [27:23]: QWord location where error was detected
 *			- Bit [22:16]: Bit location where error was detected
 *			- Bit [15:2]: Reserved
 *			- Bit [1:0]: Define validity of error address.
 *				- 00: Info not available
 *				- 01: Address out of range
 *				- 10: Reserved
 *				- 11: Address valid
 *		- CfrStatusInfo->ErrAddrH: This stores the high address of
 *		the last 7 corrected error details if correction is enabled
 *		in design.
 *			- Bits[31:27]: Reserved
 *			- Bits[26:23]: Row number where error was detected
 *			- Bits[22:20]: Block type of the frame
 *			- Bits[19:0]: Frame address where error was detected
 *		- CfrStatusInfo->ErrCorCnt: Counter value of Correctable
 *		Error Bits
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: If NULL pointer reference of CfrStatusInfo
 *		- XST_SUCCESS: On successful read from PMC RAM
 *****************************************************************************/
XStatus XSem_CmdCfrGetStatus(XSemCfrStatus *CfrStatusInfo)
{
	XStatus Status = XST_FAILURE;
	u32 Index = 0U;

	/* Validate CRAM scan status info structure pointer */
	if (NULL == CfrStatusInfo) {
		XSem_Dbg("[%s] ERROR: CfrStatusInfo is NULL\n\r", __func__);
		goto END;
	}
	/* Get CRAM scan status */
	CfrStatusInfo->Status = Xil_In32(PMC_RAM_SEM_CRAM_STATUS);
	/* Get CRAM scan corrected bit count */
	CfrStatusInfo->ErrCorCnt = Xil_In32(PMC_RAM_SEM_CRAM_COR_BITCNT);

	/* Get CRAM scan error addresses */
	for (Index = 0U; Index < MAX_CRAMERR_REGISTER_CNT; Index++) {
		CfrStatusInfo->ErrAddrL[Index] = \
			Xil_In32((PMC_RAM_SEM_CRAMERR_ADDRL0 + (Index * 8U)));
		CfrStatusInfo->ErrAddrH[Index] = \
			Xil_In32((PMC_RAM_SEM_CRAMERR_ADDRH0 + (Index * 8U)));
	}

	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to Read frame ECC of a particular Frame.
 *		Primarily this function sends an IPI request to PLM to invoke SEM
 *		CRAM SendFrameEcc, waits for PLM to process the request and reads
 *		the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[in]	CframeAddr	Frame Address
 *					- Frame Number [0:19] (Refer note)
 *					- Block Type [20:22]
 * @param[in]	RowLoc		Row index(Min: 0 , Max: CFU_ROW_RANGE -1)
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of CRAM Send Frame ECC(0x3030A)
 *		- Resp->RespMsg2: Segment 0 ECC value
 *		- Resp->RespMsg3: Segment 1 ECC value
 *		- Resp->RespMsg4: Status of CRAM stop scan
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On CRAM Read Frame ECC failure
 *		- XST_SUCCESS: On CRAM Read Frame ECC success
 *
 * @note
 *	- Total number of frames in a row is not same for all rows.
 *
 *	- XSem_CmdCfrGetTotalFrames API is provided to know the total number of
 * frames in a row for each block. Output param (FrameCntPtr) of
 * XSem_CmdCfrGetTotalFrames API is updated with total number of frames of each
 * block type for the input row. If a particular block in a row has 0 frames,
 * then error injection shall not be performed. Range of Frame number: 0 to
 * (FrameCntPtr[n] - 1) where n is block type with range 0 to 6.
 *
 *	- The safe location to perform error injection is QWORD 12 which has
 * ECC bits. The error injection will not change the design behaviour.
 *****************************************************************************/
XStatus XSem_CmdCfrReadFrameEcc(XIpiPsu *IpiInst, u32 CframeAddr, u32 RowLoc,
		 XSemIpiResp *Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance structure pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: Resp is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD3(Payload, CMD_ID_CFR_RDFRAME_ECC, CframeAddr, RowLoc);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status){
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status){
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];
	Resp->RespMsg3 = Response[3U];
	Resp->RespMsg4 = Response[4U];

	/* Print API status */
	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to start NPI scan from user application.
 *		Primarily this function sends an IPI request to PLM to invoke
 *		SEM NPI StartScan, waits for PLM to process the request and
 *		reads the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of NPI start scan(0x10305)
 *		- Resp->RespMsg2: Status of NPI start scan
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On NPI start scan failure
 *		- XST_SUCCESS: On NPI start scan success
 *****************************************************************************/
XStatus XSem_CmdNpiStartScan (XIpiPsu *IpiInst, XSemIpiResp * Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: RespStruct is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_NPI_STARTSCAN);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with"
		"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with"
		"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to stop NPI scan from user application.
 *		Primarily this function sends an IPI request to PLM to invoke
 *		SEM NPI StopScan, waits for PLM to process the request and
 *		reads the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of NPI stop scan(0x10306)
 *		- Resp->RespMsg2: Status of NPI stop scan
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On NPI stop scan failure
 *		- XST_SUCCESS: On NPI stop scan success
 *****************************************************************************/
XStatus XSem_CmdNpiStopScan (XIpiPsu *IpiInst, XSemIpiResp * Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: RespStruct is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_NPI_STOPSCAN);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with"
		"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with"
		"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to inject SHA error in NPI descriptor list
 *		(in the first NPI descriptor) from user application.
 *		Primarily this function sends an IPI request to PLM to invoke
 *		SEM NPI ErrorInject, waits for PLM to process the request and
 *		reads the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of NPI error injection(0x10307)
 *		- Resp->RespMsg2: Status of NPI error injection
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On NPI error injection failure
 *		- XST_SUCCESS: On NPI error injection success
 *
 * @note	The caller shall invoke this XSem_CmdNpiInjectError function
 *		again to correct the injected error in NPI descriptor.
 *****************************************************************************/
XStatus XSem_CmdNpiInjectError (XIpiPsu *IpiInst, XSemIpiResp * Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: RespStruct is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_NPI_ERRINJECT);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with"
		" ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with"
		" ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to get golden SHA
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of NPI get golden SHA(0x10310)
 *		- Resp->RespMsg2: Status of NPI get golden SHA
 * @param[out]  DescData	Structure pointer to hold total descriptor count,
 *							golden SHA and information related to descriptors
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On NPI golden SHA retrieve failure
 *		- XST_SUCCESS: On NPI golden SHA retrieve success
 *
 *****************************************************************************/
XStatus XSem_CmdNpiGetGldnSha (XIpiPsu *IpiInst, XSemIpiResp * Resp,
		XSem_DescriptorData * DescData)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI response structure */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: RespStruct is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD2(Payload, CMD_NPI_GET_GLDN_SHA, DescData);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with"
		" ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with"
		" ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];

	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to read all NPI Status registers from
 *		PMC RAM and send to user application
 *
 * @param[out]	NpiStatusInfo Structure Pointer with NPI Status details
 *		- NpiStatusInfo->Status: Provides details about NPI scan
 *			- Bit [31]: Cryptographic acceleration blocks are disabled for
 *			export compliance
 *			- Bit [30]: Periodic scan missed
 *			- Bit [29]: Pulse Check failed. NPI scan skipped descriptors
 *			- Bit [28]: Execution time exceeded. NPI scan exceeded 20ms
 *			budget time
 *			- Bit [27-26]: Reserved
 *			- Bit [25]: NPI GPIO write failure
 *			- Bit [24]: NPI SHA engine failure
 *			- Bit [23]: NPI Safety register write failure
 *			- Bit [22]: NPI GT slave arbitration failure
 *			- Bit [21]: NPI DDRMC Main slave arbitration failure
 *			- Bit [20]: NPI Slave Address is not valid
 *			- Bit [19]: NPI Descriptor SHA header is not valid
 *			- Bit [18]: NPI Descriptor header is not valid
 *			- Bit [17]: NPI SHA mismatch error is detected during
 *			scan
 *			- Bit [16]: NPI SHA mismatch error is detected in
 *			first scan
 *			- Bit [15-12]: Reserved
 *			- Bit [11]: NPI periodic scan is enabled
 *			- Bit [10]: NPI scan is suspended
 *			- Bit [09]: NPI completed the first scan
 *			- Bit [08]: NPI Scan is disabled in design
 *			- Bit [07-06]: Reserved
 *			- Bit [05]: NPI Internal Error State
 *			- Bit [04]: NPI SHA mismatch Error State
 *			- Bit [03]: NPI SHA Error Injection State
 *			- Bit [02]: NPI Scan State
 *			- Bit [01]: NPI Initialization State
 *			- Bit [00]: NPI Idle State
 *		- NpiStatusInfo->SlvSkipCnt: Provides NPI descriptor slave skip
 *		counter value if arbitration failure. This is 8 words result
 *		to accommodate 32 1-Byte skip counters for individual slaves
 *		arbitration failures. Slaves can be DDRMC Main, GT for which
 *		arbitration is required before performing scanning.
 *		- NpiStatusInfo->ScanCnt: NPI scan counter value. This
 *		counter represents number of periodic scan cycle completion.
 *		- NpiStatusInfo->HbCnt: NPI heartbeat counter value. This
 *		counter represents number of scanned descriptor slaves.
 *		- NpiStatusInfo->ErrInfo: NPI scan error information if SHA
 *		mismatch is detected. This is 2 word information.
 *			- Word 0: Node ID of descriptor for which SHA
 *			mismatch is detected
 *			- Word 1 Bit [15-8]: NPI descriptor index number
 *			- Word 1 Bit [7-0]: NPI Slave Skip count Index
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: If NULL pointer reference of NpiStatusInfo
 *		- XST_SUCCESS: On successful read from PMC RAM
 *****************************************************************************/
XStatus XSem_CmdNpiGetStatus(XSemNpiStatus *NpiStatusInfo)
{
	XStatus Status = XST_FAILURE;
	u32 Index = 0U;

	/* Validate NPI status Info structure pointer */
	if (NULL == NpiStatusInfo) {
		XSem_Dbg("[%s] ERROR: NpiStatusInfo is NULL\n\r", __func__);
		goto END;
	}

	/* Get NPI status */
	NpiStatusInfo->Status = Xil_In32(PMC_RAM_SEM_NPI_STATUS);
	/* Get NPI scan count */
	NpiStatusInfo->ScanCnt = Xil_In32(PMC_RAM_SEM_NPI_SCAN_CNT);
	/* Get NPI scan heart beat count */
	NpiStatusInfo->HbCnt = Xil_In32(PMC_RAM_SEM_NPI_HEARTBEAT_CNT);

	/* Get NPI scan slave skip count */
	for (Index = 0U; Index < MAX_NPI_SLV_SKIP_CNT; Index++) {
		NpiStatusInfo->SlvSkipCnt[Index] = \
			Xil_In32(PMC_RAM_SEM_NPI_SLVSKIP_CNT0 + (Index * 4U));
	}
	/* Get NPI scan error info */
	for (Index = 0U; Index < MAX_NPI_ERR_INFO_CNT; Index++) {
		NpiStatusInfo->ErrInfo[Index] = \
			Xil_In32(PMC_RAM_SEM_NPIERR_INFO0 + (Index * 4U));
	}

	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to register/un-register event
 *		notification with XilSEM Server.
 *		Primarily this function sends an IPI request to PLM to invoke
 *		SEM Event Notifier registration, waits for PLM to process the
 *		request and check the status.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[in]	Notifier	Pointer of the notifier object to be associated
 * 		with the requested notification
 *		- Notifier->Module: The SEM module from which notification is required
 *		- Notifier->Event: Event(s) belonging to the Module for which
 *			notifications are required
 *		- Notifier->Flag: Flags to enable or disable notifications
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On event registration/un-registration failure
 *		- XST_SUCCESS: On event registration/un-registration success
 *
 * @note	The caller shall initialize the notifier object before invoking
 *		the XSem_RegisterEvent function.
 *
 *****************************************************************************/
XStatus XSem_RegisterEvent(XIpiPsu *IpiInst, XSem_Notifier* Notifier)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance structure pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}

	/* Validate Notifier structure pointer */
	if (NULL == Notifier) {
		XSem_Dbg("[%s] ERROR: Notifier is NULL\n\r", __func__);
		goto END;
	}

	/* Flag will decide whether to enable/disable event */
	PACK_PAYLOAD4(Payload, CMD_EM_EVENT_REGISTER, Notifier->Module,
			Notifier->Event, Notifier->Flag);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with "\
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with "\
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}

	/* Print API status */
	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function is used to read CRAM & NPI configuration.
 *		Primarily this function sends an IPI request to PLM to invoke SEM
 *		Get configuration command, waits for PLM to process the request and
 *		reads the response message.
 *
 * @param[in]	IpiInst		Pointer to IPI driver instance
 * @param[out]	Resp		Structure Pointer of IPI response
 *		- Resp->RespMsg1: Acknowledgment ID of Get Configuration(0x30309)
 *
 *		- Resp->RespMsg2: CRAM Attribute register details
 *			- Bit [31:16]: Not Implemented
 *			- Bit [15:9]: Reserved
 *			- Bit [8]: Reserved
 *			- Bit [7]: Reserved
 *			- Bit [6:5]: Indicates when to start CRAM scan
 *				- 00: Do not automatically start scan
 *				- 01: Enable scan automatically after
 *						device configuration.
 *				- 10: Reserved
 *				- 11: Reserved
 *			- Bit [4]: Reserved
 *			- Bit [3]: Indicates HwECC/SwECC
 *				- 0: Uses hardware calculated ECC.
 *				- 1: Uses software calculated ECC that comes from tools
 *						and part of CDO
 *			- Bit [2]: Indicates Correctable error is to be corrected/not
 *				- 0: Disables error correction capability
 *				- 1: Enables error correction capability
 *			- Bit [1:0]: Define the mode of the scan (Enable/Disable scan)
 *				- 00: Disable Configuration RAM scan
 *				- 01: RESERVED
 *				- 10: Enable Configuration RAM scan
 *				- 11: RESERVED
 *
 *		- Resp->RespMsg3: NPI Attribute register details
 *			- Bit [31:24]: Not implemented
 *			- Bit [23:18]: Reserved
 *			- Bit [17:8]: The scheduled time in milliseconds that the
 *							NPI scan will be periodically performed.
 *							Default Setting: 0x064 = 100ms
 *			- Bit [7:6]: Reserved
 *			- Bit [5:4]: Indicates when to start NPI scan
 *				- 00: Do not automatically start scan
 *				- 01: Enable scan automatically after
 *					  device configuration.
 *				- 10: Reserved
 *				- 11: Reserved
 *			- Bit [3]: Reserved
 *			- Bit [2]: Indicates HwSHA/SwSHA
 *				0: Use hardware calculated SHA.
 *				1: Use software calculated SHA.
 *			- Bit [1:0]: Reserved
 *
 *		- Resp->RespMsg4: Status of Get Configuration command
 *
 * @return	This API returns the success or failure.
 *		- XST_FAILURE: On Get Configuration failure
 *		- XST_SUCCESS: On Get Configuration success
 *****************************************************************************/
XStatus XSem_CmdGetConfig(XIpiPsu *IpiInst, XSemIpiResp *Resp)
{
	XStatus Status = XST_FAILURE;
	u32 Payload[PAYLOAD_ARG_CNT] = {0U};
	u32 Response[RESPONSE_ARG_CNT] = {0U};

	/* Validate IPI instance structure pointer */
	if (NULL == IpiInst) {
		XSem_Dbg("[%s] ERROR: IpiInst is NULL\n\r", __func__);
		goto END;
	}
	/* Validate IPI Response structure pointer */
	if (NULL == Resp) {
		XSem_Dbg("[%s] ERROR: Resp is NULL\n\r", __func__);
		goto END;
	}
	/* Pack commands to be sent over IPI */
	PACK_PAYLOAD1(Payload, CMD_ID_SEM_GET_CONFIG);

	/* Send request to PLM with the payload */
	Status = XSem_IpiSendReqPlm(IpiInst, Payload);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiSendReqPlm failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Check PLM response */
	Status = XSem_IpiPlmRespMsg(IpiInst, Response);
	if (XST_SUCCESS != Status) {
		XSem_Dbg("[%s] ERROR: XSem_IpiPlmRespMsg failed with " \
				"ErrCode 0x%x\n\r", __func__, Status);
		goto END;
	}
	/* Copy response messages */
	Resp->RespMsg1 = Response[1U];
	Resp->RespMsg2 = Response[2U];
	Resp->RespMsg3 = Response[3U];
	Resp->RespMsg4 = Response[4U];

	/* Print API status */
	XSem_Dbg("[%s] SUCCESS: 0x%x\n\r", __func__, Status);

END:
	return Status;
}


/*****************************************************************************/
/**
 * @brief	This function is used to read CFRAME golden CRC for a row
 *
 * @param[in]	RowIndex	Row index for which CRC to be read
 *							(Min: 0 , Max: CFU_ROW_RANGE -1)
 *
 * @return	This API returns the Golden CRC for a given Row.
 *
 * @note
 * - Total number of rows is not same for all platforms.
 * - The number maximum rows (CFU_ROW_RANGE) can be obtained by reading the
 * address CFU_ROW_RANGE(0XF12B006C).
 *****************************************************************************/
u32 XSem_CmdCfrGetCrc(u32 RowIndex)
{
	u32 GoldenCrc = 0U;

	/**
	 * SEU Golden Hardware CRC from first scan is stored in Bits [95:64] of
	 * CFRAME_SEU_CRC_ADDR (128-Bit) register
	 */
	/** Read CRC register word 0 */
	(void)Xil_In32(CFRAME_BASE_ADDRESS +
			(RowIndex * CFRAME_ROW_OFFSET) + CFRAME_SEU_CRC_ADDR);
	/** Read CRC register word 1 */
	(void)Xil_In32(CFRAME_BASE_ADDRESS +
			(RowIndex * CFRAME_ROW_OFFSET) + CFRAME_SEU_CRC_ADDR + 0x04U);
	/** Read CRC register word 2 and store it as GoldenCrc */
	GoldenCrc = Xil_In32(CFRAME_BASE_ADDRESS +
			(RowIndex * CFRAME_ROW_OFFSET) +
			CFRAME_SEU_CRC_ADDR + 0x08U);
	/** Read CRC register word 3 */
	(void)Xil_In32(CFRAME_BASE_ADDRESS +
			(RowIndex * CFRAME_ROW_OFFSET) + CFRAME_SEU_CRC_ADDR + 0x0CU);

	/** Return GoldenCrc */
	return GoldenCrc;
}
/*****************************************************************************/
/**
 * @brief	This function is used to read total frames in a row
 *
 * @param[in]	RowIndex	 Row index for which total number of frames is to
 *								be read
 *                           (Min: 0 , Max: CFU_ROW_RANGE -1)
 * @param[out]	FrameCntPtr	 Pointer to store Total frames
 *		- FrameCntPtr[0] : Type_0 total frames
 *		- FrameCntPtr[1] : Type_1 total frames
 *		- FrameCntPtr[2] : Type_2 total frames
 *		- FrameCntPtr[3] : Type_3 total frames
 *		- FrameCntPtr[4] : Type_4 total frames
 *		- FrameCntPtr[5] : Type_5 total frames
 *		- FrameCntPtr[6] : Type_6 total frames
 *
 * @note
 *	- Total number of frames in a row is not same for all rows.
 *	- XSem_CmdCfrGetTotalFrames API is provided to know the total number of
 * frames in a row for each block. Output param (FrameCntPtr) of
 * XSem_CmdCfrGetTotalFrames API is updated with total number of frames of each
 * block type for the input row. If a particular block in a row has 0 frames,
 * then error injection shall not be performed. Range of Frame number: 0 to
 * (FrameCntPtr[n] - 1) where n is block type with range 0 to 6.
 *	- The safe location to perform error injection is QWORD 12 which has
 * ECC bits. The error injection will not change the design behaviour.
 *****************************************************************************/
void XSem_CmdCfrGetTotalFrames(u32 RowIndex, u32 *FrameCntPtr)
{
	u32 TypeId;
	u32 Buf[8];

	for (TypeId = 0U; TypeId < 8U; TypeId++) {
		/* Read total frames in each type */
		Buf[TypeId] = Xil_In32(CFRAME_BASE_ADDRESS + \
		                        (RowIndex * CFRAME_ROW_OFFSET) + \
								(CFRAME_LAST_BOT_ADDR + (TypeId * 4U)));
	}
	/* Get Type_0 total frames */
	FrameCntPtr[0] = Buf[0] & CFRAME_BIT_0_19_MASK;

	/* Get Type_1 total frames */
	FrameCntPtr[1] = (Buf[0] & CFRAME_BIT_20_39_MASK_LOW) >> \
							   CFRAME_BIT_20_39_SHIFT_R;
	FrameCntPtr[1] |= (Buf[1] & CFRAME_BIT_20_39_MASK_HIGH) << \
								CFRAME_BIT_20_39_SHIFT_L;

	/* Get Type_2 total frames */
	FrameCntPtr[2] = (Buf[1] & CFRAME_BIT_40_59_MASK) >> \
								CFRAME_BIT_40_59_SHIFT_R;

	/* Get Type_3 total frames */
	FrameCntPtr[3] = (Buf[1] & CFRAME_BIT_60_79_MASK_LOW) >> \
								CFRAME_BIT_60_79_SHIFT_R;
	FrameCntPtr[3] |= (Buf[2] & CFRAME_BIT_60_79_MASK_HIGH) << \
								CFRAME_BIT_60_79_SHIFT_L;

	/* Get Type_4 total frames */
	FrameCntPtr[4] = Buf[4] & CFRAME_BIT_0_19_MASK;

	/* Get Type_5 total frames */
	FrameCntPtr[5] = (Buf[4] & CFRAME_BIT_20_39_MASK_LOW) >> \
								CFRAME_BIT_20_39_SHIFT_R;
	FrameCntPtr[5] |= (Buf[5] & CFRAME_BIT_20_39_MASK_HIGH) << \
								CFRAME_BIT_20_39_SHIFT_L;

	/* Get Type_6 total frames */
	FrameCntPtr[6] = (Buf[5] & CFRAME_BIT_40_59_MASK) >> \
									CFRAME_BIT_40_59_SHIFT_R;
}
/** @} */

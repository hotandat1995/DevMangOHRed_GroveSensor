
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_LPT_COMMON_H_INCLUDE_GUARD
#define LE_LPT_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_LPT_PROTOCOL_ID "ddfabab9b938cdcb7564dbd7632f76bc"
#define IFGEN_LE_LPT_MSG_SIZE 18



//--------------------------------------------------------------------------------------------------
/**
 * eDRX Radio Access Technology enum
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_LPT_EDRX_RAT_UNKNOWN = 0,
        ///< Unknown
    LE_LPT_EDRX_RAT_EC_GSM_IOT = 1,
        ///< EC-GSM-IoT (A/Gb mode)
    LE_LPT_EDRX_RAT_GSM = 2,
        ///< GSM (A/Gb mode)
    LE_LPT_EDRX_RAT_UTRAN = 3,
        ///< UTRAN (Iu mode)
    LE_LPT_EDRX_RAT_LTE_M1 = 4,
        ///< E-UTRAN (WB-S1 mode)
    LE_LPT_EDRX_RAT_LTE_NB1 = 5,
        ///< E-UTRAN (NB-S1 mode)
    LE_LPT_EDRX_RAT_MAX = 6
        ///< Invalid
}
le_lpt_EDrxRat_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_lpt_EDrxParamsChange'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_lpt_EDrxParamsChangeHandler* le_lpt_EDrxParamsChangeHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler to report a change in the network-provided eDRX parameters.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_lpt_EDrxParamsChangeHandlerFunc_t)
(
        le_lpt_EDrxRat_t rat,
        ///< Radio Access Technology.
        le_onoff_t activation,
        ///< eDRX activation state.
        uint8_t eDrxValue,
        ///< eDRX cycle value, defined in 3GPP
        ///< TS 24.008 Rel-13 section 10.5.5.32.
        uint8_t pagingTimeWindow,
        ///< Paging Time Window, defined in 3GPP
        ///< TS 24.008 Rel-13 section 10.5.5.32.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_lpt_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_lpt_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_lpt_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the eDRX activation state for the given Radio Access Technology.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNSUPPORTED    eDRX is not supported by the platform.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_lpt_SetEDrxState
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxRat_t rat,
        ///< [IN] Radio Access Technology.
        le_onoff_t activation
        ///< [IN] eDRX activation state.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the requested eDRX cycle value for the given Radio Access Technology.
 * The eDRX cycle value is defined in 3GPP TS 24.008 Release 13 section 10.5.5.32.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNSUPPORTED    eDRX is not supported by the platform.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_lpt_SetRequestedEDrxValue
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxRat_t rat,
        ///< [IN] Radio Access Technology.
        uint8_t eDrxValue
        ///< [IN] Requested eDRX cycle value, defined in 3GPP
        ///< TS 24.008 Rel-13 section 10.5.5.32.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the requested eDRX cycle value for the given Radio Access Technology.
 * The eDRX cycle value is defined in 3GPP TS 24.008 Release 13 section 10.5.5.32.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNSUPPORTED    eDRX is not supported by the platform.
 *  - LE_UNAVAILABLE    No requested eDRX cycle value.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_lpt_GetRequestedEDrxValue
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxRat_t rat,
        ///< [IN] Radio Access Technology.
        uint8_t* eDrxValuePtr
        ///< [OUT] Requested eDRX cycle value, defined in 3GPP
        ///< TS 24.008 Rel-13 section 10.5.5.32.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the network-provided eDRX cycle value for the given Radio Access Technology.
 * The eDRX cycle value is defined in 3GPP TS 24.008 Release 13 section 10.5.5.32.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNSUPPORTED    eDRX is not supported by the platform.
 *  - LE_UNAVAILABLE    No network-provided eDRX cycle value.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_lpt_GetNetworkProvidedEDrxValue
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxRat_t rat,
        ///< [IN] Radio Access Technology.
        uint8_t* eDrxValuePtr
        ///< [OUT] Network-provided eDRX cycle value, defined in
        ///< 3GPP TS 24.008 Rel-13 section 10.5.5.32.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the network-provided Paging Time Window for the given Radio Access Technology.
 * The Paging Time Window is defined in 3GPP TS 24.008 Release 13 section 10.5.5.32.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNSUPPORTED    eDRX is not supported by the platform.
 *  - LE_UNAVAILABLE    No defined Paging Time Window.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_lpt_GetNetworkProvidedPagingTimeWindow
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxRat_t rat,
        ///< [IN] Radio Access Technology.
        uint8_t* pagingTimeWindowPtr
        ///< [OUT] Network-provided Paging Time Window, defined
        ///< in 3GPP TS 24.008 Rel-13 section 10.5.5.32.
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_lpt_EDrxParamsChange'
 *
 * Event to report a change in the network-provided eDRX parameters.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_lpt_EDrxParamsChangeHandlerRef_t ifgen_le_lpt_AddEDrxParamsChangeHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxParamsChangeHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_lpt_EDrxParamsChange'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_lpt_RemoveEDrxParamsChangeHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_lpt_EDrxParamsChangeHandlerRef_t handlerRef
        ///< [IN]
);

#endif // LE_LPT_COMMON_H_INCLUDE_GUARD
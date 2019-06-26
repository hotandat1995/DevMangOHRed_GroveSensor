

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_LPT_INTERFACE_H_INCLUDE_GUARD
#define LE_LPT_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_lpt_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_lpt_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_lpt_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_lpt_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * eDRX Radio Access Technology enum
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler to report a change in the network-provided eDRX parameters.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_lpt_EDrxParamsChange'
 */
//--------------------------------------------------------------------------------------------------



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
le_result_t le_lpt_SetEDrxState
(
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
le_result_t le_lpt_SetRequestedEDrxValue
(
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
le_result_t le_lpt_GetRequestedEDrxValue
(
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
le_result_t le_lpt_GetNetworkProvidedEDrxValue
(
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
le_result_t le_lpt_GetNetworkProvidedPagingTimeWindow
(
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
le_lpt_EDrxParamsChangeHandlerRef_t le_lpt_AddEDrxParamsChangeHandler
(
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
void le_lpt_RemoveEDrxParamsChangeHandler
(
    le_lpt_EDrxParamsChangeHandlerRef_t handlerRef
        ///< [IN]
);


#endif // LE_LPT_INTERFACE_H_INCLUDE_GUARD
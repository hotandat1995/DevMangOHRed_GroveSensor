

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ANTENNA_INTERFACE_H_INCLUDE_GUARD
#define LE_ANTENNA_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_antenna_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_antenna_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_antenna_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_antenna_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 *  Reference type for an antenna diagnostic.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Antenna type.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Antenna status
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for antenna status.
 *
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_antenna_StatusEvent'
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Requested the antenna monitoring.
 *
 * @return
 *      - Reference to the antenna object.
 *      - NULL on failure.
 */
//--------------------------------------------------------------------------------------------------
le_antenna_ObjRef_t le_antenna_Request
(
    le_antenna_Type_t antennaType
        ///< [IN] antenna to be monitored
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the antenna type.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_BAD_PARAMETER if an invalid reference provided.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_GetType
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    le_antenna_Type_t* antennaTypePtr
        ///< [OUT] allocated antenna type
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ADC value used to detect a short circuit.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_FAULT on other failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_SetShortLimit
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    uint32_t shortLimit
        ///< [IN] ADC value used to detect a short circuit
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ADC value used to detect a short circuit.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_FAULT on other failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_GetShortLimit
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    uint32_t* shortLimitPtr
        ///< [OUT] ADC value used to detect a short circuit
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ADC value used to detect an open circuit.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_FAULT on other failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_SetOpenLimit
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    uint32_t openLimit
        ///< [IN] ADC value used to detect an open circuit
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ADC value used to detect an open circuit.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_FAULT on other failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_GetOpenLimit
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    uint32_t* openLimitPtr
        ///< [OUT] ADC value used to detect an open circuit
);



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_antenna_StatusEvent'
 *
 * This event provides information on antenna status for the given antennaRef.
 *
 */
//--------------------------------------------------------------------------------------------------
le_antenna_StatusEventHandlerRef_t le_antenna_AddStatusEventHandler
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    le_antenna_StatusHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_antenna_StatusEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_antenna_RemoveStatusEventHandler
(
    le_antenna_StatusEventHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the antenna status.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_UNSUPPORTED if the antenna detection is not supported
 *      - LE_FAULT on other failure
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_GetStatus
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    le_antenna_Status_t* statusPtr
        ///< [OUT] antenna status
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the external ADC used to monitor the requested antenna.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_UNSUPPORTED request not supported
 *      - LE_FAULT on other failure
 *
 * @note The same external ADC may not be selected for both antennas at the same time.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_SetExternalAdc
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    int8_t adcId
        ///< [IN] ADC index used to monitor the requested antenna
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the external ADC used to monitor the requested antenna.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the antenna reference is unknown
 *      - LE_UNSUPPORTED request not supported
 *      - LE_FAULT on other failure
 *
 * @note If the returned ADC index is "-1", it means no external ADC are used to monitor
 * the requested antenna.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_antenna_GetExternalAdc
(
    le_antenna_ObjRef_t antennaRef,
        ///< [IN] antenna reference
    int8_t* adcIdPtr
        ///< [OUT] ADC index used to monitor the requested antenna
);


#endif // LE_ANTENNA_INTERFACE_H_INCLUDE_GUARD
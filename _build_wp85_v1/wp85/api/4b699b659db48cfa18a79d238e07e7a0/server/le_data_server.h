

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_DATA_INTERFACE_H_INCLUDE_GUARD
#define LE_DATA_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdc_server.h"
#include "le_dcs_server.h"

// Internal includes for this interface
#include "le_data_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_data_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_data_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_data_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Request function and used by Release function
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Technologies.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Time protocols.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for connection state changes
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_data_ConnectionState'
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_data_ConnectionState'
 *
 * This event provides information on connection state changes
 */
//--------------------------------------------------------------------------------------------------
le_data_ConnectionStateHandlerRef_t le_data_AddConnectionStateHandler
(
    le_data_ConnectionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_data_ConnectionState'
 */
//--------------------------------------------------------------------------------------------------
void le_data_RemoveConnectionStateHandler
(
    le_data_ConnectionStateHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Request the default data connection
 *
 * @return
 *      - Reference to the data connection (to be used later for releasing the connection)
 *      - NULL if the data connection requested could not be processed
 */
//--------------------------------------------------------------------------------------------------
le_data_RequestObjRef_t le_data_Request
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Release a previously requested data connection
 */
//--------------------------------------------------------------------------------------------------
void le_data_Release
(
    le_data_RequestObjRef_t requestRef
        ///< [IN] Reference to a previously requested data connection
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the rank of the technology used for the data connection service
 *
 * @return
 *      - @ref LE_OK if the technology is added to the list
 *      - @ref LE_BAD_PARAMETER if the technology is unknown
 *      - @ref LE_UNSUPPORTED if the technology is not available
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_SetTechnologyRank
(
    uint32_t rank,
        ///< [IN] Rank of the used technology
    le_data_Technology_t technology
        ///< [IN] Technology
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the first technology to use
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator if the list is not empty
 *      - @ref LE_DATA_MAX if the list is empty
 */
//--------------------------------------------------------------------------------------------------
le_data_Technology_t le_data_GetFirstUsedTechnology
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the next technology to use
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator if the list is not empty
 *      - @ref LE_DATA_MAX if the list is empty or the end of the list is reached
 */
//--------------------------------------------------------------------------------------------------
le_data_Technology_t le_data_GetNextUsedTechnology
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the technology of the currently connected data connection. In the absence of an actively
 * connected data connection, @ref LE_DATA_MAX is returned.
 *
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator
 *      - @ref LE_DATA_MAX if not connected
 */
//--------------------------------------------------------------------------------------------------
le_data_Technology_t le_data_GetTechnology
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the default route activation status for the data connection service interface.
 *
 * @return
 *      - true:  the default route is set by the data connection service
 *      - false: the default route is not set by the data connection service
 */
//--------------------------------------------------------------------------------------------------
bool le_data_GetDefaultRouteStatus
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Add a route on the data connection service interface, if the data session is connected using
 * the cellular technology and has an IPv4 or IPv6 address.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    cellular technology not currently used
 *      - LE_BAD_PARAMETER  incorrect IP address
 *      - LE_FAULT          for all other errors
 *
 * @note Limitations:
 *      - only IPv4 is supported for the moment
 *      - route only added for a cellular connection
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_AddRoute
(
    const char* LE_NONNULL ipDestAddrStr
        ///< [IN] The destination IP address in dotted
        ///< format
);



//--------------------------------------------------------------------------------------------------
/**
 * Delete a route on the data connection service interface, if the data session is connected using
 * the cellular technology and has an IPv4 or IPv6 address.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    cellular technology not currently used
 *      - LE_BAD_PARAMETER  incorrect IP address
 *      - LE_FAULT          for all other errors
 *
 * @note Limitations:
 *      - only IPv4 is supported for the moment
 *      - route only removed for a cellular connection
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_DelRoute
(
    const char* LE_NONNULL ipDestAddrStr
        ///< [IN] The destination IP address in dotted
        ///< format
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the cellular profile index used by the data connection service when the cellular technology
 * is active.
 *
 * @return
 *      - Cellular profile index
 */
//--------------------------------------------------------------------------------------------------
int32_t le_data_GetCellularProfileIndex
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the cellular profile index used by the data connection service when the cellular technology
 * is active.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if the profile index is invalid
 *      - LE_BUSY           the cellular connection is in use
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_SetCellularProfileIndex
(
    int32_t profileIndex
        ///< [IN] Cellular profile index to be used
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the date from the configured server using the configured time protocol.
 *
 * @warning An active data connection is necessary to retrieve the date.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if a parameter is incorrect
 *      - LE_FAULT          if an error occurred
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_GetDate
(
    uint16_t* yearPtr,
        ///< [OUT] UTC Year A.D. [e.g. 2017].
    uint16_t* monthPtr,
        ///< [OUT] UTC Month into the year [range 1...12].
    uint16_t* dayPtr
        ///< [OUT] UTC Days into the month [range 1...31].
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the time from the configured server using the configured time protocol.
 *
 * @warning An active data connection is necessary to retrieve the time.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if a parameter is incorrect
 *      - LE_FAULT          if an error occurred
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_GetTime
(
    uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
    uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
    uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
    uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
);


#endif // LE_DATA_INTERFACE_H_INCLUDE_GUARD
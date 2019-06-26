
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_DATA_COMMON_H_INCLUDE_GUARD
#define LE_DATA_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdc_common.h"
#include "le_dcs_common.h"

#define IFGEN_LE_DATA_PROTOCOL_ID "aac783b0085c3fbdc0ff6e0a660dd11d"
#define IFGEN_LE_DATA_MSG_SIZE 113



//--------------------------------------------------------------------------------------------------
/**
 * Interface name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DATA_INTERFACE_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Interface name string length. One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DATA_INTERFACE_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Request function and used by Release function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_data_RequestObj* le_data_RequestObjRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Technologies.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DATA_WIFI = 0,
        ///< Wi-Fi
    LE_DATA_CELLULAR = 1,
        ///< Cellular
    LE_DATA_MAX = 2
        ///< Unknown state.
}
le_data_Technology_t;


//--------------------------------------------------------------------------------------------------
/**
 * Time protocols.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DATA_TP = 0,
        ///< Time Protocol, defined in RFC 868
    LE_DATA_NTP = 1
        ///< Network Time Protocol, defined in RFC 5905
}
le_data_TimeProtocol_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_data_ConnectionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_data_ConnectionStateHandler* le_data_ConnectionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for connection state changes
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_data_ConnectionStateHandlerFunc_t)
(
        const char* LE_NONNULL intfName,
        ///< Interface name for the data connection
        bool isConnected,
        ///< State: connected or disconnected
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_data_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_data_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_data_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_data_ConnectionState'
 *
 * This event provides information on connection state changes
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_data_ConnectionStateHandlerRef_t ifgen_le_data_AddConnectionStateHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED void ifgen_le_data_RemoveConnectionStateHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_data_RequestObjRef_t ifgen_le_data_Request
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Release a previously requested data connection
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_data_Release
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_data_SetTechnologyRank
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_data_Technology_t ifgen_le_data_GetFirstUsedTechnology
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the next technology to use
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator if the list is not empty
 *      - @ref LE_DATA_MAX if the list is empty or the end of the list is reached
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_data_Technology_t ifgen_le_data_GetNextUsedTechnology
(
    le_msg_SessionRef_t _ifgen_sessionRef
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
LE_SHARED le_data_Technology_t ifgen_le_data_GetTechnology
(
    le_msg_SessionRef_t _ifgen_sessionRef
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
LE_SHARED bool ifgen_le_data_GetDefaultRouteStatus
(
    le_msg_SessionRef_t _ifgen_sessionRef
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
LE_SHARED le_result_t ifgen_le_data_AddRoute
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_data_DelRoute
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED int32_t ifgen_le_data_GetCellularProfileIndex
(
    le_msg_SessionRef_t _ifgen_sessionRef
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
LE_SHARED le_result_t ifgen_le_data_SetCellularProfileIndex
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_data_GetDate
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_data_GetTime
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
        uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
        uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
        uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
);

#endif // LE_DATA_COMMON_H_INCLUDE_GUARD
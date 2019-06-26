
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_AVDATA_COMMON_H_INCLUDE_GUARD
#define LE_AVDATA_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_AVDATA_PROTOCOL_ID "d042ee9f795ba3ff64aacf91d970b8fc"
#define IFGEN_LE_AVDATA_MSG_SIZE 794



//--------------------------------------------------------------------------------------------------
/**
 * Define the maximum characters and bytes of a path name
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVDATA_PATH_NAME_LEN 511

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVDATA_PATH_NAME_BYTES 512

//--------------------------------------------------------------------------------------------------
/**
 * Define the maximum characters and bytes of a string
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVDATA_STRING_VALUE_LEN 255

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVDATA_STRING_VALUE_BYTES 256

//--------------------------------------------------------------------------------------------------
/**
 * Resource access modes:
 *   - Variable: read (server), read/write (client)
 *   - Setting: read/write (server), read/write (client)
 *   - Command: execute (server)
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVDATA_ACCESS_VARIABLE = 0,
        ///< read(server) or read/write(client)
    LE_AVDATA_ACCESS_SETTING = 1,
        ///< read/write (server) or read/write (client)
    LE_AVDATA_ACCESS_COMMAND = 2
        ///< execute (server)
}
le_avdata_AccessMode_t;


//--------------------------------------------------------------------------------------------------
/**
 * Resource access types
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVDATA_ACCESS_READ = 0x1,        ///< read access type
    LE_AVDATA_ACCESS_WRITE = 0x2,        ///< write access type
    LE_AVDATA_ACCESS_EXEC = 0x4        ///< execute access type
}
le_avdata_AccessType_t;


//--------------------------------------------------------------------------------------------------
/**
 * Resource namespace
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVDATA_NAMESPACE_APPLICATION = 0,
        ///<
    LE_AVDATA_NAMESPACE_GLOBAL = 1
        ///<
}
le_avdata_Namespace_t;


//--------------------------------------------------------------------------------------------------
/**
 * Data types
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVDATA_DATA_TYPE_NONE = 0,
        ///< Null Data Type
    LE_AVDATA_DATA_TYPE_INT = 1,
        ///< Integer Data Type
    LE_AVDATA_DATA_TYPE_FLOAT = 2,
        ///< Float Data Type
    LE_AVDATA_DATA_TYPE_BOOL = 3,
        ///< Boolean Data Type
    LE_AVDATA_DATA_TYPE_STRING = 4
        ///< String Data Type
}
le_avdata_DataType_t;


//--------------------------------------------------------------------------------------------------
/**
 * Status of the data push
 * @todo Provide additional status to troubleshoot delivery problems
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVDATA_PUSH_SUCCESS = 0,
        ///< Push was successful
    LE_AVDATA_PUSH_FAILED = 1
        ///< Push has failed
}
le_avdata_PushStatus_t;


//--------------------------------------------------------------------------------------------------
/**
 * Argument list reference, for command only.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avdata_ArgumentList* le_avdata_ArgumentListRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avdata_ResourceEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avdata_ResourceEventHandler* le_avdata_ResourceEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * A record reference
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avdata_Record* le_avdata_RecordRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by RequestSession function and used by ReleaseSession function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avdata_RequestSessionObj* le_avdata_RequestSessionObjRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * AVMS session state
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVDATA_SESSION_STARTED = 0,
        ///< AVMS session started
    LE_AVDATA_SESSION_STOPPED = 1
        ///< AVMS session stopped
}
le_avdata_SessionState_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avdata_SessionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avdata_SessionStateHandler* le_avdata_SessionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for resource activity. Note that the path returned by this event follows the unix format
 * (e.g. "/a/b/c") even if the app uses the URL format (e.g "a.b.c") when registering
 * the handler.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_avdata_ResourceHandlerFunc_t)
(
        const char* LE_NONNULL path,
        ///<
        le_avdata_AccessType_t accessType,
        ///<
        le_avdata_ArgumentListRef_t argumentListRef,
        ///<
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for pushing data result.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_avdata_CallbackResultFunc_t)
(
        le_avdata_PushStatus_t status,
        ///<
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for AV session changes
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_avdata_SessionStateHandlerFunc_t)
(
        le_avdata_SessionState_t sessionState,
        ///< Session started or stopped?
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_avdata_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avdata_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avdata_ResourceEvent'
 *
 * Upon resource access on the server side, the registered handler is called.
 *
 * For "settings" (read/write), the same handler is called for both read and write access.
 *
 * For "commands", the handler function must call the "ReplyExecResult" function to send the command
 * execution result back to the AVC daemon (which then sends the proper response back to the AV
 * server).
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avdata_ResourceEventHandlerRef_t ifgen_le_avdata_AddResourceEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        le_avdata_ResourceHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avdata_ResourceEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avdata_RemoveResourceEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ResourceEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Create an asset data with the provided path. Note that asset data type and value are determined
 * upon the first call to a Set function. When an asset data is created, it contains a null value,
 * represented by the data type of none.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_DUPLICATE if path has already been called by CreateResource before, or path is parent
 *        or child to an existing Asset Data path.
 *      - LE_FAULT on any other error.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_CreateResource
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        le_avdata_AccessMode_t accessMode
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets the namespace for asset data.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the namespace is unknown
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_SetNamespace
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_Namespace_t _namespace
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to contain a null value, represented by the data type of none.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_SetNull
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Gets the integer value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetInt
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        int32_t* valuePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to an integer value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_SetInt
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        int32_t value
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Gets the float value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetFloat
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        double* valuePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to a float value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_SetFloat
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        double value
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Gets the bool value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetBool
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        bool* valuePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to a bool value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_SetBool
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        bool value
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Gets the string value of an asset data.
 *
 * @return:
 *      - LE_BAD_PARAMETER - asset data being accessed is of the wrong data type
 *      - LE_UNAVAILABLE - asset data contains null value
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OVERFLOW - asset data length exceeds the maximum length
 *      - LE_OK - access successful
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetString
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        char* value,
        ///< [OUT]
        size_t valueSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets an asset data to a string value.
 *
 * @return:
 *      - LE_NOT_FOUND - if the path is invalid and does not point to an asset data
 *      - LE_NOT_PERMITTED - asset data being accessed does not have the right permission
 *      - LE_OK - access successful.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_SetString
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        const char* LE_NONNULL value
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the bool argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetBoolArg
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
        const char* LE_NONNULL argName,
        ///< [IN]
        bool* boolArgPtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the float argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetFloatArg
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
        const char* LE_NONNULL argName,
        ///< [IN]
        double* floatArgPtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the int argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetIntArg
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
        const char* LE_NONNULL argName,
        ///< [IN]
        int32_t* intArgPtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the string argument with the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API
 *      - LE_OVERFLOW - argument length exceeds the maximum length
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetStringArg
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
        const char* LE_NONNULL argName,
        ///< [IN]
        char* strArg,
        ///< [OUT]
        size_t strArgSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the length (excluding terminating null byte) of the string argument of the specified name.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if argument doesn't exist, or its data type doesn't match the API.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_GetStringArgLength
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
        const char* LE_NONNULL argName,
        ///< [IN]
        int32_t* strArgLenPtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Reply command execution result to AVC Daemon, which can then respond to AV server. This function
 * MUST be called at the end of a command execution, in order for AV server to be notified about the
 * execution status.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avdata_ReplyExecResult
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_ArgumentListRef_t argumentListRef,
        ///< [IN]
        le_result_t result
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Push asset data to the server.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the provided path doesn't exist
 *      - LE_BUSY if push service is busy. Data added to queue list for later push
 *      - LE_OVERFLOW if data size exceeds the maximum allowed size
 *      - LE_NO_MEMORY if push queue is full, try again later
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_Push
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        le_avdata_CallbackResultFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Push data dump to a specified path on the server.
 *
 * @return:
 *      - LE_OK on success
 *      - LE_BUSY if push service is busy. Data added to queue list for later push
 *      - LE_OVERFLOW if data size exceeds the maximum allowed size
 *      - LE_NO_MEMORY if push queue is full, try again later
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_PushStream
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL path,
        ///< [IN]
        int fd,
        ///< [IN]
        le_avdata_CallbackResultFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Create a timeseries record
 *
 * @return Reference to the record
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avdata_RecordRef_t ifgen_le_avdata_CreateRecord
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete a timeseries record
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avdata_DeleteRecord
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RecordRef_t recordRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Accumulate int data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_RecordInt
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RecordRef_t recordRef,
        ///< [IN]
        const char* LE_NONNULL path,
        ///< [IN]
        int32_t value,
        ///< [IN]
        uint64_t timestamp
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Accumulate float data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_RecordFloat
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RecordRef_t recordRef,
        ///< [IN]
        const char* LE_NONNULL path,
        ///< [IN]
        double value,
        ///< [IN]
        uint64_t timestamp
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Accumulate boolean data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_RecordBool
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RecordRef_t recordRef,
        ///< [IN]
        const char* LE_NONNULL path,
        ///< [IN]
        bool value,
        ///< [IN]
        uint64_t timestamp
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Accumulate string data
 *
 * @note The client will be terminated if the recordRef is not valid, or the resource doesn't exist
 *
 * @return:
 *      - LE_OK on success
 *      - LE_NO_MEMORY if the current entry was NOT added because the time series buffer is full.
 *      - LE_FAULT on any other error
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_RecordString
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RecordRef_t recordRef,
        ///< [IN]
        const char* LE_NONNULL path,
        ///< [IN]
        const char* LE_NONNULL value,
        ///< [IN]
        uint64_t timestamp
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Push record to the server
 *
 ** @return:
 *      - LE_OK on success
 *      - LE_BUSY if push service is busy. Data added to queue list for later push
 *      - LE_OVERFLOW if data size exceeds the maximum allowed size
 *      - LE_NO_MEMORY if push queue is full, try again later
 *      - LE_FAULT on any other error
 *
 * * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avdata_PushRecord
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RecordRef_t recordRef,
        ///< [IN]
        le_avdata_CallbackResultFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avdata_SessionState'
 *
 * This event provides information on AV session state changes
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avdata_SessionStateHandlerRef_t ifgen_le_avdata_AddSessionStateHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avdata_SessionState'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avdata_RemoveSessionStateHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_SessionStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Request the avcServer to open a session.
 *
 * @return
 *      - SessionRef if session request succeeded
 *      - NULL if session request failed
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avdata_RequestSessionObjRef_t ifgen_le_avdata_RequestSession
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Request the avcServer to close a session.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avdata_ReleaseSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avdata_RequestSessionObjRef_t requestRef
        ///< [IN] Reference to a previously opened AV session.
);

#endif // LE_AVDATA_COMMON_H_INCLUDE_GUARD

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_SMSINBOX1_COMMON_H_INCLUDE_GUARD
#define LE_SMSINBOX1_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdmDefs_common.h"
#include "le_sim_common.h"
#include "le_sms_common.h"

#define IFGEN_LE_SMSINBOX1_PROTOCOL_ID "e5657c8f2742e4c9390d2b2bb639e1b8"
#define IFGEN_LE_SMSINBOX1_MSG_SIZE 192



//--------------------------------------------------------------------------------------------------
/**
 * Define unknown format
 */
//--------------------------------------------------------------------------------------------------
#define LE_SMSINBOX1_FORMAT_UNKNOWN -1

//--------------------------------------------------------------------------------------------------
/**
 * Define the name of length of the message box name
 */
//--------------------------------------------------------------------------------------------------
#define LE_SMSINBOX1_MAX_MBOX_NAME_LEN 12

//--------------------------------------------------------------------------------------------------
/**
 * Reference type for referring to open message box sessions.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_smsInbox1_Session* le_smsInbox1_SessionRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_smsInbox1_RxMessage'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_smsInbox1_RxMessageHandler* le_smsInbox1_RxMessageHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for New Message.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_smsInbox1_RxMessageHandlerFunc_t)
(
        uint32_t msgId,
        ///< Message identifier.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_smsInbox1_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_smsInbox1_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Open a message box.
 *
 * @return
 * Reference on the opened message box session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_smsInbox1_SessionRef_t ifgen_le_smsInbox1_Open
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Close a previously open message box.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_smsInbox1_Close
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_smsInbox1_SessionRef_t sessionRef
        ///< [IN] Mailbox session reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_smsInbox1_RxMessage'
 *
 * This event provides information on new received messages.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_smsInbox1_RxMessageHandlerRef_t ifgen_le_smsInbox1_AddRxMessageHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_smsInbox1_RxMessageHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_smsInbox1_RxMessage'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_smsInbox1_RemoveRxMessageHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_smsInbox1_RxMessageHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete a Message.
 *
 * @note It is valid to delete a non-existent message.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_smsInbox1_DeleteMsg
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId
        ///< [IN] Message identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the IMSI of the message receiver SIM if it applies.
 *
 * @return
 *  - LE_NOT_FOUND     The message item is not tied to a SIM card, the imsi string is empty.
 *  - LE_OVERFLOW      The imsiPtr buffer was too small for the IMSI.
 *  - LE_BAD_PARAMETER The message reference is invalid.
 *  - LE_FAULT         The function failed.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetImsi
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId,
        ///< [IN] Message identifier.
        char* imsi,
        ///< [OUT] IMSI.
        size_t imsiSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the message format (text, binary or PDU).
 *
 * @return
 *  - Message format.
 *  - FORMAT_UNKNOWN when the message format cannot be identified or the message reference is
 *    invalid.
 * @note For incoming SMS Inbox, format returned by le_smsInbox1_GetFormat() is never
 * LE_SMSINBOX1_FORMAT_PDU.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_sms_Format_t ifgen_le_smsInbox1_GetFormat
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId
        ///< [IN] Message identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Sender Identifier.
 *
 * @return
 *  - LE_BAD_PARAMETER The message reference is invalid.
 *  - LE_OVERFLOW      Identifier length exceed the maximum length.
 *  - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetSenderTel
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId,
        ///< [IN] Message identifier.
        char* tel,
        ///< [OUT] Identifier string.
        size_t telSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Message Time Stamp string (it does not apply for PDU message).
 *
 * @return
 *  - LE_BAD_PARAMETER The message reference is invalid.
 *  - LE_NOT_FOUND     The message is a PDU message.
 *  - LE_OVERFLOW      Timestamp number length exceed the maximum length.
 *  - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetTimeStamp
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId,
        ///< [IN] Message identifier.
        char* timestamp,
        ///< [OUT] Message time stamp (for text or binary
        ///<  messages). String format:
        ///< "yy/MM/dd,hh:mm:ss+/-zz"
        ///< (Year/Month/Day,Hour:Min:Seconds+/-TimeZone)
        size_t timestampSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the message Length value.
 *
 * @return Number of characters for text messages, or the length of the data in bytes for raw
 *         binary and PDU messages.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED size_t ifgen_le_smsInbox1_GetMsgLen
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId
        ///< [IN] Message identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the text Message.
 *
 *
 * @return
 *  - LE_BAD_PARAMETER The message reference is invalid.
 *  - LE_FORMAT_ERROR  Message is not in text format.
 *  - LE_OVERFLOW      Message length exceed the maximum length.
 *  - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetText
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId,
        ///< [IN] Message identifier.
        char* text,
        ///< [OUT] Message text.
        size_t textSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the binary Message.
 *
 *
 * @return
 *  - LE_BAD_PARAMETER The message reference is invalid.
 *  - LE_FORMAT_ERROR  Message is not in binary format.
 *  - LE_OVERFLOW      Message length exceed the maximum length.
 *  - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetBinary
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId,
        ///< [IN] Message identifier.
        uint8_t* binPtr,
        ///< [OUT] Binary message.
        size_t* binSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the PDU message.
 *
 * Output parameters are updated with the PDU message content and the length of the PDU message
 * in bytes.
 *
 * @return
 *  - LE_BAD_PARAMETER The message reference is invalid.
 *  - LE_FORMAT_ERROR  Unable to encode the message in PDU.
 *  - LE_OVERFLOW      Message length exceed the maximum length.
 *  - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetPdu
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId,
        ///< [IN] Message identifier.
        uint8_t* pduPtr,
        ///< [OUT] PDU message.
        size_t* pduSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the first Message object reference in the inbox message.
 *
 * @return
 *  - 0 No message found (message box parsing is over).
 *  - Message identifier.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED uint32_t ifgen_le_smsInbox1_GetFirst
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_smsInbox1_SessionRef_t sessionRef
        ///< [IN] Mailbox session reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the next Message object reference in the inbox message.
 *
 * @return
 *  - 0 No message found (message box parsing is over).
 *  - Message identifier.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED uint32_t ifgen_le_smsInbox1_GetNext
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_smsInbox1_SessionRef_t sessionRef
        ///< [IN] Mailbox session reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * allow to know whether the message has been read or not. The message status is tied to the client
 * app.
 *
 * @return True if the message is unread, false otherwise.
 *
 * @note If the caller is passing a bad message reference into this function, it is a fatal error,
 *        the function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_smsInbox1_IsUnread
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId
        ///< [IN] Message identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Mark a message as 'read'.
 *
 * @note If the caller is passing a bad message reference into this function, it is a fatal error,
 *        the function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_smsInbox1_MarkRead
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId
        ///< [IN] Message identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Mark a message as 'unread'.
 *
 * @note If the caller is passing a bad message reference into this function, it is a fatal error,
 *        the function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_smsInbox1_MarkUnread
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t msgId
        ///< [IN] Message identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the maximum number of messages for message box.
 *
 * @return
 *  - LE_BAD_PARAMETER The message box name is invalid.
 *  - LE_OVERFLOW      Message count exceed the maximum limit.
 *  - LE_OK            Function succeeded.
 *  - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_SetMaxMessages
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t maxMessageCount
        ///< [IN] Maximum number of messages
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the maximum number of messages for message box.
 *
 * @return
 *  - LE_BAD_PARAMETER The message box name is invalid.
 *  - LE_OK            Function succeeded.
 *  - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_smsInbox1_GetMaxMessages
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t* maxMessageCountPtrPtr
        ///< [OUT] Maximum number of messages
);

#endif // LE_SMSINBOX1_COMMON_H_INCLUDE_GUARD
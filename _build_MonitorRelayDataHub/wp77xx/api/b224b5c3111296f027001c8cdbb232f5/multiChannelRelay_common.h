
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef MULTICHANNELRELAY_COMMON_H_INCLUDE_GUARD
#define MULTICHANNELRELAY_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_MULTICHANNELRELAY_PROTOCOL_ID "16de23c0f85cf908c3b282860918809c"
#define IFGEN_MULTICHANNELRELAY_MSG_SIZE 9




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_multiChannelRelay_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_multiChannelRelay_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_multiChannelRelay_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_multiChannelRelay_turn_off_channel
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint8_t channel
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_multiChannelRelay_turn_on_channel
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint8_t channel
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED uint8_t ifgen_multiChannelRelay_getChannelState
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // MULTICHANNELRELAY_COMMON_H_INCLUDE_GUARD
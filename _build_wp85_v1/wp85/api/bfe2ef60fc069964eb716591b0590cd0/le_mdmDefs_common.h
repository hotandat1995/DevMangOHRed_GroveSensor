
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_MDMDEFS_COMMON_H_INCLUDE_GUARD
#define LE_MDMDEFS_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_MDMDEFS_PROTOCOL_ID "190c66f3e7fa9b9f1da713489f318311"
#define IFGEN_LE_MDMDEFS_MSG_SIZE 9



//--------------------------------------------------------------------------------------------------
/**
 * Cf. ITU-T recommendations E.164/E.163. E.164 numbers can have a maximum of 15 digits except the
 * international prefix ('+' or '00'). One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDMDEFS_PHONE_NUM_MAX_LEN 17

//--------------------------------------------------------------------------------------------------
/**
 * Cf. ITU-T recommendations E.164/E.163. E.164 numbers can have a maximum of 15 digits except the
 * international prefix ('+' or '00'). One extra byte is added for the null character.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDMDEFS_PHONE_NUM_MAX_BYTES 18

//--------------------------------------------------------------------------------------------------
/**
 **  IP Version
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDMDEFS_IPV4 = 0,
        ///< IPv4 technology
    LE_MDMDEFS_IPV6 = 1,
        ///< IPv6 technology
    LE_MDMDEFS_IPMAX = 2
        ///<
}
le_mdmDefs_IpVersion_t;



//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_mdmDefs_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_mdmDefs_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_mdmDefs_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

#endif // LE_MDMDEFS_COMMON_H_INCLUDE_GUARD
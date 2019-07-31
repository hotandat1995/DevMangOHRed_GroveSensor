
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_POSCTRL_COMMON_H_INCLUDE_GUARD
#define LE_POSCTRL_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_POSCTRL_PROTOCOL_ID "d47cc30512e69387f546399a44c565e4"
#define IFGEN_LE_POSCTRL_MSG_SIZE 12



//--------------------------------------------------------------------------------------------------
/**
 * Reference type for dealing with Position Service management.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_posCtrl_Activation* le_posCtrl_ActivationRef_t;



//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_posCtrl_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_posCtrl_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_posCtrl_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Request activation of the positioning service.
 *
 * @return
 *      - Reference to the service activation request (to be used later for releasing the request).
 *      - NULL if the service request could not be processed.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_posCtrl_ActivationRef_t ifgen_le_posCtrl_Request
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Release the positioning service.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_posCtrl_Release
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_posCtrl_ActivationRef_t ref
        ///< [IN] Reference to a positioning service activation request.
);

#endif // LE_POSCTRL_COMMON_H_INCLUDE_GUARD

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_TPF_COMMON_H_INCLUDE_GUARD
#define LE_TPF_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_TPF_PROTOCOL_ID "98a7a2c82da94bf8f5efa51e130aa0f0"
#define IFGEN_LE_TPF_MSG_SIZE 271



//--------------------------------------------------------------------------------------------------
/**
 * Maximum package URI length without NULL terminator.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TPF_URI_PACKAGE_MAX_LEN 255

//--------------------------------------------------------------------------------------------------
/**
 * Maximum URI length, one extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TPF_URI_PACKAGE_MAX_SIZE 256


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_tpf_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_tpf_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_tpf_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to set the package URI for download from 3rd party server using HTTP(S) protocol.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if failed to configure the package URI.
 *      - LE_BAD_PARAMETER if the given URI or port number are not in the right format.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_tpf_SetPackageUri
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL packageUri
        ///< [IN] Package address
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to get the package URI for download from 3rd party server.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if failed to configure the package URI.
 *      - LE_BAD_PARAMETER if the given URI or port number are not in the right format.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_tpf_GetPackageUri
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* packageUri,
        ///< [OUT] Package address
        size_t packageUriSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Start a package download from a 3rd party server
 *
 * This will cause a query to be sent to the server, for pending updates.
 *
 * @return
 *      - LE_OK if connection request has been sent.
 *      - LE_FAULT on failure.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_tpf_Start
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the current state of TPF service.
 *
 * @return
 *      - true if TPF service is enabled.
 *      - false if TPF service is disabled.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_tpf_IsTpfStarted
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_TPF_COMMON_H_INCLUDE_GUARD
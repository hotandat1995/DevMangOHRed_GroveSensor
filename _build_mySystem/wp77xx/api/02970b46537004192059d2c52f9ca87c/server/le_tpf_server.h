

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_TPF_INTERFACE_H_INCLUDE_GUARD
#define LE_TPF_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_tpf_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_tpf_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_tpf_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_tpf_AdvertiseService
(
    void
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
le_result_t le_tpf_SetPackageUri
(
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
le_result_t le_tpf_GetPackageUri
(
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
le_result_t le_tpf_Start
(
    void
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
bool le_tpf_IsTpfStarted
(
    void
);


#endif // LE_TPF_INTERFACE_H_INCLUDE_GUARD
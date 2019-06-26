
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_NET_COMMON_H_INCLUDE_GUARD
#define LE_NET_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_dcs_common.h"

#define IFGEN_LE_NET_PROTOCOL_ID "5c1cd37439147f155a34e5cba4221b5f"
#define IFGEN_LE_NET_MSG_SIZE 113



//--------------------------------------------------------------------------------------------------
/**
 * Interface name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_NET_INTERFACE_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * IP addr string's max length
 */
//--------------------------------------------------------------------------------------------------
#define LE_NET_IPADDR_MAX_LEN 46


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_net_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_net_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_net_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add or remove a route on the given channel according to the input flag in the last argument for
 * the given destination address its given subnet, which is a subnet mask for IPv4 and subnet mask's
 * prefix length for IPv6
 *
 * The channel reference in the first input argument identifies the network interface which a route
 * is to be added onto or removed from. Whether the operation is an add or a remove depends on the
 * isAdd boolean value of the last API input argument.
 *
 * The IP address and subnet input arguments specify the destination address and subnet for the
 * route. If it is a network route, the formats used for them are the same as used in the Linux
 * command "route -A inet add -net <ipAddr> netmask <ipSubnet> dev <netInterface>" for IPv4, and
 * "route -A inet6 add -net <ipAddr/prefixLength> dev <netInterface>" for IPv6. If the route is a
 * host route, the subnet input argument should be given as "" (i.e. a null string).
 *
 * @return
 *      - LE_OK upon success, otherwise another le_result_t failure code
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_net_ChangeRoute
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_dcs_ChannelRef_t channelRef,
        ///< [IN] the channel onto which the route change is made
        const char* LE_NONNULL destAddr,
        ///< [IN] Destination IP address for the route
        const char* LE_NONNULL destSubnet,
        ///< [IN] Destination's subnet: IPv4 netmask or IPv6 prefix
        ///< length
        bool isAdd
        ///< [IN] the change is to add (true) or delete (false)
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the default GW addr for the given data channel retrieved from its technology
 *
 * @return
 *      - LE_OK upon success, otherwise LE_FAULT
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_net_SetDefaultGW
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_dcs_ChannelRef_t channelRef
        ///< [IN] the channel on which interface its default GW
        ///< addr is to be set
);

//--------------------------------------------------------------------------------------------------
/**
 * Backup default GW config of the system
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_net_BackupDefaultGW
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Backup default GW config of the system
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_net_RestoreDefaultGW
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the DNS addresses for the given data channel retrieved from its technology
 *
 * @return
 *      - LE_OK upon success, otherwise LE_FAULT
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_net_SetDNS
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_dcs_ChannelRef_t channelRef
        ///< [IN] the channel from which the DNS addresses retrieved
        ///< will be set into the system config
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove the last added DNS addresses via the le_dcs_SetDNS API
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_net_RestoreDNS
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_NET_COMMON_H_INCLUDE_GUARD


/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_NET_INTERFACE_H_INCLUDE_GUARD
#define LE_NET_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_dcs_server.h"

// Internal includes for this interface
#include "le_net_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_net_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_net_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_net_AdvertiseService
(
    void
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
le_result_t le_net_ChangeRoute
(
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
le_result_t le_net_SetDefaultGW
(
    le_dcs_ChannelRef_t channelRef
        ///< [IN] the channel on which interface its default GW
        ///< addr is to be set
);



//--------------------------------------------------------------------------------------------------
/**
 * Backup default GW config of the system
 */
//--------------------------------------------------------------------------------------------------
void le_net_BackupDefaultGW
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Backup default GW config of the system
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_net_RestoreDefaultGW
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the DNS addresses for the given data channel retrieved from its technology
 *
 * @return
 *      - LE_OK upon success, otherwise LE_FAULT
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_net_SetDNS
(
    le_dcs_ChannelRef_t channelRef
        ///< [IN] the channel from which the DNS addresses retrieved
        ///< will be set into the system config
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove the last added DNS addresses via the le_dcs_SetDNS API
 */
//--------------------------------------------------------------------------------------------------
void le_net_RestoreDNS
(
    void
);


#endif // LE_NET_INTERFACE_H_INCLUDE_GUARD
#include "utils.h"

#include "contiki-net.h"

#include <stdio.h>

resolv_status_t set_connection_address(uip_ipaddr_t *ipaddr, char *udp_connection_addr)
{
    resolv_status_t status = RESOLV_STATUS_ERROR;

    if(uiplib_ipaddrconv(udp_connection_addr, ipaddr) == 0)
    {
        uip_ipaddr_t *resolved_addr = NULL;
        status = resolv_lookup(udp_connection_addr,&resolved_addr);
        if(status == RESOLV_STATUS_UNCACHED || status == RESOLV_STATUS_EXPIRED)
        {
            printf("Buscando por %s...\r\n", udp_connection_addr);
            resolv_query(udp_connection_addr);
            status = RESOLV_STATUS_RESOLVING;
        }
        else if(status == RESOLV_STATUS_CACHED && resolved_addr != NULL)
        {
            printf("Busca por \"%s\" concluida!\r\n", udp_connection_addr);
        }
        else if(status == RESOLV_STATUS_RESOLVING)
        {
            printf("Ainda buscando por \"%s\"...\r\n", udp_connection_addr);
        }
        else
        {
            printf("Busca por \"%s\" falhou. status = %d.\r\n", udp_connection_addr, status);
        }
        if(resolved_addr)
        {
            uip_ipaddr_copy(ipaddr, resolved_addr);
        }
    }
    else
    {
        status = RESOLV_STATUS_CACHED;
    }

    return status;
}

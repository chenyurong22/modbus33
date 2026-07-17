#include <stdio.h>
#include "tcp-server.h"

#ifndef MB_MODE
	#define MB_MODE MB_MODE_SLAVE
#endif

#include "mb.h"

int main(int argc,char**argv)
{
    printf("Starting Test Slave over TCP\r\n");
    mb_set_tx_handler(tcp_server_send);
    tcp_server_start();
    while(1)
    {
        tcp_server_rx(mb_rx_new_data);
    }
    tcp_server_stop();
    return 0;
}

#include <stdio.h>
#include "tcp-client.h"

#ifndef MB_MODE
	#define MB_MODE MB_MODE_MASTER
#endif

#include "mb.h"
#include "mb-packet.h"

void master_process(mb_packet_s p)
{
    printf("#");
}

int main(int argc,char**argv)
{
    printf("Starting Test Master over TCP\r\n");
    mb_set_tx_handler(tcp_client_send);
    mb_set_master_process_handler(master_process);
    tcp_client_connect2server();
    while(1)
    {
        mb_tx_packet_handler(mb_packet_request_read_coil(1,0,1));
        mb_tx_packet_handler(mb_packet_request_read_discrete_inputs(1,1,1));
        sleep(1);
        tcp_client_check_rx(mb_rx_new_data);
    }
    tcp_client_close();
    return 0;
}


#include <arpa/inet.h>
#include <fcntl.h>

#include "tcp-client.h"

int sock;
struct sockaddr_in server;
uint8_t rx[256];

void tcp_client_connect2server(void)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock, (struct sockaddr *)&server, sizeof(server));
}

void tcp_client_check_rx(void (*rx_data)(uint8_t))
{
    int n = recv(sock, rx, sizeof(rx), 0);
    if(n>0)
    {
        #ifdef TCP_DEBUG
            printf("Received %d bytes\n", n);
            for(int i=0;i<n;i++)
            {
                printf("0X%02X ",rx[i]);
            }
            printf("\r\n");
        #endif

        for(int i=0;i<n;i++)
        {
            rx_data(rx[i]);
        }
    }
}

void tcp_client_send(uint8_t *Data,uint8_t Len)
{
    send(sock, Data, Len, 0);
    #ifdef TCP_DEBUG
        printf("Send %d bytes\n", Len);
        for(int i=0;i<Len;i++)
        {
            printf("0X%02X ",Data[i]);
        }
        printf("\r\n");
    #endif
}

void tcp_client_close(void)
{
    close(sock);
}

#include <arpa/inet.h>

#include "tcp-server.h"

int listenfd, clientfd;
struct sockaddr_in addr;
uint8_t rx[256];

void tcp_server_start(void)
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listenfd, (struct sockaddr *)&addr, sizeof(addr));

    listen(listenfd, 1);

    printf("Waiting for client...\n");

    clientfd = accept(listenfd, NULL, NULL);

    printf("Client connected.\n");
}

void tcp_server_stop(void)
{
    close(clientfd);
    close(listenfd);
}

void tcp_server_rx(void (*rx_data)(uint8_t))
{
    int n = recv(clientfd, rx, sizeof(rx), 0);
    if (n <= 0)
        return;

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

void tcp_server_send(uint8_t *Data,uint8_t Len)
{
    send(clientfd, Data, Len, 0);
    #ifdef TCP_DEBUG
        printf("Send %d bytes\n", Len);
        for(int i=0;i<Len;i++)
        {
            printf("0X%02X ",Data[i]);
        }
        printf("\r\n");
    #endif
}

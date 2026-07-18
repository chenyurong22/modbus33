#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#include "tcp-server.h"

static int listenfd = -1;
static int clientfd = -1;

static struct sockaddr_in addr;
static uint8_t rx[256];

void tcp_server_start(void)
{
    int opt = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listenfd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
    }

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("bind");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 1) < 0)
    {
        perror("listen");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    printf("TCP Server started on port %d\n", PORT);
}

void tcp_server_stop(void)
{
    if (clientfd >= 0)
    {
        close(clientfd);
        clientfd = -1;
    }

    if (listenfd >= 0)
    {
        close(listenfd);
        listenfd = -1;
    }
}

void tcp_server_rx(void (*rx_data)(uint8_t))
{
    int n;

    /* اگر کلاینتی متصل نیست */
    if (clientfd < 0)
    {
        printf("Waiting for client...\n");

        clientfd = accept(listenfd, NULL, NULL);

        if (clientfd < 0)
        {
            return;
        }

        printf("Client connected.\n");
    }

    n = recv(clientfd, rx, sizeof(rx), 0);

    /* کلاینت قطع شد */
    if (n == 0)
    {
        printf("Client disconnected.\n");

        close(clientfd);
        clientfd = -1;

        return;
    }

    /* خطا */
    if (n < 0)
    {
        perror("recv");

        close(clientfd);
        clientfd = -1;

        return;
    }

#ifdef TCP_DEBUG
    printf("RX (%d): ", n);

    for (int i = 0; i < n; i++)
        printf("%02X ", rx[i]);

    printf("\n");
#endif

    for (int i = 0; i < n; i++)
        rx_data(rx[i]);
}

void tcp_server_send(uint8_t *Data, uint8_t Len)
{
    if (clientfd < 0)
        return;

    if (send(clientfd, Data, Len, 0) <= 0)
    {
        printf("Send failed.\n");

        close(clientfd);
        clientfd = -1;

        return;
    }

#ifdef TCP_DEBUG
    printf("TX (%d): ", Len);

    for (int i = 0; i < Len; i++)
        printf("%02X ", Data[i]);

    printf("\n");
#endif
}

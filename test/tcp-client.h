#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define PORT 1502

#define TCP_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

/* Connect to the TCP server */
void tcp_client_connect2server(void);

/* Register receive callback */
void tcp_client_check_rx(void (*rx_data)(uint8_t));

/* Send data */
void tcp_client_send(uint8_t *data, uint8_t len);

/* Close the connection */
void tcp_client_close(void);

#ifdef __cplusplus
}
#endif

#endif /* TCP_CLIENT_H */

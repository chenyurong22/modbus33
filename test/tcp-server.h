#ifndef TCP_SERVER_H
#define TCP_SERVER_H

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

void tcp_server_start(void);
void tcp_server_stop(void);

/* Register receive callback */
void tcp_server_rx(void (*rx_data)(uint8_t));

/* Send data */
void tcp_server_send(uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* TCP_SERVER_H */

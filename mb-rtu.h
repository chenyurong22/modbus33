#ifndef _MB_LINK_H_
#define _MB_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// #define MB_LINK_DEBUG

#define MB_LINK_Rx_Buffer_Size     MB_DEFAULT_BUFFER_SIZE
#define MB_LINK_Tx_Buffer_Size     MB_DEFAULT_BUFFER_SIZE

#define MB_LINK_Rx_MDBL  (MB_LINK_Rx_Buffer_Size-9)

typedef enum{
    MB_LINK_OK,
    MB_LINK_ERROR_Address,
    MB_LINK_ERROR_Data_Size,
    MB_LINK_ERROR_CRC,
    MB_LINK_ERROR_FUNC
}mb_rtu_error_e;

void            mb_rtu_check_new_data(uint8_t oneByte);
void            mb_rtu_send(uint8_t *Data,uint8_t Len);
void            mb_rtu_reset_rx_buffer(void);
void            mb_rtu_prepare_tx_data(mb_packet_s Packet);
void            mb_rtu_error_handler(mb_rtu_error_e err);
mb_packet_s     mb_rtu_rx_packet_split(uint8_t *Packet_Buffer,uint8_t Len);

void            mb_rx_new_data(uint8_t Byte);								// Get New Data for Detect Valid MODBUS Data
void            mb_rx_timeout_handler(void);								// Reset MODBUS Input Buffer Index as Timeout Error
void            mb_tx_packet_handler(mb_packet_s Packet);					// Prepare and Send Packet

#ifdef __cplusplus
}
#endif

#endif

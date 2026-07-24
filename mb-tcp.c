#include "mb-tcp.h"
#include "mb-packet.h"

static uint16_t transaction_id = 0;

void mb_tcp_send(uint8_t *Data, uint16_t Len)
{
    if(mb.tx_handler!=NULL)
    mb.tx_handler(Data,Len);
}

// Detect MBAP Header
void mb_tcp_receive(uint8_t *Data, uint16_t Len)
{
    mb_packet_s Packet;
    uint16_t pdu_len;

    if(Len < 8) return;

    // Protocol ID
    if(Data[2] != 0 || Data[3] != 0)return;

    // Length
    pdu_len = ((uint16_t)Data[4] << 8) | Data[5];

    // Check Len
    if((pdu_len + 6) != Len) return;

    Packet.unit_id  = Data[6];
    Packet.function = Data[7];
    Packet.length = pdu_len - 2;
    Packet.payload=&Data[8];

    mb_rx_packet_handler(Packet);
}

void mb_tcp_prepare_tx_data(mb_packet_s Packet)
{
    uint8_t Buffer[MB_TCP_Tx_Buffer_Size];
    uint16_t len;

    transaction_id++;

    Buffer[0] = (uint8_t)(transaction_id >> 8);
    Buffer[1] = (uint8_t)(transaction_id & 0xff);
    Buffer[2] = 0;
    Buffer[3] = 0;

    len = Packet.length + 2; // Unit ID + Function + Data

    Buffer[4] = (uint8_t)(len >> 8);
    Buffer[5] = (uint8_t)(len & 0xff);

    Buffer[6] = Packet.unit_id;
    Buffer[7] = Packet.function;

    for(uint16_t i=0;i<Packet.length && (i+8)<MB_TCP_Tx_Buffer_Size;i++)
    {
        Buffer[8+i] = Packet.payload[i];
    }

    mb_tcp_send(Buffer, len+6);
}
/*
 MODBUS Link Layer
 By Liyanboy74
 https://github.com/liyanboy74/modbus
*/

#include <stdio.h>

#include "mb.h"
#include "mb-link.h"
#include "mb-crc.h"
#include "mb-check.h"

uint8_t MB_LINK_Tx_Buffer[MB_LINK_Tx_Buffer_Size];
uint8_t MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Size];

uint8_t MB_LINK_Rx_Buffer_Index=0,MB_LINK_Loop_C=0,MB_LINK_Func=0;

mb_link_error_e MB_LINK_Status=MB_LINK_OK;

void mb_link_error_handler(mb_link_error_e err)
{
    MB_LINK_Status=err;
    #ifdef MB_LINK_DEBUG
    printf("MB_LINK: ");
    switch(err)
    {
        case MB_LINK_OK:printf("OK!\n");break;
        case MB_LINK_ERROR_Data_Size:printf("Data Size ERROR!\n");break;
        case MB_LINK_ERROR_CRC:printf("CRC Error!\n");break;
        case MB_LINK_ERROR_FUNC:printf("Func not match\n");break;
        case MB_LINK_ERROR_Address:printf("Addres not match!\n");break;
        default:break;
    }
    #endif
    return;
}

void mb_link_send(uint8_t *Data,uint8_t Len)
{
    if(MB_Config.tx_handler!=NULL)
    MB_Config.tx_handler(Data,Len);
}

void mb_link_prepare_tx_data(mb_packet_s Packet)
{
    int i;

    MB_LINK_Tx_Buffer[1]=Packet.function;
    #if (MB_MODE==MB_MODE_SLAVE)
        MB_LINK_Tx_Buffer[0]=MB_Config.address;
    #endif
    #if (MB_MODE==MB_MODE_MASTER)
        MB_LINK_Tx_Buffer[0]=Packet.unit_id;
    #endif
    for(i=0;i<Packet.length;i++)
    {
        MB_LINK_Tx_Buffer[i+2]=Packet.payload[i];
    }
    mb_crc_add(MB_LINK_Tx_Buffer,i+2);  //+2=ID+FUNC
    mb_link_send(MB_LINK_Tx_Buffer,i+4);//+2=CRC
    return;
}

void mb_link_reset_rx_buffer(void)
{
    if(MB_LINK_Rx_Buffer_Index)
    {
        MB_LINK_Rx_Buffer_Index=0;
        MB_LINK_Loop_C=0;
        MB_LINK_Func=0;
        // MB_LINK_Packet_Type=MB_PACKET_TYPE_UNKNOWN;
    }
}

void mb_link_check_new_data(uint8_t oneByte)
{
    #if(MB_MODE==MB_MODE_MASTER)

        if(!MB_LINK_Rx_Buffer_Index) // Device Address
        {
            MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
            MB_LINK_Rx_Buffer_Index++;
        }else if(MB_LINK_Rx_Buffer_Index==1) // Func
        {
            MB_LINK_Func=oneByte;
            MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
            MB_LINK_Rx_Buffer_Index++;
        }
        else if(MB_LINK_Func & 0x80)    //MB_PACKET_TYPE_ERROR
        {
            MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
            MB_LINK_Rx_Buffer_Index++;

            if(MB_LINK_Rx_Buffer_Index>=5)
            {
                if(mb_crc_check(MB_LINK_Rx_Buffer,5)==MB_CRC_OK)
                {
                    // OK -> Remove CRC & Go!
                    mb_rx_packet_handler(mb_rx_packet_split(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index-2));
                }
                else mb_link_error_handler(MB_LINK_ERROR_CRC);
                mb_link_reset_rx_buffer();
                return;
            }
        }
        else //check func
        {
            switch (MB_LINK_Func)
            {
                //Slave Res VAR
                case MB_FUNC_Read_Coils:
                case MB_FUNC_Read_Discrete_Inputs:
                case MB_FUNC_Read_Holding_Registers:
                case MB_FUNC_Read_Input_Registers:
                    if(MB_LINK_Rx_Buffer_Index==2) // Size of Data Bytes
                    {
                        if(oneByte>MB_LINK_Rx_MDBL)
                        {
                            mb_link_error_handler(MB_LINK_ERROR_Data_Size);
                            mb_link_reset_rx_buffer();
                            return;
                        }
                        else
                        {
                            MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                            MB_LINK_Rx_Buffer_Index++;
                            MB_LINK_Loop_C=oneByte+MB_LINK_Rx_Buffer_Index+2;
                        }
                    }else if((MB_LINK_Loop_C > MB_LINK_Rx_Buffer_Index) && MB_LINK_Loop_C) // Data + CRC
                    {
                        MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                        MB_LINK_Rx_Buffer_Index++;
                        if(MB_LINK_Loop_C == MB_LINK_Rx_Buffer_Index) // Data Ready
                        {
                            if(mb_crc_check(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index)==MB_CRC_OK)
                            {
                                // OK -> Remove CRC & Go!
                                mb_rx_packet_handler(mb_rx_packet_split(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index-2));
                            }
                            else mb_link_error_handler(MB_LINK_ERROR_CRC);
                            mb_link_reset_rx_buffer();
                            return;
                        }
                    }
                break;
                
                //Slave Res Fix
                case MB_FUNC_Write_Single_Coil:
                case MB_FUNC_Write_Single_Register:
                case MB_FUNC_Write_Multiple_Coils:
                case MB_FUNC_Write_Multiple_Registers:
                    MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                    MB_LINK_Rx_Buffer_Index++;

                    if(MB_LINK_Rx_Buffer_Index>=8)
                    {
                        if(mb_crc_check(MB_LINK_Rx_Buffer,8)==MB_CRC_OK)
                        {
                            // OK -> Remove CRC & Go!
                            mb_rx_packet_handler(mb_rx_packet_split(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index-2));
                        }
                        else mb_link_error_handler(MB_LINK_ERROR_CRC);
                        mb_link_reset_rx_buffer();
                        return;
                    }
                break;
                
                // MB Func Not Match!
                default:
                    mb_link_error_handler(MB_LINK_ERROR_FUNC);
                    mb_link_reset_rx_buffer();
                    return;
                break;
            }
        }

    #elif(MB_MODE==MB_MODE_SLAVE)

        if(!MB_LINK_Rx_Buffer_Index) // Device Address
        {
            if(MB_Config.address==oneByte
            #ifdef MB_SLAVE_LISTEN_BROADCAST
            || oneByte == MB_BROADCAST_ADDRESS
            #endif
            )
            {
                MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                MB_LINK_Rx_Buffer_Index++;
            }
            else mb_link_error_handler(MB_LINK_ERROR_Address);
        }
        else if(MB_LINK_Rx_Buffer_Index==1) // Func
        {
            MB_LINK_Func=oneByte;
            MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
            MB_LINK_Rx_Buffer_Index++;
        }
        else // Check Func
        {
            switch (MB_LINK_Func)
            {
                //master request var
                case MB_FUNC_Write_Multiple_Coils:
                case MB_FUNC_Write_Multiple_Registers:
                case MB_FUNC_Read_Write_Multiple_Registers:
                    if(MB_LINK_Rx_Buffer_Index<((MB_LINK_Func==0x17)?10:6))
                    {
                        MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                        MB_LINK_Rx_Buffer_Index++;
                    }
                    else if(MB_LINK_Rx_Buffer_Index==((MB_LINK_Func==0x17)?10:6))
                    {
                        if(oneByte>MB_LINK_Rx_MDBL)
                        {
                            mb_link_error_handler(MB_LINK_ERROR_Data_Size);
                            mb_link_reset_rx_buffer();
                            return;
                        }
                        else
                        {
                            MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                            MB_LINK_Rx_Buffer_Index++;
                            MB_LINK_Loop_C=oneByte+MB_LINK_Rx_Buffer_Index+2;
                        }
                    }else if((MB_LINK_Loop_C > MB_LINK_Rx_Buffer_Index) && MB_LINK_Loop_C) // Data + CRC
                    {
                        MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                        MB_LINK_Rx_Buffer_Index++;
                        if(MB_LINK_Loop_C == MB_LINK_Rx_Buffer_Index) // Data Ready
                        {
                            if(mb_crc_check(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index)==MB_CRC_OK)
                            {
                                // OK -> Remove CRC & Go!
                                mb_rx_packet_handler(mb_rx_packet_split(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index-2));
                            }
                            else mb_link_error_handler(MB_LINK_ERROR_CRC);
                            mb_link_reset_rx_buffer();
                            return;
                        }
                    }
                
                break;
                //master request fix
                case MB_FUNC_Read_Coils:
                case MB_FUNC_Read_Discrete_Inputs:
                case MB_FUNC_Read_Holding_Registers:
                case MB_FUNC_Read_Input_Registers:
                case MB_FUNC_Write_Single_Coil:
                case MB_FUNC_Write_Single_Register:
                case MB_FUNC_Encapsulated_Interface:
                    {
                        MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                        MB_LINK_Rx_Buffer_Index++;

                        if(MB_LINK_Rx_Buffer_Index>=((MB_LINK_Func==0x2B)?7:8))
                        {
                            if(mb_crc_check(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index)==MB_CRC_OK)
                            {
                                // OK -> Remove CRC & Go!
                                mb_rx_packet_handler(mb_rx_packet_split(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index-2));
                            }
                            else mb_link_error_handler(MB_LINK_ERROR_CRC);
                            mb_link_reset_rx_buffer();
                            return;
                        }
                    }
                break;

                #if MB_ENABLE_FUNC_Read_Exception_Status
                case MB_FUNC_Read_Exception_Status:
                    MB_LINK_Rx_Buffer[MB_LINK_Rx_Buffer_Index]=oneByte;
                    MB_LINK_Rx_Buffer_Index++;
                    if(MB_LINK_Rx_Buffer_Index>=4)
                    {
                        if(mb_crc_check(MB_LINK_Rx_Buffer,4)==MB_CRC_OK)
                        {
                            // OK -> Remove CRC & Go!
                            mb_rx_packet_handler(mb_rx_packet_split(MB_LINK_Rx_Buffer,MB_LINK_Rx_Buffer_Index-2));
                        }
                        else mb_link_error_handler(MB_LINK_ERROR_CRC);
                        mb_link_reset_rx_buffer();
                        return;
                    }
                break;
                #endif
            
                default:
                // MB Func Not Match!
                mb_link_error_handler(MB_LINK_ERROR_FUNC);
                mb_link_reset_rx_buffer();
                return;
                break;
            }

        }
    #endif
}

mb_packet_s mb_rx_packet_split(uint8_t *Packet_Buffer,uint8_t Len)
{
    mb_packet_s Packet;

    Packet.unit_id=Packet_Buffer[0];
    Packet.function=(mb_function_e)Packet_Buffer[1];
    Packet.length=Len-2;//remove unit_id & function
    Packet.payload=&Packet_Buffer[2];
    return Packet;
}

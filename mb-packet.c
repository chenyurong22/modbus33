/*
 MODBUS Packet Maker
 By Liyanboy74
 https://github.com/liyanboy74/modbus
*/

#include "mb-packet.h"

uint8_t MB_PACKET_Buffer[MB_PACKET_Buffer_Size];

#if (MB_MODE==MB_MODE_MASTER)

mb_packet_s mb_packet_type_master_request_var(mb_function_e function,uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity,uint8_t len,uint8_t *Data)
{
	int i;
	mb_packet_s Packet={.payload=MB_PACKET_Buffer};
	Packet.unit_id=DeviceAddress;
	Packet.function= function;
	Packet.length= 5+len;
	Packet.payload[0]=(uint8_t)(StartAd>>8)&0xff;
	Packet.payload[1]=(uint8_t)(StartAd&0xff);
	Packet.payload[2]=(uint8_t)(Quantity>>8)&0xff;
	Packet.payload[3]=(uint8_t)(Quantity&0xff);
	Packet.payload[4]=len;
	for(i=0;i<len && (i+1<MB_PACKET_Buffer_Size);i++)
		Packet.payload[5+i]=Data[i];
	return Packet;
}

mb_packet_s mb_packet_type_master_request_fix(mb_function_e function,uint8_t DeviceAddress,uint16_t w1,uint16_t w2)
{
	mb_packet_s Packet={.payload=MB_PACKET_Buffer};
	Packet.unit_id=DeviceAddress;
	Packet.function= function;
	Packet.length=4;
	Packet.payload[0]=(uint8_t)(w1>>8)&0xff;
	Packet.payload[1]=(uint8_t)(w1&0xff);
	Packet.payload[2]=(uint8_t)(w2>>8)&0xff;
	Packet.payload[3]=(uint8_t)(w2&0xff);
	return Packet;
}

mb_packet_s mb_packet_request_read_coil(uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	return mb_packet_type_master_request_fix(MB_FUNC_Read_Coils,DeviceAddress,StartAd,Quantity);
}

mb_packet_s mb_packet_request_read_discrete_inputs(uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	return mb_packet_type_master_request_fix(MB_FUNC_Read_Discrete_Inputs,DeviceAddress,StartAd,Quantity);
}

mb_packet_s mb_packet_request_read_holding_registers(uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	return mb_packet_type_master_request_fix(MB_FUNC_Read_Holding_Registers,DeviceAddress,StartAd,Quantity);
}

mb_packet_s mb_packet_request_read_input_registers(uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity)
{
	return mb_packet_type_master_request_fix(MB_FUNC_Read_Input_Registers,DeviceAddress,StartAd,Quantity);
}

mb_packet_s mb_packet_request_write_single_coil(uint8_t DeviceAddress,uint16_t CAddress,uint16_t Data)
{
	return mb_packet_type_master_request_fix(MB_FUNC_Write_Single_Coil,DeviceAddress,CAddress,Data);
}

mb_packet_s mb_packet_request_write_single_register(uint8_t DeviceAddress,uint16_t RAddress,uint16_t Data)
{
	return mb_packet_type_master_request_fix(MB_FUNC_Write_Single_Register,DeviceAddress,RAddress,Data);
}

mb_packet_s mb_packet_request_write_multiple_coils(uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity,uint8_t len,uint8_t *Data)
{
	return mb_packet_type_master_request_var(MB_FUNC_Write_Multiple_Coils,DeviceAddress,StartAd,Quantity,len,Data);
}

mb_packet_s mb_packet_request_write_multiple_registers(uint8_t DeviceAddress,uint16_t StartAd,uint16_t Quantity,uint8_t len,uint8_t *Data)
{
	return mb_packet_type_master_request_var(MB_FUNC_Write_Multiple_Registers,DeviceAddress,StartAd,Quantity,len,Data);
}

#endif
#if (MB_MODE==MB_MODE_SLAVE)

mb_packet_s mb_packet_type_slave_responce_var(mb_function_e function,uint8_t len,uint8_t *Data)
{
	int i;
	mb_packet_s Packet={.payload=MB_PACKET_Buffer};
	Packet.function= function;
	Packet.length= len+1;
	Packet.payload[0]=len;
	for(i=0; i<len && (i+1<MB_PACKET_Buffer_Size);i++)
		Packet.payload[i+1]=Data[i];
	return Packet;
}

mb_packet_s mb_packet_type_slave_responce_fix(mb_function_e function,uint16_t w1,uint16_t w2)
{
	mb_packet_s Packet={.payload=MB_PACKET_Buffer};
	Packet.function= function;
	Packet.length=4;
	Packet.payload[0]=(uint8_t)(w1>>8)&0xff;
	Packet.payload[1]=(uint8_t)(w1&0xff);
	Packet.payload[2]=(uint8_t)(w2>>8)&0xff;
	Packet.payload[3]=(uint8_t)(w2&0xff);
	return Packet;
}

mb_packet_s mb_packet_response_read_exeption_status(uint8_t Status)
{
	mb_packet_s Packet={.payload=MB_PACKET_Buffer};
	Packet.function= MB_FUNC_Read_Exception_Status;
	Packet.length=1;
	Packet.payload[0]=Status;
	return Packet;
}

mb_packet_s mb_packet_response_read_coil(uint8_t len,uint8_t *Data)
{
	return mb_packet_type_slave_responce_var(MB_FUNC_Read_Coils,len,Data);
}

mb_packet_s mb_packet_response_read_discrete_inputs(uint8_t len,uint8_t *Data)
{
	return mb_packet_type_slave_responce_var(MB_FUNC_Read_Discrete_Inputs,len,Data);
}

mb_packet_s mb_packet_response_read_holding_registers(uint8_t len,uint8_t *Data)
{
	return mb_packet_type_slave_responce_var(MB_FUNC_Read_Holding_Registers,len,Data);
}

mb_packet_s mb_packet_response_read_write_multiple_registers(uint8_t len,uint8_t *Data)
{
	return mb_packet_type_slave_responce_var(MB_FUNC_Read_Write_Multiple_Registers,len,Data);
}

mb_packet_s mb_packet_response_read_input_registers(uint8_t len,uint8_t *Data)
{
	return mb_packet_type_slave_responce_var(MB_FUNC_Read_Input_Registers,len,Data);
}

mb_packet_s mb_packet_response_write_single_coil(uint16_t CAddress,uint16_t Data)
{
	return mb_packet_type_slave_responce_fix(MB_FUNC_Write_Single_Coil,CAddress,Data);
}

mb_packet_s mb_packet_response_write_single_register(uint16_t RAddress,uint16_t Data)
{
	return mb_packet_type_slave_responce_fix(MB_FUNC_Write_Single_Register,RAddress,Data);
}

mb_packet_s mb_packet_response_write_multiple_coils(uint16_t StartAd,uint16_t Quantity)
{
	return mb_packet_type_slave_responce_fix(MB_FUNC_Write_Multiple_Coils,StartAd,Quantity);
}

mb_packet_s mb_packet_response_write_multiple_registers(uint16_t StartAd,uint16_t Quantity)
{
	return mb_packet_type_slave_responce_fix(MB_FUNC_Write_Multiple_Registers,StartAd,Quantity);
}

mb_packet_s mb_packet_error(mb_function_e Func,mb_error_e Exeption_Code)
{
	// Packet.type=MB_PACKET_TYPE_ERROR;
	mb_packet_s Packet={.payload=MB_PACKET_Buffer};
	Packet.function= 0x80|Func;
	Packet.length=1;
	Packet.payload[0]= Exeption_Code;
	return Packet;
}

#endif

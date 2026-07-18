# MODBUS
Modbus RTU C library for embedded systems.

### Guide 

Follow below steps:
- Select master or slave mode in `mb.h` or in Compiler Defines.
- Send Byte-by-Byte received data to `mb_rx_new_data()` for processing .
- Use `mb_set_tx_handler()` for set callback for transmit data from MODBUS layer.
- In master mode, received packet don't process automatically! so the callback must be set by `mb_set_master_process_handler()` .

**Note:** The library don't handle Rx Timout error! the `mb_rx_timeout_handler()` must be call for reset rx buffer index .

### Examples
- [STM32 MODBUS Slave Example](https://github.com/ioelectro/modbus-stm32-slave-example.git)
- [STM8 MODBUS Slave Example](https://github.com/ioelectro/modbus-stm8-slave-example)
- [AVR MODBUS Network Example](https://github.com/ioelectro/modbus-avr-example.git)


### File Description
**Common**
- `mb` : Common MODBUS init,functions and types
- `mb-link` : Prepare Packet for send, receive and process
- `mb-crc` : Cyclic redundancy check
- `mb-packet` : Make packets

**Slave**
- `mb-check` : Checking received data
- `mb-process` : Process received packets
- `mb-table` : Database

### Running Test Server

A test server is provided to run and test the Modbus RTU slave implementation.

To build and start the test server:

```bash
cd test
make server
./server
```
The server listens on TCP port 1502 and allows a client application to send test frames to the Modbus RTU slave.

The TCP connection is only used as a test interface; the Modbus protocol implementation remains RTU.

#ifndef SERIAL_H_
#define SERIAL_H_

#include <samd21.h>
#include <stdint.h>

#define STDIO_SERCOM SERCOM2
#define STDIO_SERCOM_IRQn SERCOM2_IRQn
#define STDIO_SERCOM_GCLK_ID_CORE GCLK_CLKCTRL_ID_SERCOM2_CORE_Val

class Serial{
public:
	Serial();
	void write(uint8_t byte);
	char read();
	
	void print(const char* str, const char* endl = "\n");
	void print(uint16_t value, const char* endl = "\n");

	void (*line_received_cb)(char*) = nullptr;
	void (*receive_buffer_full_cb)(char*) = nullptr;
};

extern Serial serial;

#endif
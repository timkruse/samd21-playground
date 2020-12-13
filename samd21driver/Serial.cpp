#include "Serial.h"
#include <string>
#include "Pin.h"

Serial serial;

void STDIO_SERCOM_Handler(){
	static char recv_buffer[64];
	static uint8_t recv_buffer_len = 0;
	char c = ((SercomUsart *)STDIO_SERCOM)->DATA.bit.DATA;
	if(c == '\n') {
		recv_buffer[recv_buffer_len] = 0; // overwrites newline
		if(serial.line_received_cb != nullptr) serial.line_received_cb((recv_buffer)); // copy buffer to a string and call dedicated function
		recv_buffer_len = 0;
	} else {
		if (recv_buffer_len < 63) recv_buffer[recv_buffer_len++] = c;
		if (recv_buffer_len >= 63){
			recv_buffer_len = 0;
			if(serial.receive_buffer_full_cb != nullptr) serial.receive_buffer_full_cb((recv_buffer));
		}	
	}
}

Serial::Serial(){
//	Configuration of STDIO_SERCOM as UART. RX/PAD[1] on PA09 and TX/PAD[0] on PA08, peripheral D
	// Config PA08 as output of STDIO_SERCOM.PAD[0] (USART.TX) -> pin func D
	Pin uart_tx_pa08(Pin::Port::A, 8, Pin::Function::GPIO_Output | Pin::Function::PeripheralD);

	// Config PA09 as input of STDIO_SERCOM.PAD[1] (USART.RX) -> pin func D
	Pin uart_rx_pa09(Pin::Port::A, 9, Pin::Function::GPIO_Input | Pin::Function::PeripheralD);

	// Config STDIO_SERCOM peripheral clock
	GCLK_CLKCTRL_Type gclk_clkctrl_config = {.reg = 0};
	gclk_clkctrl_config.bit.ID = STDIO_SERCOM_GCLK_ID_CORE;
	gclk_clkctrl_config.bit.CLKEN = true;
	gclk_clkctrl_config.bit.GEN = GCLK_CLKCTRL_GEN_GCLK0_Val;
	GCLK->CLKCTRL.reg = gclk_clkctrl_config.reg;

//	USART Config:

	// enable receive complete interrupt
	((SercomUsart *)STDIO_SERCOM)->INTENSET.bit.RXC = true;
	NVIC_ClearPendingIRQ(STDIO_SERCOM_IRQn);
	NVIC_SetPriority(STDIO_SERCOM_IRQn, 3); // lowest prio
	NVIC_EnableIRQ(STDIO_SERCOM_IRQn);

	((SercomUsart *)STDIO_SERCOM)->CTRLA.bit.ENABLE = false; // disable to be able to write to ctrl regs
	// Configure STDIO_SERCOM as USART for 115200/8N1
	SercomUsart uart;

	uart.CTRLB.reg = 0; // create config
	uart.CTRLB.bit.RXEN = true;
	uart.CTRLB.bit.TXEN = true;

	uart.BAUD.reg = 0; // create config
	uart.BAUD.bit.BAUD = 59944; // 63019->115200baud, 59944 -> 256000, Table 31-2 Asynchronous Arithmetic

	uart.CTRLA.reg = 0; // create config
	uart.CTRLA.bit.RUNSTDBY = true;
	uart.CTRLA.bit.RXPO = 1; // SERCOM PAD1 used for rx
	uart.CTRLA.bit.TXPO = 0; // SERCOM PAD0 used for tx
	uart.CTRLA.bit.MODE = 1; // using internal clock
	uart.CTRLA.bit.DORD = 1; // lsb first
	uart.CTRLA.bit.ENABLE = true; // enable peripheral

	// install config
	((SercomUsart *)STDIO_SERCOM)->CTRLB.reg = uart.CTRLB.reg;
	((SercomUsart *)STDIO_SERCOM)->BAUD.reg = uart.BAUD.reg;
	((SercomUsart *)STDIO_SERCOM)->CTRLA.reg = uart.CTRLA.reg; // this enables the uart
}
void Serial::write(uint8_t byte){
	while(!((SercomUsart *)STDIO_SERCOM)->INTFLAG.bit.DRE);
	((SercomUsart *)STDIO_SERCOM)->DATA.bit.DATA = byte; // write byte
}
char Serial::read(){
	while(!((SercomUsart *)STDIO_SERCOM)->INTFLAG.bit.RXC);
	return ((SercomUsart *)STDIO_SERCOM)->DATA.bit.DATA; // read byte
}
void Serial::print(const char* str, const char* endl){
	const char* ptr = str;
	while(*ptr != 0){
		this->write(*ptr++);
	}
	ptr = endl;
	while(*ptr != 0){
		this->write(*ptr++);
	}
}

void Serial::print(uint16_t value, const char* endl){
	using namespace std;
	string s = to_string(value);
	for(auto c : s){
		this->write((uint8_t) c);
	}
	this->write((uint8_t) *endl);
}

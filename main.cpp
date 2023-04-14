#include <samd21.h>
#include <stdint.h>
#include "samd21driver/Pin.h"
#include "samd21driver/delay.h"

Pin userled(Pin::Port::A, 28, Pin::Function::GPIO_Output | Pin::Function::High);

void SysTick_Handler() {
    g_ms_ticks++;
}

int main() {
	// configure XOSC@16MHz
	SYSCTRL->XOSC.reg = SYSCTRL_XOSC_STARTUP(0xc) | SYSCTRL_XOSC_GAIN_3 | SYSCTRL_XOSC_XTALEN | SYSCTRL_XOSC_ENABLE;
	while (!(SYSCTRL->PCLKSR.bit.XOSCRDY)) {} // wait until XTAL has started

	// config xosc/16 (div=7) -> fdpll
	SYSCTRL->DPLLCTRLB.reg = SYSCTRL_DPLLCTRLB_DIV(7) | SYSCTRL_DPLLCTRLB_REFCLK_REF1 | SYSCTRL_DPLLCTRLB_FILTER_DEFAULT;
	SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDRFRAC(0) | SYSCTRL_DPLLRATIO_LDR(47); // fref * (LDR + 1 + LDFRAC/16)
	SYSCTRL->DPLLCTRLA.reg = SYSCTRL_DPLLCTRLA_ENABLE;
	while (!(SYSCTRL->DPLLSTATUS.bit.LOCK)) {} // wait for the pll to be locked/ready

	NVMCTRL->CTRLB.bit.RWS = 1; // introduce 1 read wait state which is neccessary if FCPU > 24MHz

	// config fdpll -> gclk0/mainclk -> 48MHz
	GCLK->GENDIV.reg  = GCLK_GENDIV_ID(0) | GCLK_GENDIV_DIV(0);
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC_FDPLL | GCLK_GENCTRL_GENEN;
	while (GCLK->STATUS.bit.SYNCBUSY) {}

	SysTick_Config(48000000 / 1000); // 1 ms
    NVIC_EnableIRQ(SysTick_IRQn);

	while (true) {
		userled.toggle();
		delay_ms(1000);
	}

	return 0;
}
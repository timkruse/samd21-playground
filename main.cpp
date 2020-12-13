#include "board.h"

int main(){

	GCLK_GENDIV_Type gclk_gendiv_config = {.reg = 0};
	gclk_gendiv_config.bit.ID = 0; // use gclk0 as clock source
	gclk_gendiv_config.bit.DIV = 1; // ... gclk0 divided by 1
	GCLK->GENDIV.reg = gclk_gendiv_config.reg;

	GCLK_GENCTRL_Type gclk_genctrl_config = {.reg = 0};
	gclk_genctrl_config.bit.ID = 0;
	gclk_genctrl_config.bit.GENEN = true;
	GCLK->GENCTRL.reg = gclk_genctrl_config.reg;

	while(true){

	}

	return 0;
}
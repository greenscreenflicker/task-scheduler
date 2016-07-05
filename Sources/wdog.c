#include <stdio.h>
#include "MK22F51212.h"
#include "wdog.h"
#include "mcu_tracer.h"
#include "taskcall.h"

taskcall_t task_watchdog_reset={"wdog",0,NULL,&wdog_refresh_func,NULL,NULL};

void wdog_init(void){
    uint32_t value = 0U;

    value = WDOG_STCTRLH_WDOGEN(1) //enable watchdog
    		| WDOG_STCTRLH_CLKSRC(0) //0=lpo=1kHz
			| WDOG_STCTRLH_IRQRSTEN(0) //0
			| WDOG_STCTRLH_WINEN(0) //0=window comparator disabled
			| WDOG_STCTRLH_ALLOWUPDATE(0)
			| WDOG_STCTRLH_DBGEN(0) | //watchdog disabled in debug mode
            WDOG_STCTRLH_STOPEN(1) | //watchdog 0=disabled in stop mode
            WDOG_STCTRLH_WAITEN(1) | //watchdog 0=disabled in wait mode
            WDOG_STCTRLH_DISTESTWDOG(1U) //disable test
			| 0x0100U; //bit required internally

	__disable_irq();
	// WDOG->UNLOCK: WDOGUNLOCK=0xC520
	WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520);
	// WDOG->UNLOCK: WDOGUNLOCK=0xD928
	WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928);
	__asm__("NOP"); // We need to wait one cycle
	WDOG->PRESC = 0;
	WDOG->WINH = 0;
	WDOG->WINL = 0;
	WDOG->TOVALH = 0;
    WDOG->TOVALL = 5000;

	//WDOG->TOVALH=0;
	//WDOG->TOVALL=2000;
	WDOG->STCTRLH=value;

	__enable_irq();
}
/* WARNING: DO NOT CALL THIS FUNCTION TWICE inbetween one bus clock cycle
 *
 * We have here a watchdog clock from lpo (1kHz), so wait at least 2usec
 * before calling again the reset function.
 *
 * If you call again, the reset will be ignored; ignoring several resets
 * leads to undesired watchdog timeouts. Therefore the reset here is
 * done by an individual task.
 * */

void wdog_refresh(void){
	__disable_irq();
	WDOG->REFRESH=0xA602;
	WDOG->REFRESH=0xB480;
	__enable_irq();

}

void wdog_refresh_func(int64_t delay){
	if(tracer_green){
		wdog_refresh();
		//mcu_tracer_msg("WDOG RESET");
	}
	_taskcall_task_register_time(&task_watchdog_reset,(120000000)*2-delay);
}


/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include <stdio.h>
#include "MK22F51212.h"
#include "uart1.h"
#include "mcu_tracer.h"
#include "leds.h"
#include "taskcall.h"
#include "wdog.h"
#include "lowvoltagewarning.h"

//delte

void task_toogle_blue_led(int64_t delay);
void task_toogle_red_led(int64_t delay);
void task_toogle_green_led(int64_t delay);
// Wir definieren hier die Tasks die registriert werden sollen (holen uns also hier den memory)
taskcall_t blink_blue ={"blue_b",0,NULL,&task_toogle_blue_led,NULL,NULL};
taskcall_t blink_red  ={"red_b",0,NULL,&task_toogle_red_led,NULL,NULL};
taskcall_t blink_green={"green_b",0,NULL,&task_toogle_green_led,NULL,NULL};


/*taskcall_t blink_green={"green_b",5,NULL,NULL,NULL,NULL};
taskcall_t blink_red={"red_b",17,NULL,NULL,NULL,NULL};
taskcall_t blink_pink={"pink",10,NULL,NULL,NULL,NULL};
taskcall_t blink_yel1={"yel1",14,NULL,NULL,NULL,NULL};
taskcall_t blink_yel2={"yel2",14,NULL,NULL,NULL,NULL};*/

// Simple delay - not for real programs!
void delay(void) {
   volatile unsigned long i;
   for (i=4800000; i>0; i--) {
      __asm__("nop");
   }
}

/* Example use of interrupt handler
 *
 * The standard ARM libraries provide basic support for the system timer
 * This function is used for the System Timer interrupt handler.
 *
 */

void SysTick_Handler(void) {
	//blueLedToggle();

_taskcall_exec();
	//greenLedToggle();
}

void task_toogle_blue_led(int64_t delay){
	if(tracer_blue){
		blue_toggle();
	}else{
		blue_off();
	}
	_taskcall_task_register_time(&blink_blue,10000000-delay);
}

void task_toogle_red_led(int64_t delay){
	//redLedToggle();
	if(tracer_red){
		red_toogle();
	}else{
		red_off();
	}
	_taskcall_task_register_time(&blink_red, 20000000-delay);
}


void task_toogle_green_led(int64_t delay){
	if(tracer_green){
		green_toggle();
	}else{
		green_off();
	}
	_taskcall_task_register_time(&blink_green,30000000-delay);
}

int main(void) {
	SystemCoreClockUpdate();
	lvd_init();
	init_leds();
	mcu_tracer_config();
	startup_reason_report();

	// printf("SystemBusClock  = %ld\n", SystemBusClock);
	//printf("SystemCoreClock = %ld\n", SystemCoreClock);
	tracer_green=1;
	_taskcall_task_register(&blink_blue);
	_taskcall_task_register(&blink_red);
	_taskcall_task_register(&blink_green);
	_taskcall_task_register_time(&task_watchdog_reset,(120000000*2));
	_taskcall_start();

   while(1){
	   //put here low priority tasks, like communication
       mcu_tracer_process();
       mainloop_iterations=mainloop_iterations+1;
       if(mainloop_iterations>1000000) mainloop_iterations=0;
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

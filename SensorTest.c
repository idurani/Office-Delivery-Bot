#include <inits_tiva.h>
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "math.h"
#include "open_interface.h"
#include "sensors.h"

int main(void)
{
	oi_t *data = oi_alloc();
	oi_init(data);
	while(1)
	{
		oi_update(data);
		printf("Left cliff: %d\n",data->cliffLeftSignal);
		timer_waitMillis(100);
	}
}

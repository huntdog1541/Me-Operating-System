#ifndef TIMER_H
#define TIMER_H

#include "types.h"
#include "pit.h"
#include "isr.h"
#include "utility.h"
#include "screen.h"

void timer_callback(registers_t* regs);
void sleep(uint32 _time);
uint32 millis();						// returns the milliseconds since start (= ticks / PIT frequency)

extern volatile uint32 ticks;			// ticks since computer start (except the few milliseconds needed to boot and prepare interrupts)
extern uint32 frequency;				// PIT frequency

#endif
/*
 * TimerDynamicMemory.h
 *
 * TimerDynamicMemory - A timer malloc library for Arduino.
 * Author: Mariano@pollio.it
 * Copyright (c) 2016 individual Software Italy
 *
 * This library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation; either version 3.0 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser
 * General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef TIMERDYNAMICMEMORY_H
#define TIMERDYNAMICMEMORY_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

typedef void (*timer_callback)(void);

// maximum number of timers
#define MAX_TIMERS 10

class TimerDynamicMemory {

public:

    // setTimer() constants
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;

    // constructor
    TimerDynamicMemory();

    // this function must be called inside loop()
    void run();

	//deallocate the previously allocated space
    void freeMemory();
	
    // call function f every d milliseconds
    int setInterval(long d, timer_callback f);
	
    // call function f once after d milliseconds
    int setTimeout(long d, timer_callback f);

    // call function f every d milliseconds for n times
    int setTimer(long d, timer_callback f, int n);

    // destroy the specified timer
    void deleteTimer(int numTimer);

    // restart the specified timer
    void restartTimer(int numTimer);

    // returns true if the specified timer is enabled
    boolean isEnabled(int numTimer);

    // enables the specified timer
    void enable(int numTimer);

    // disables the specified timer
    void disable(int numTimer);

    // enables the specified timer if it's currently disabled,
    // and vice-versa
    void toggle(int numTimer);

    // returns the number of used timers
    int getNumTimers();

    // returns the number of available timers
    int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };

private:
    // deferred call constants
    const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;      // call the callback function and delete the timer

    // create timer malloc or realloc
    int createTimerSlot();
	
	// check first timer free
	int checkTimerFree();

    int numTimers;
	
	typedef struct rectimer {
      boolean enabled;
      timer_callback callbacks;                   // if the callback pointer is zero, the slot is free, i.e. doesn't "contain" any timer
      unsigned long prev_millis;
      byte numRuns;    // max 255
      byte maxNumRuns; // max 255
	  unsigned long long delays;        
	  byte toBeCalled;   // max 255
    }recTimer;
	
	rectimer *pRecTimer = NULL;

	//recTimer aRecTimer[MAX_TIMERS];
};

#endif
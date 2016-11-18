/*
 * TimerDynamicMemory.cpp
 *
 * Timer - A timer library with malloc e realloc (I like free memory :-) ) for Arduino.
 * Author: mariano@pollio.it
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
 */

#include "TimerDynamicMemory.h"

static inline unsigned long elapsed() { return millis(); }

unsigned long currentMillis = 0;

TimerDynamicMemory::TimerDynamicMemory() {
   currentMillis = elapsed();
}

void TimerDynamicMemory::run() {
    int i;
    unsigned long currentMillis;

    // tempo corrente
    currentMillis = elapsed();

    for (i = 0; i < numTimers; i++) {

        pRecTimer[i].toBeCalled = DEFCALL_DONTRUN;

        if (pRecTimer[i].callbacks) {

            if (currentMillis - pRecTimer[i].prev_millis >= pRecTimer[i].delays) {

                pRecTimer[i].prev_millis += pRecTimer[i].delays;
                // check if the timer callback has to be executed
                if (pRecTimer[i].enabled) {
                    // "run forever" timers must always be executed
                    if (pRecTimer[i].maxNumRuns == RUN_FOREVER) {
                        pRecTimer[i].toBeCalled = DEFCALL_RUNONLY;
                    }
                    // other timers get executed the specified number of times
                    else if (pRecTimer[i].numRuns < pRecTimer[i].maxNumRuns) {
                        pRecTimer[i].toBeCalled = DEFCALL_RUNONLY;
                        pRecTimer[i].numRuns++;

                        // after the last run, delete the timer
                        if (pRecTimer[i].numRuns >= pRecTimer[i].maxNumRuns) {
                            pRecTimer[i].toBeCalled = DEFCALL_RUNANDDEL;
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < numTimers; i++) {
        switch(pRecTimer[i].toBeCalled) {
            case DEFCALL_DONTRUN:
                break;

            case DEFCALL_RUNONLY:
                (*pRecTimer[i].callbacks)();
                break;

            case DEFCALL_RUNANDDEL:
                (*pRecTimer[i].callbacks)();
                deleteTimer(i);
                break;
        }
    }
}

// check one slot free
int TimerDynamicMemory::checkTimerFree() {

    // all slots are used
    if (numTimers >= MAX_TIMERS) {
        return -1;
    }
   
   for ( int i = 0; i < numTimers; i++) {
		
        if (pRecTimer[i].callbacks == 0) {
			// slot free
            return i;
        }
    }
	return -1;
}

// Create slot timer malloc and realloc. Return -1 if not found
int TimerDynamicMemory::createTimerSlot() {
    int i=0;
	
	if(numTimers == 0)
	{
      if ((pRecTimer = (recTimer *)malloc(sizeof(recTimer))) == NULL) {
           return -1; 
	}  
	memset(&pRecTimer[0],sizeof(recTimer),0);
   }
   else
   {
	   
      if ((pRecTimer = (recTimer *)realloc(pRecTimer,sizeof(recTimer) * (numTimers+1))) == NULL) {
           return -1; 
	  }   
	    
	  memset(&pRecTimer[numTimers],sizeof(recTimer),0);
   }

   // setto campi
   pRecTimer[numTimers].prev_millis = currentMillis;
   pRecTimer[numTimers].callbacks = 0;
   
   numTimers++;

   return (numTimers-1);   
}

// set timer
int TimerDynamicMemory::setTimer(long d, timer_callback f, int n) {
    int allocTimer=0;

    if (f == NULL) {
        return -1;
    }

// check timer free	
	if ((allocTimer=checkTimerFree()) < 0) {
		// is not free create timer malloc or realloc
		    allocTimer = createTimerSlot();
            if (allocTimer < 0) {
               return -1;
            }
    }

    pRecTimer[allocTimer].delays = d;
    pRecTimer[allocTimer].callbacks = f;
    pRecTimer[allocTimer].maxNumRuns = n;
    pRecTimer[allocTimer].enabled = true;
    pRecTimer[allocTimer].prev_millis = elapsed();

    return allocTimer;
}

int TimerDynamicMemory::setInterval(long d, timer_callback f) {
    return setTimer(d, f, RUN_FOREVER);
}

int TimerDynamicMemory::setTimeout(long d, timer_callback f) {
    return setTimer(d, f, RUN_ONCE);
}

void TimerDynamicMemory::deleteTimer(int timerId) {
    if (timerId >= numTimers) {
        return;
    }

    // nothing to delete if no timers are in use
    if (numTimers == 0) {
        return;
    }

    // don't decrease the number of timers if the
    // specified slot is already empty
    if (pRecTimer[timerId].callbacks != NULL) {
        pRecTimer[timerId].callbacks = 0;
        pRecTimer[timerId].enabled = false;
        pRecTimer[timerId].toBeCalled = DEFCALL_DONTRUN;
        pRecTimer[timerId].delays = 0;
        pRecTimer[timerId].numRuns = 0;
    }
}

// free memory all timer and reset timer
void TimerDynamicMemory::freeMemory() {
	// this struct is private
	// clear all timer
	memset(pRecTimer,sizeof(recTimer)*numTimers,0);
	numTimers=0;
	free(pRecTimer);
}

void TimerDynamicMemory::restartTimer(int numTimer) {
    if (numTimer >= numTimers) {
        return;
    }

    pRecTimer[numTimer].prev_millis = elapsed();
}

boolean TimerDynamicMemory::isEnabled(int numTimer) {
    if (numTimer >= numTimers) {
        return false;
    }

    return pRecTimer[numTimer].enabled;
}


void TimerDynamicMemory::enable(int numTimer) {
    if (numTimer >= numTimers) {
        return;
    }

    pRecTimer[numTimer].enabled = true;
}

void TimerDynamicMemory::disable(int numTimer) {
    if (numTimer >= numTimers) {
        return;
    }

    pRecTimer[numTimer].enabled = false;
}


void TimerDynamicMemory::toggle(int numTimer) {
    if (numTimer >= numTimers) {
        return;
    }

    pRecTimer[numTimer].enabled = !pRecTimer[numTimer].enabled;
}

int TimerDynamicMemory::getNumTimers() {
    return numTimers;
}
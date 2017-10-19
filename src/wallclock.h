// sparselizard - Copyright (C) 2017-2018 A. Halbach and C. Geuzaine, University of Liege
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to <alexandre.halbach at ulg.ac.be>.


#ifndef WALLCLOCK_H
#define WALLCLOCK_H

#include <iostream>
#include <chrono>

class wallclock
{
	private:

		std::chrono::steady_clock::time_point starttime;
        
        double accumulator = 0;
        
        bool ispaused = false;
        
	public:
	
		// The constructor acts like 'tic':
		wallclock(void);
		// Reset clock:
		void tic(void);
		// Returns time since previous tic or object creation in nanoseconds.
        // Takes into account any accumulated time via pause/resume.
		double toc(void);
		// Prints the time since previous tic or object creation in an adapted unit:
		void print(void);
        
        // Pause the clock:
        void pause(void);
        // Resume the clock:
        void resume(void);
};

#endif

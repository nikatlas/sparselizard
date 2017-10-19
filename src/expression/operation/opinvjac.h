// sparselizard - Copyright (C) 2017-2018 A. Halbach and C. Geuzaine, University of Liege
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to <alexandre.halbach at ulg.ac.be>.


#ifndef OPINVJAC_H
#define OPINVJAC_H

#include "operation.h"

class opinvjac: public operation
{

	private:
        
        int myrow, mycol;
	
	public:
        
        // Define as invjac(m,n):
        opinvjac(int m, int n) { myrow = m; mycol = n; };
        
        std::vector<std::vector<densematrix>> interpolate(elementselector& elemselect, std::vector<double>& evaluationcoordinates, expression* meshdeform);
        densematrix multiharmonicinterpolate(int numtimeevals, elementselector& elemselect, std::vector<double>& evaluationcoordinates, expression* meshdeform);

        std::shared_ptr<operation> copy(void);
        
        void print(void);
        
};

#endif

// sparselizard - Copyright (C) 2017-2018 A. Halbach and C. Geuzaine, University of Liege
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to <alexandre.halbach at ulg.ac.be>.


#ifndef FORMULATION_H
#define FORMULATION_H

#include <iostream>
#include <vector>
#include "integration.h"
#include "contribution.h"
#include "dofmanager.h"
#include "vec.h"
#include "mat.h"
#include "expression.h"
#include "mathop.h"
#include "universe.h"
#include "memory.h"
#include "densematrix.h"
#include "intdensematrix.h"
#include "rawvec.h"
#include "rawmat.h"

class contribution;

class formulation
{

	private:
	
        // myvec is the right handside vector rhs.
        shared_ptr<rawvec> myvec = NULL;
        // - mymat[0] is the stiffness matrix K
        // - mymat[1] is the damping matrix C
        // - mymat[2] is the mass matrix M
        std::vector<shared_ptr<rawmat>> mymat = {NULL, NULL, NULL};
        
		// The link between the dof number and its row and column in the matrix:
		shared_ptr<dofmanager> mydofmanager;
		
		// mycontributions[m][i][j] gives the jth contribution of block number i for:
        // - the right handside if     m = 0
        // - the stiffness matrix if   m = 1
        // - the damping matrix if     m = 2
        // - the mass matrix if        m = 3
		std::vector< std::vector<std::vector<contribution>> > mycontributions = {{}, {}, {}, {}};
                
        // Always call this generate from the public generate functions:
        void generate(int m, int contributionnumber);
        
	public:
        
        // Has this formulation been called to compute a constraint?
        bool isconstraintcomputation = false;
	
        
        formulation(void);
        
		// The following adds the contribution defined in the integration object.
		void operator+=(integration integrationobject);
		
        // Generate all blocks:
        void generate(void);
        void generate(std::vector<int> contributionnumbers);
		void generate(int contributionnumber);
        
        shared_ptr<dofmanager> getdofmanager(void) { return mydofmanager; };
        
        
        // Get the assembled matrices or get the right hanside vector.
        // Choose to discard or not all values after getting the vector/matrix.
        
        // b() is an alias for rhs() and A() for K():
        vec b(bool keepvector = false);
        mat A(bool keepfragments = false);
        
        vec rhs(bool keepvector = false);
        mat K(bool keepfragments = false);
        mat C(bool keepfragments = false);
        mat M(bool keepfragments = false);
        // KCM set to 0 gives K, 1 gives C and 2 gives M.
        mat getmatrix(int KCM, bool keepfragments = false);
        
};



#endif

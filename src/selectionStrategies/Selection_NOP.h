/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMSELECTION_NOP_H__
#define __NOXIMSELECTION_NOP_H__

#include "SelectionStrategy.h"
#include "SelectionStrategies.h"
#include "../Router.h"

using namespace std;

class Selection_NOP : SelectionStrategy {
	public:
        int apply(Router * router, const vector < int >&directions, const RouteData & route_data);
        void perCycleUpdate(Router * router);

		static Selection_NOP * getInstance();

	private:
		Selection_NOP(){};
		~Selection_NOP(){};

		static Selection_NOP * selection_NOP;
		static SelectionStrategiesRegister selectionStrategiesRegister;
};

#endif

/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMSELECTION_RANDOM_H__
#define __NOXIMSELECTION_RANDOM_H__

#include "SelectionStrategy.h"
#include "SelectionStrategies.h"
#include "../Router.h"

using namespace std;

class Selection_RANDOM : SelectionStrategy {
	public:
        int apply(Router * router, const vector < int >&directions, const RouteData & route_data);
        void perCycleUpdate(Router * router);

		static Selection_RANDOM * getInstance();

	private:
		Selection_RANDOM(){};
		~Selection_RANDOM(){};

		static Selection_RANDOM * selection_RANDOM;
		static SelectionStrategiesRegister selectionStrategiesRegister;
};

#endif

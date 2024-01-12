/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMSELECTIONSTRATEGY_H__
#define __NOXIMSELECTIONSTRATEGY_H__

#include <vector>
#include "../DataStructs.h"
#include "../Utils.h"

using namespace std;

struct Router;

class SelectionStrategy
{
	public:
        virtual int apply(Router * router, const vector < int >&directions, const RouteData & route_data) = 0;
        virtual void perCycleUpdate(Router * router) = 0;
};

#endif

/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMROUTINGALGORITHM_H__
#define __NOXIMROUTINGALGORITHM_H__

#include <vector>
#include "../DataStructs.h"
#include "../Utils.h"

using namespace std;

struct Router;

class RoutingAlgorithm
{
	public:
		virtual vector<int> route(Router * router, const RouteData & routeData) = 0;
};

#endif

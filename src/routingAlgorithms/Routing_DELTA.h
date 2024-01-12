/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMROUTING_DELTA_H__
#define __NOXIMROUTING_DELTA_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"
#define name() "Routing_DELTA"

using namespace std;

class Routing_DELTA : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_DELTA * getInstance();

	private:
		Routing_DELTA(){};
		~Routing_DELTA(){};

		static Routing_DELTA * routing_DELTA;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif

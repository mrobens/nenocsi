/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMROUTING_DYAD_H__
#define __NOXIMROUTING_DYAD_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_DYAD : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_DYAD * getInstance();

	private:
		Routing_DYAD(){};
		~Routing_DYAD(){};

		static Routing_DYAD * routing_DYAD;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
        static RoutingAlgorithm * odd_even;
};

#endif

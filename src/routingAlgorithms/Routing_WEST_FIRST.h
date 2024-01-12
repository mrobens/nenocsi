/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMROUTING_WEST_FIRST_H__
#define __NOXIMROUTING_WEST_FIRST_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_WEST_FIRST : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_WEST_FIRST * getInstance();

	private:
		Routing_WEST_FIRST(){};
		~Routing_WEST_FIRST(){};

		static Routing_WEST_FIRST * routing_WEST_FIRST;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
        static RoutingAlgorithm * xy;
};

#endif

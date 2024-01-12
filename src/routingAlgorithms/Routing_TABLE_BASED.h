/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMROUTING_TABLE_BASED_H__
#define __NOXIMROUTING_TABLE_BASED_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

#include "../GlobalRoutingTable.h"

using namespace std;

class Routing_TABLE_BASED : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_TABLE_BASED * getInstance();

        inline string name() { return "Routing_TABLE_BASED";};

	private:
		Routing_TABLE_BASED(){};
		~Routing_TABLE_BASED(){};

		static Routing_TABLE_BASED * routing_TABLE_BASED;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif

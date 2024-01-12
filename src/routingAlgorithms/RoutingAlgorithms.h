/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#ifndef __NOXIMROUTINGALGORITHMS_H__
#define __NOXIMROUTINGALGORITHMS_H__

#include <map>
#include <string>
#include "RoutingAlgorithm.h"

using namespace std;

typedef map<string, RoutingAlgorithm * > RoutingAlgorithmsMap;

class RoutingAlgorithms {
	public:
		static RoutingAlgorithmsMap * routingAlgorithmsMap;
		static RoutingAlgorithmsMap * getRoutingAlgorithmsMap();

		static RoutingAlgorithm * get(const string & routingAlgorithmName);
};

struct RoutingAlgorithmsRegister : RoutingAlgorithms {
	RoutingAlgorithmsRegister(const string & routingAlgorithmName, RoutingAlgorithm * routingAlgorithm) {
		getRoutingAlgorithmsMap()->insert(make_pair(routingAlgorithmName, routingAlgorithm));
	}
};

#endif

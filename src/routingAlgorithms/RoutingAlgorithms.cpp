/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#include "RoutingAlgorithms.h"

RoutingAlgorithmsMap * RoutingAlgorithms::routingAlgorithmsMap = 0;

RoutingAlgorithm * RoutingAlgorithms::get(const string & routingAlgorithmName) {
	RoutingAlgorithmsMap::iterator it = getRoutingAlgorithmsMap()->find(routingAlgorithmName);

	if(it == getRoutingAlgorithmsMap()->end())
		return 0;

	return it->second;
}

RoutingAlgorithmsMap * RoutingAlgorithms::getRoutingAlgorithmsMap() {
	if(routingAlgorithmsMap == 0) 
		routingAlgorithmsMap = new RoutingAlgorithmsMap();
	return routingAlgorithmsMap; 
}

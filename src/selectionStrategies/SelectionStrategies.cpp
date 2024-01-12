/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 */
#include "SelectionStrategies.h"

SelectionStrategiesMap * SelectionStrategies::selectionStrategiesMap = 0;

SelectionStrategy * SelectionStrategies::get(const string & selectionStrategyName) {
	SelectionStrategiesMap::iterator it = getSelectionStrategiesMap()->find(selectionStrategyName);

	if(it == getSelectionStrategiesMap()->end())
		return 0;

	return it->second;
}

SelectionStrategiesMap * SelectionStrategies::getSelectionStrategiesMap() {
	if(selectionStrategiesMap == 0) 
		selectionStrategiesMap = new SelectionStrategiesMap();
	return selectionStrategiesMap; 
}

#include "FindDataHazards.h"
using namespace scpar;

FindDataHazards::FindDataHazards(SplitWaitBlocks::segIDVarMapType segIDVarMap, SplitWaitBlocks::entryFunctionSegIDMapType entryFunctionSegIDMap, Model::moduleMapType moduleMap)
:_segIDVarMap(segIDVarMap)
,_entryFunctionSegIDMap(entryFunctionSegIDMap)
,_moduleMap(moduleMap)
,_WAWHazards(0)
,_RAWHazards(0)
,_WARHazards(0){
	
	_dataHazardTable = new bool*[_segIDVarMap.size()];
	for (int i = 0; i<_segIDVarMap.size(); i++) {
		_dataHazardTable[i] = new bool[_segIDVarMap.size()];
	}
}

FindDataHazards::~FindDataHazards() {
	
}

void FindDataHazards::fillHazardTable(int toCompareSegID, vector<SplitWaitBlocks::varTypePair> readAccessVariables, vector<SplitWaitBlocks::varTypePair> writeAccessVariables) {
	
	SpltiWaitBlocks::segIDVarMapType::iterator foundEntry = _segIDVarMap.find(toCompareSegID);
	SplitWaitBlocks::accessVectorPairType toComare = foundEntry->second;
	vector<SplitWaitBlocks::varTypePair> toCompareReadAccessVariables = toCompare.first;
	vector<SplitWaitBlocks::varTypePair> toCompareWriteAccessVariables = toCompare.second;
	
	// 3 iterations read with write, write with write, write with read 
	for (int i = 0; i<toCompareReadAccessVariables.size(); i++) {
		for (int j = 0; j<writeAccessVariables.size(); j++) {
			if(writeAccessVariables.at(j).first == 0) {
				//need timing information
				// update boolean table
			}
		}
	}
}

void FindDataHazards::constructHazardTable() {

	for (SplitWaitBlocks::entryFunctionSegIDMapType::iterator it = _entryFunctionSegIDMap.begin(), eit = _entryFunctionSegIDMap.end();
	it != eit;
	it++) {
		EntryFunctionContainer *currentEntryFunction = it->first;
		vector<int> currentSegIDVector = it->second;
		for (SplitWaitBlocks::segIDVarMapType::iterator sit = _segIDVarMap.begin(), 				seit != _segIDVarMap.end();
		sit != seit;
		sit++) {
			int currentSegID = sit->first;
			bool sameEntryFunction = false;
			for (int i = 0; i<currentSegIDVector.size(); i++) 
			{
				if(currentSegIDVector.at(i) == currentSegID) {
					sameEntryFunction = true;
					break;
				}
			}
			if(!sameEntryFunction) { // perform hazard analysis

				SplitWaitBlocks::accessVectorPairType currentAccessVector = sit->second;
				vector<SplitWaitBlocks::varTypePair> readAccessVariables = currentAccessVector.first;
				vector<SplitWaitBlocks::varTypePair> writeAccessVariables = currentAccessVector.second;
			
				for (int j = 0; j<currentSegIDVector.size(); j++) {
					int toCompareSegID = currentSegIDVector.at(j);
					if(_segIDVarMap.find(toCompareSegID) != _segIDVarMap.end()) {
						fillHazardTable(toCompareSegID, readAccessVariables, writeAccessVariables); //Eden Hazard.. KTBFFH	
					}
				}
			
			}	
			else {
				continue;
			}
		}
	}
}


#ifndef _FIND_SUSPENSION_TIME_H_
#define _FIND_SUSPENSION_TIME_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include "EntryFunctionContainer.h"
#include "SplitWaitBlocks.h"
#include "automata.h"
#include <map>
#include <deque>
namespace scpar {
  
  using namespace clang;
  using namespace std;
	
	class FindSuspensionTime {	
		public:				
			
			FindSuspensionTime(SplitWaitBlocks::entryFunctionGraphMapType, raw_ostream&);
			~FindSuspensionTime();

			typedef pair<unsigned int, unsigned int> timePairType;
			typedef vector<timePairType> timeAdvanceVectorType;

			typedef pair<int, timeAdvanceVectorType> waitBlockIDSuspensionPairType;
			typedef map<int, timeAdvanceVectorType> waitBlockIDSuspensionMapType;

			typedef pair<Edge*, timePairType> edgeSuspensionPairType;
			typedef map<Edge*, timePairType> edgeSuspensionMapType;			
			
			void calculateSuspensionTiming();
			void updateTime(bool);
			bool isTimedWait(Optional <CFGStmt>);
			bool isDeltaWait(Optional <CFGStmt>);
			bool isEventWait(Optional <CFGStmt>);
			
			edgeSuspensionMapType returnEdgeSuspensionMap();
			unsigned int getTimeArg(Optional <CFGStmt>);

			void dump();
						
		private:
			SplitWaitBlocks::entryFunctionGraphMapType _entryFunctionGraphMap;
			edgeSuspensionMapType _edgeSuspensionMap;						
			raw_ostream& _os;
	};

} // End namespace scpar

#endif

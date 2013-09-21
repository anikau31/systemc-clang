#ifndef _FIND_DATA_HAZARDS_H_
#define _FIND_DATA_HAZARDS_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include "EntryFunctionContainer.h"
#include "SplitWaitBlocks.h"
#include "Model.h"
#include "ModuleDecl.h"
#include <map>
#include <deque>
namespace scpar {
  
  using namespace clang;
  using namespace std;
		
	class FindDataHazards {
		public:
			FindDataHazards(SplitWaitBlocks::segIDVarMapType, SplitWaitBlocks::entryFunctionSegIDMapType, Model::moduleMapType);
			~FindDataHazards();
			
			typedef **bool dataHazardTable;
			
			void constructHazardTable();
		private:
			dataHazardTable _dataHazardTable;
			SplitWaitBlocks::segIDVarMapType _segIDVarMap;
			SplitWaitBlocks::entryFunctionSegIDMapType _entryFunctionSegIDMap;
			int _WAWHazards;
			int _RAWHazards;
			int _WARHazards;
	};

} // End namespace scpar

#endif

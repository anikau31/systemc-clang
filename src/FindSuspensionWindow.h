#ifndef _FIND_SUSPENSION_WINDOW_H_
#define _FIND_SUSPENSION_WINDOW_H_

#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include <map>
#include "EntryFunctionContainer.h"
#include <deque>
namespace scpar {
  
  using namespace clang;
  using namespace std;
		
	class VariableAccess{
		public:
			VariableAccess(const string&, bool, bool);
			~VariableAccess();
			bool returnWriteStatus();
			bool returnReadStatus();
			string returnVariableName();

			private:
				string _variableName;
				bool _writeStatus;
				bool _readStatus;
	};


	class SuspensionWindow {
		public:
			SuspensionWindow(CXXMemberCallExpr*, unsigned int, unsigned int, bool, bool);
			SuspensionWindow(CXXMemberCallExpr*, unsigned int, unsigned int, bool);
			~SuspensionWindow();
			
			CXXMemberCallExpr* returnMemberExpr();
			unsigned int returnSimTime();
			unsigned int returnDeltaTime();
			bool returnStatus();
			bool isWaitEvent();

		private:
			CXXMemberCallExpr *_memberExpr;
			unsigned int _simulationTime;
			unsigned int _deltaTime;
			bool _complete;
			bool _eventWait;
	};

  class FindSuspensionWindow {
  	public:
			FindSuspensionWindow(vector<EntryFunctionContainer*>, ASTContext*, raw_ostream&);
			~FindSuspensionWindow();
		
			typedef pair<CXXMemberCallExpr*, SuspensionWindow*> waitNotifyTimeWindowPairType;
			typedef map <CXXMemberCallExpr*, SuspensionWindow*> waitNotifyTimeWindowMapType;

			typedef pair<string, VariableAccess*> variableAccessPairType;
			typedef map<string, VariableAccess*> variableAccessMapType;
	
			typedef pair<string, vector<CXXMemberCallExpr*> > waitsAfterEventPairType;
			typedef map<string, vector<CXXMemberCallExpr*> > waitsAfterEventMapType;

			typedef pair<CXXMemberCallExpr*, vector<VariableAccess*> > waitNotifyVariablesPairType;
			typedef map<CXXMemberCallExpr*, vector<VariableAccess*> > waitNotifyVariablesMapType;
			

			void traverseCFG();
			void addLeftVariable(string);
			void addRightVariable(string);
			void addVariable(string);
			void addVariableAccessType(Stmt *);	
			void updateWaitTime(CXXMemberCallExpr*, unsigned int *, unsigned int *, bool*);
			void updateNotifyTime(CXXMemberCallExpr*, unsigned int *, unsigned int *, bool*);
			CXXMemberCallExpr* isWaitCall(const Stmt*);
			CXXMemberCallExpr* isNotifyCall(const Stmt*);			
			waitNotifyVariablesMapType getWaitVariableMap();
			
			void dump();

		private:

			waitNotifyTimeWindowMapType _waitTimeWindowMap;	
			waitNotifyTimeWindowMapType _notifyTimeWindowMap;
  		vector <EntryFunctionContainer*> _vef;
			waitsAfterEventMapType _waitsAfterEventMap;
			waitNotifyVariablesMapType _waitVariablesMap;
			vector<CXXMemberCallExpr*> _waitsAfterEventVector;
			vector<VariableAccess*> _waitVariables;		
			variableAccessMapType _variableAccessMap;
			CXXMemberCallExpr *currExpr;
			vector<SuspensionWindow*> _cleanNotify;
			CFG* _cfg;
			raw_ostream& _os;
			ASTContext *_a;	

	}; // End class SplitWaitBlocks
   
} // End namespace scpar

#endif

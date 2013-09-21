#ifndef _GRAPH_DEPENDENCY_H_
#define _GRAPH_DEPENDENCY_H_

#include <vector>
#include <deque>
#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/Expr.h"
#include <stdio.h>
namespace scpar {
  using namespace clang;
  using namespace std;
  
  class Task {
  public:

    typedef pair<int, Task* > connectPairType;    
    typedef map<int, Task* > connectMapType;
    
  public:
    Task();
    Task(int);
		
		~Task();

    void addSuccessor(Task* );
    void addPredecessor(Task* );
    vector<int> getSuccessors(int);
    vector<int> getPredecessors(int);
    int getId();
    void dump(raw_ostream&, int);
  protected:
    int _id;
    connectMapType _preds;
    connectMapType _succs;
    
  };
  
  class Connection {
  public:
    Connection(Task*, Task*);
		Connection(int, int);
		Connection(Task*, Task*, int);

    void dump(raw_ostream&, int);
    int getId();

// For generating statements from the automaton

    int getToId();
    int getFromId();
			
		void addDataTransferOverhead(double);
		void addKernelTime(double);
		double getProfilingTime();
			
	protected:
    int _id;
    Task* _from;
    Task* _to;
    double _dataTransferOverhead;
    double _kernelTime; 			
 };
  
 class TaskGraph {
  public:
		
    typedef vector <int> tasks;
    typedef map<int, Task*> taskMapType;
    typedef pair<int, Task*> taskPairType;
   
    typedef map<int, Connection*> connectionMapType;
    typedef pair<int, Connection*> connectionPairType;
  
    typedef pair<int, int> twoIntPairType;
    
    typedef pair< twoIntPairType, Connection* > adjPairType;
    typedef map<pair<int, int>, Connection* > adjMapType;

    typedef pair<int, vector<Connection*> > adjConnectionsPairType;
    typedef map <int, vector<Connection*> > adjConnectionsMapType;

    typedef pair <int , vector<int> > taskStatePair;
    typedef map<int, vector<int> > taskStateMap;
  
  public:
    TaskGraph();
    ~TaskGraph();
    
    Task* addTask();
    Task* addTask(int);
    Connection* addConnection(Task*, Task*);
		Connection* addConnection(Task*, Task*, bool);
    Connection* getConnection(Task*, Task*);
    Connection* getConnection(int, int);
    vector <Connection*> getConnections(int);
		Task* returnTask(int);
		adjMapType returnAdjList();
    tasks returnTaskIds();

		void dump(raw_ostream&, int tabn = 0);
    void dumpTaskGraph(raw_ostream&, int tabn = 0); 
    void Optimization(raw_ostream&);
		double min(double, double);
		bool BFS(deque<int>, double*, double*, int*, int*, raw_ostream&);
		double getProfilingTimems(double);	
	protected:
    adjMapType _adjList;
    taskMapType _taskMap;
    connectionMapType _connectionMap;
    tasks _taskIds;
    adjConnectionsMapType _adjConnections;
    int _nTasks;
    int _nConnections;
  };  
}

#endif

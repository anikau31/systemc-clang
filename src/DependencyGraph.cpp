#include "DependencyGraph.h"
#define BLACK 0
#define WHITE 1
#define GREY 2
#define infinity -1

using namespace scpar;
/// Task class.

Task::Task() 
: _id(-1) {

}

Task::Task(int i) 
: _id(i) {
  
}

Task::~Task() {
	_preds.clear();
	_succs.clear();
}

int Task::getId() {
  return _id;
}

void Task::addSuccessor(Task* s) {
  _succs.insert(Task::connectPairType(s->getId(), s));
  s->addPredecessor(this);
}

void Task::addPredecessor(Task* p) {
  _preds.insert(Task::connectPairType(p->getId(), p));
}

vector<int> Task::getSuccessors(int fromId) {

  vector<int> tmpSuccs;
  
  for (Task::connectMapType::iterator it = _succs.begin(), eit = _succs.end();
	it != eit;
	it++) {
	//	if(it->first == fromId) {
			tmpSuccs.push_back(it->first);
	//	}
	}
	return tmpSuccs;

}

vector<int> Task::getPredecessors(int toId) {

  vector<int> tmpPreds;
  
  for (Task::connectMapType::iterator it = _preds.begin(), eit = _preds.end();
	it != eit;
	it++) {
	//	if(it->first == fromId) {
			tmpPreds.push_back(it->first);
	//	}
	}
	return tmpPreds;

}

void Task::dump(raw_ostream& os, int tabn) {
  os << "Task " << getId() << "\n";
  os << "  preds: ";
  for (Task::connectMapType::iterator it = _preds.begin(), eit = _preds.end();
       it!=eit;
       it++) {
    os << it->first << " ";
  }
  os << "\n  succs: ";
  for (Task::connectMapType::iterator it = _succs.begin(), eit = _succs.end();
       it!=eit;
       it++) {
    os << it->first << " ";
  }
  os << "\n";
}


////////////////////////////////////////////////////////////////////////////////////////
/// Connection class.
Connection::Connection(Task* f, Task* t)
: _id(-1)
, _from(f)
, _to(t) 
, _dataTransferOverhead(0)
, _kernelTime(0) 
{

}

Connection::Connection(Task* f, Task* t, int i)
: _id(i)
, _from(f)
, _to(t) 
, _dataTransferOverhead(0)
, _kernelTime(0) {
  
}

int Connection::getId() {
  return _id;
}

void Connection::dump(raw_ostream& os, int tabn) {
  os << "Connection (" << _from->getId() << "," << _to->getId() << ")\n";
}

int Connection::getToId() {
   return _to->getId();
}

int Connection::getFromId() {
   return _from->getId();
}

void Connection::addDataTransferOverhead(double bytes) {
	_dataTransferOverhead = bytes;
}

void Connection::addKernelTime(double kernelTime) {
	_kernelTime = kernelTime;
}

double Connection::getProfilingTime() {

	double totalTime;

	totalTime = _dataTransferOverhead + _kernelTime;

	return totalTime;
}

////////////////////////////////////////////////////////////////////////////////////////
/// TaskGraph class

TaskGraph::TaskGraph()
: _nTasks(0)
, _nConnections(0) {

}

Task* TaskGraph::addTask() {
  Task* n = new Task(_nTasks);
  _taskMap.insert(TaskGraph::taskPairType(_nTasks,n));
	_taskIds.push_back(_nTasks);   
  ++_nTasks;
  return n;
}

Task * TaskGraph::addTask(int id) {
  Task* n = new Task(id);
  _taskMap.insert(TaskGraph::taskPairType(id,n));
	_taskIds.push_back(_nTasks);	
	++_nTasks;
  return n;
}

Connection* TaskGraph::addConnection(Task* f, Task* t) {
  Connection* e = new Connection(f, t, _nConnections);
  _connectionMap.insert(TaskGraph::connectionPairType(_nConnections,e));

  // Update preds and succs in tasks.
  f->addSuccessor(t);
  ++_nConnections;
  
  // Insert in adjacency list.
  _adjList.insert(TaskGraph::adjPairType(TaskGraph::twoIntPairType(f->getId(), t->getId())
                                     , e));
  return e;
}

Task* TaskGraph::returnTask(int taskID) {
	taskMapType::iterator found = _taskMap.find(taskID);
	return found->second;
}

Connection* TaskGraph::addConnection(Task* f, Task* t, bool edgeDependency) {
  Connection* e = new Connection(f, t, edgeDependency);
  _connectionMap.insert(TaskGraph::connectionPairType(_nConnections,e));

  // Update preds and succs in tasks.
  f->addSuccessor(t);
  ++_nConnections;
  
  // Insert in adjacency list.
  _adjList.insert(TaskGraph::adjPairType(TaskGraph::twoIntPairType(f->getId(), t->getId())
                                     , e));
  return e;
}


Connection* TaskGraph::getConnection(int f, int t) {
  TaskGraph::adjMapType::iterator fit = _adjList.find(TaskGraph::twoIntPairType(f,t));
  if (fit == _adjList.end()) {
    return NULL;
  }
  
  return fit->second;
}

Connection* TaskGraph::getConnection(Task* f, Task* t) {
  TaskGraph::adjMapType::iterator fit = _adjList.find(TaskGraph::twoIntPairType(f->getId(), t->getId()));
  if (fit == _adjList.end()) {
    return NULL;
  }
  
  return fit->second;
}


void TaskGraph::dump(raw_ostream& os, int tabn ) {

  // Print all tasks.
  os << "Task map: " << _nTasks << "\n";
  for (TaskGraph::taskMapType::iterator it = _taskMap.begin(), eit = _taskMap.end();
       it != eit;
       it++){
    
    os <<  it->first << " " << it->second << " " ;
    it->second->dump(os, tabn++);
  }
  
  // Print all Connections.
  os << "Connection map: " << _nConnections << "\n";
  for (TaskGraph::connectionMapType::iterator it = _connectionMap.begin(), eit = _connectionMap.end();
       it != eit;
       it++){
    
    os <<  it->first << " " << it->second << " ";
//    it->second->dump(os, tabn++);
  }

  os << "Adjacency list: " << _adjList.size() << "\n";
  for (TaskGraph::adjMapType::iterator it = _adjList.begin(), eit = _adjList.end();
       it != eit;
       it++) {
    TaskGraph::twoIntPairType p = it->first;
    Connection* e = it->second;
    	
//		e->dump(os,tabn++);
  
    os << "\nConnection (" << p.first << "," << p.second << ") ";		
	}
       
}

TaskGraph::adjMapType TaskGraph::returnAdjList() {
	return _adjList;
}
TaskGraph::tasks TaskGraph::returnTaskIds() {
	return _taskIds;
}
//initialization

	// White = 0
	// Gray = 1
	// Black = 2

double TaskGraph::min (double x, double y) {
	return x<y ? x : y;  // returns minimum of x and y
}

bool TaskGraph::BFS(deque<int> nodes, double* capacity, double* flow, int* color, int* pred, raw_ostream& _os) {

	for (int i = 0; i<_taskIds.size(); i++) {
		color[i] = 0;
	}
	nodes.push_front(0);
	pred[0] = -1;
	while(nodes.size() != 0) {
		int currentNode = nodes.front();
		nodes.pop_front();
		color[currentNode] = 2; //node colored black
		for (int j = 0; j <_taskIds.size(); j++) {
			if(_adjList.find(twoIntPairType(currentNode, j)) != _adjList.end()) { //found an edge
				adjMapType::iterator connectionFound = _adjList.find(twoIntPairType(currentNode, j));
				Connection *edge = connectionFound->second;

				if((color[j] == 0)&&(capacity[currentNode*_taskIds.size() + j] - flow[currentNode*_taskIds.size() + j] > 0)) { // to check if edge is augmenting edge

					nodes.push_back(j);
					pred[j] = currentNode;
					color[j] = 1;
				}
			}
		}
	}
	if(color[1] == 2) {	
		return true;
	}
	else {
		return false;
	}
}

void TaskGraph::Optimization(raw_ostream& os) {

	int N = _taskIds.size();
	double maxFlow;
	double capacity[N*N];
	double flow[N*N];
	int color[N];
	int pred[N];
	deque <int> nodes;

	maxFlow = 0;

	// taskIds size is 0. Hence you are dumbass
	// setting flow to zero

	for (int i = 0 ; i<_taskIds.size(); i++) {
		for (int j = 0; j < _taskIds.size(); j++) {
			flow[i*_taskIds.size() + j] = 0;
		}
	}

	// extracting capacities
	for (int i = 0; i<_taskIds.size(); i++) {
		for (int j = 0; j<_taskIds.size(); j++) {
			if(_adjList.find(twoIntPairType(i, j)) != _adjList.end()){
				TaskGraph::adjMapType::iterator connectionFound = _adjList.find(twoIntPairType(i, j));
				Connection *edge = connectionFound->second;
				double edgeCapacity = edge->getProfilingTime();
				os <<"\n For connection between " <<i<< " and " <<j <<" " 
				<<edgeCapacity;
				capacity[i*_taskIds.size() + j] = edgeCapacity;	
			}
			else {
				capacity[i*_taskIds.size() + j] = 0;
			}
		}
	}

	while(BFS(nodes, capacity, flow, color, pred, os)) {

		double increment = 1e+30;	
		for (int u = 1; pred[u] >= 0; u = pred[u]) {
			increment = min(increment, capacity[pred[u]*_taskIds.size() + u]-flow[pred[u]*_taskIds.size() + u]);
		}
		for(int u = 1; pred[u] >=0; u = pred[u]) {
			flow[pred[u]*_taskIds.size() + u] +=increment;
			flow[u*_taskIds.size() + pred[u]] -= increment;
		}
		maxFlow +=increment;

	}

	// need to find cutset
	vector <Connection *> cutsetConnections;
	for (int i = 0; i<_taskIds.size(); i++) {
		color[i] = 0;
	}

	nodes.push_front(0);
	while(nodes.size() != 0) {
		int currentNode = nodes.front();
		nodes.pop_front();
		color[currentNode] = 2; //node colored black
		for (int j = 0; j <_taskIds.size(); j++) {
			if(_adjList.find(twoIntPairType(currentNode, j)) != _adjList.end()) { //found an edge
				adjMapType::iterator connectionFound = _adjList.find(twoIntPairType(currentNode, j));
				Connection *edge = connectionFound->second;
				if((color[j] == 0)&&(capacity[currentNode*_taskIds.size() + j] - flow[currentNode*_taskIds.size() + j] > 0)) { // to check if edge is augmenting edge
					nodes.push_back(j);
					pred[j] = currentNode;
					color[j] = 1;
				}
				
				else {
					if(flow[currentNode*_taskIds.size() + j] > 0){
						bool connectionDuplicate = false;
						for (int k = 0; k<cutsetConnections.size(); k++) {
							if(cutsetConnections.at(k) == edge) {
								connectionDuplicate = true;
								break;
							}
						}
						cutsetConnections.push_back(edge);
					}
					
					else if(currentNode == 0) {
						nodes.push_back(j);
					}
				}
			}
		}
	}

	for (int i = 0; i<_taskIds.size(); i++) {
		for (int j = 0; j<_taskIds.size(); j++) {
			if(_adjList.find(twoIntPairType(i, j)) != _adjList.end()) {
				adjMapType::iterator connectionFound = _adjList.find(twoIntPairType(i, j));
				for (int k = 0; k<cutsetConnections.size(); k++) {
					if(connectionFound->second == cutsetConnections.at(k)) {
						os <<"\n Connection between : " <<i <<" and "<<j<<" in the cutset";
					}
				}
			}
		}
	}
}	

TaskGraph::~TaskGraph() {
// Responsible for cleaning everything up. 
  for (TaskGraph::taskMapType::iterator it = _taskMap.begin(), eit = _taskMap.end();
       it != eit;
       it++){
  
    delete it->second;
//    os <<  it->first << " " << it->second;
//    it->second->dump(os, tabn++);
  }
  _taskMap.clear();
  
  // Print all Connections.
  for (TaskGraph::connectionMapType::iterator it = _connectionMap.begin(), eit = _connectionMap.end();
       it != eit;
       it++){

    delete it->second;
  }
 
 _connectionMap.clear(); 
 _adjList.clear();
 for (TaskGraph::adjConnectionsMapType::iterator it = _adjConnections.begin(), eit = _adjConnections.end();
			it != eit;
			it++) {
		for (int i = 0; i<it->second.size(); i++) {
			delete it->second.at(i);
		}
	}
	_adjConnections.clear();	 
}

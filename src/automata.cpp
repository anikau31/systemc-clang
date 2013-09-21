#include "automata.h"

using namespace scpar;
/// Node class.

Node::Node() 
: _id(-1) {

}

Node::Node(int i) 
: _id(i) {
  
}

int Node::getId() {
  return _id;
}

void Node::addSuccessor(Node* s) {
  _succs.insert(Node::connectPairType(s->getId(), s));
  s->addPredecessor(this);
}

void Node::addPredecessor(Node* p) {
  _preds.insert(Node::connectPairType(p->getId(), p));
}

vector<int> Node::getSuccessors(int fromId) {

  vector<int> tmpSuccs;
  
  for (Node::connectMapType::iterator it = _succs.begin(), eit = _succs.end();
	it != eit;
	it++) {
	//	if(it->first == fromId) {
			tmpSuccs.push_back(it->first);
	//	}
	}
	return tmpSuccs;

}

vector<int> Node::getPredecessors(int toId) {

  vector<int> tmpPreds;
  
  for (Node::connectMapType::iterator it = _preds.begin(), eit = _preds.end();
	it != eit;
	it++) {
	//	if(it->first == fromId) {
			tmpPreds.push_back(it->first);
	//	}
	}
	return tmpPreds;

}





void Node::dump(raw_ostream& os, int tabn) {
  os << "Node " << getId() << "\n";
  os << "  preds: ";
  for (Node::connectMapType::iterator it = _preds.begin(), eit = _preds.end();
       it!=eit;
       it++) {
    os << it->first << " ";
  }
  os << "\n  succs: ";
  for (Node::connectMapType::iterator it = _succs.begin(), eit = _succs.end();
       it!=eit;
       it++) {
    os << it->first << " ";
  }
  os << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////
/// Edge class.
Edge::Edge(Node* f, Node* t)
: _id(-1)
, _from(f)
, _to(t) {

}

Edge::Edge(Node *f, Node *t, int i, vector<CFGBlock*> stmtBlocks)
:_id(i)
, _from(f)
, _to(t)
, _preStmtBlks(stmtBlocks)
{
}

Edge::Edge(Node* f, Node* t, int i)
: _id(i)
, _from(f)
, _to(t) {
  
}

Edge::Edge(Node *f, Node*t , int i, vector<CFGBlock*> stmtBlks, Optional <CFGStmt> stmt) 
: _id(i)
, _from(f)
, _to(t)
, _preStmtBlks(stmtBlks)
, _waitStmt(stmt) {

}

int Edge::getId() {
  return _id;
}

void Edge::dump(raw_ostream& os, int tabn) {
  os << "Edge (" << _from->getId() << "," << _to->getId() << ")\n";
}

int Edge::getToId() {
   return _to->getId();
}

int Edge::getFromId() {
   return _from->getId();
}

vector<CFGBlock*> Edge::getPreStmtBlks() {
   return _preStmtBlks;
}

Optional <CFGStmt> Edge::getWaitStmt() {
	return _waitStmt;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Graph class

Graph::Graph()
: _nNodes(0)
, _nEdges(0) {

}

Node* Graph::addNode() {
  Node* n = new Node(_nNodes);
  _nodeMap.insert(Graph::nodePairType(_nNodes,n));
   
  ++_nNodes;
  return n;
}

void Graph::updateNode(const CFGBlock* waitBlock, int nodeID) {
		
	if(waitBlock) {
		_cfgBlockNodeIDMap.insert(Graph::cfgBlockNodeIDPairType(waitBlock, nodeID));
	}
}

unsigned int Graph::getNodeID(const CFGBlock *waitBlock) {
	if(_cfgBlockNodeIDMap.find(waitBlock) != _cfgBlockNodeIDMap.end()) {
		Graph::cfgBlockNodeIDMapType::iterator waitBlockFound = _cfgBlockNodeIDMap.find(waitBlock);
		return (waitBlockFound->second);
	}
	return 0;
}

Node * Graph::addNode(int id) {
  Node* n = new Node(id);
  _nodeMap.insert(Graph::nodePairType(id,n));
  ++_nNodes;
  return n;
}

Edge* Graph::addEdge(int f, int t, vector<CFGBlock*> path) {
	if(_nodeMap.find(f) != _nodeMap.end() && _nodeMap.find(t) != _nodeMap.end()) {
		nodeMapType::iterator fromNodeFound = _nodeMap.find(f);
		nodeMapType::iterator toNodeFound = _nodeMap.find(t);

		Edge *e = new Edge(fromNodeFound->second, toNodeFound->second, _nEdges, path);
		_edgeMap.insert(Graph::edgePairType(_nEdges, e));
		fromNodeFound->second->addSuccessor(toNodeFound->second);
		_adjList.insert(Graph::adjPairType(Graph::twoIntPairType(fromNodeFound->second->getId(), toNodeFound->second->getId()), e));
		++_nEdges;		
		return e;
	}
	return NULL;
}

Edge* Graph::addEdge(int f, int t, vector<CFGBlock*> path, Optional <CFGStmt> waitStmt) {
	if(_nodeMap.find(f) != _nodeMap.end() && _nodeMap.find(t) != _nodeMap.end()) {
		nodeMapType::iterator fromNodeFound = _nodeMap.find(f);
		nodeMapType::iterator toNodeFound = _nodeMap.find(t);

		Edge *e = new Edge(fromNodeFound->second, toNodeFound->second, _nEdges, path, waitStmt);
		_edgeMap.insert(Graph::edgePairType(_nEdges, e));
		fromNodeFound->second->addSuccessor(toNodeFound->second);
		_adjList.insert(Graph::adjPairType(Graph::twoIntPairType(fromNodeFound->second->getId(), toNodeFound->second->getId()), e));
		++_nEdges;		
		return e;
	}
	return NULL;
}


Edge* Graph::addEdge(Node* f, Node* t) {
  Edge* e = new Edge(f, t, _nEdges);
  _edgeMap.insert(Graph::edgePairType(_nEdges,e));

  // Update preds and succs in nodes.
  f->addSuccessor(t);
  ++_nEdges;
  
  // Insert in adjacency list.
  _adjList.insert(Graph::adjPairType(Graph::twoIntPairType(f->getId(), t->getId())
                                     , e));
  return e;
}

Edge* Graph::addEdge(Node *f, Node *t, vector<CFGBlock*> stmtBlocks) {
	Edge *e = new Edge (f, t, _nEdges, stmtBlocks);
	_edgeMap.insert(Graph::edgePairType(_nEdges, e));

	f->addSuccessor(t);
	++_nEdges;

	_adjList.insert(Graph::adjPairType(Graph::twoIntPairType(f->getId(), t->getId()), e));
	
	return e;
}

Edge* Graph::getEdge(int f, int t) {
  Graph::adjMapType::iterator fit = _adjList.find(Graph::twoIntPairType(f,t));
  if (fit == _adjList.end()) {
    return NULL;
  }
  
  return fit->second;
}

Edge* Graph::getEdge(Node* f, Node* t) {
  Graph::adjMapType::iterator fit = _adjList.find(Graph::twoIntPairType(f->getId(), t->getId()));
  if (fit == _adjList.end()) {
    return NULL;
  }
  
  return fit->second;
}
vector<Edge*> getEdges(int fromId) {
  
}
void Graph::dump(raw_ostream& os, int tabn ) {

  // Print all nodes.
  os << "Node map: " << _nNodes << "\n";
  for (Graph::nodeMapType::iterator it = _nodeMap.begin(), eit = _nodeMap.end();
       it != eit;
       it++){
    
    os <<  it->first << " " << it->second << " " ;
    it->second->dump(os, tabn++);
  }
  
  // Print all Edges.
  os << "Edge map: " << _nEdges << "\n";
  for (Graph::edgeMapType::iterator it = _edgeMap.begin(), eit = _edgeMap.end();
       it != eit;
       it++){
    
    os <<  it->first << " " << it->second << " ";
//    it->second->dump(os, tabn++);
  }

  os << "Adjacency list: " << _adjList.size() << "\n";
  for (Graph::adjMapType::iterator it = _adjList.begin(), eit = _adjList.end();
       it != eit;
       it++) {
    Graph::twoIntPairType p = it->first;
    Edge* e = it->second;
  
    os << "Edge (" << p.first << "," << p.second << ") ";
//    e->dump(os,tabn++);
  }   
}

void Graph::dumpSauto(raw_ostream& os, int tabn) {
  
	LangOptions LO;
	LO.CPlusPlus = true;
	PrintingPolicy Policy(LO);

	for (Graph::adjMapType::iterator it = _adjList.begin(), eit = _adjList.end();
	it != eit;
	it++) {
		Graph::twoIntPairType p = it->first;
		Edge *e = it->second;

		os<<" Edge ("<<p.first<<", "<<p.second<<")\n";
		os<<" Transition code \n";
		vector<CFGBlock*> codeBlocks = e->getPreStmtBlks();
		for (int i = 0; i<codeBlocks.size(); i++) {
			CFGBlock* currBlock = codeBlocks.at(i);
			for(CFGBlock::iterator it = currBlock->begin(), eit = currBlock->end();
			it != eit;
			it++) {
				if(Optional <CFGStmt> cfgStmt = it->getAs<CFGStmt>()) {
					const Stmt* stmt = cfgStmt->getStmt();
					stmt->printPretty(llvm::errs(), 0, Policy, 0);
					os<<"\n";
				}
			}
		}
	}
}



/*
void Graph::dumpSauto(raw_ostream& os, int tabn) {

  LangOptions LO;
  LO.CPlusPlus = true;
  PrintingPolicy Policy(LO);
  unsigned int state = 0;
  bool duplicateFound;
for (Graph::adjMapType::iterator it = _adjList.begin(), eit = _adjList.end();
	it != eit;
	it++) {
		Graph::twoIntPairType p = it->first;
		duplicateFound = false;
		//start and final states
		os <<"\n q_s : "<<p.first<< " "<<" q_f : "<<p.second;
		
		for (unsigned int i = 0; i<_nodeIds.size();i++) {
			if(p.first == _nodeIds.at(i)){
				duplicateFound = true;
				break;
			}
		}
		if(duplicateFound == false) {
			_nodeIds.push_back(p.first);
		}
		Edge *e = it->second;
		vector<CFGBlock*> pre = e->getPreStmtBlks();

		os <<"\n B_pre :";
		for (unsigned int i = 0; i<pre.size();i++) {
			os<<pre.at(i)->getBlockID()<<" ";
		}
		vector<CFGBlock*> guard = e->getGuardBlks();
		os <<"\n Guard :";
		for (unsigned int i = 0; i<guard.size();i++) {
			os<<guard.at(i)->getBlockID()<<" ";
			Stmt* condition = guard.at(i)->getTerminator();

		}
		vector<CFGBlock*> post = e->getPostStmtBlks();
		os <<"\n B_post :";
		for (unsigned   int i = 0; i<post.size();i++) {
			os<<post.at(i)->getBlockID()<<" ";
		}
		
		os <<"\n **************************";				
	}
}
*/

Graph::adjMapType Graph::returnAdjList() {
	return _adjList;
}

Graph::nodes Graph::returnNodeIds() {
	return _nodeIds;
}

Graph::~Graph() {
// Responsible for cleaning everything up. 
  for (Graph::nodeMapType::iterator it = _nodeMap.begin(), eit = _nodeMap.end();
       it != eit;
       it++){
  
    delete it->second;
//    os <<  it->first << " " << it->second;
//    it->second->dump(os, tabn++);
  }
  _nodeMap.clear();
  
  // Print all Edges.
  for (Graph::edgeMapType::iterator it = _edgeMap.begin(), eit = _edgeMap.end();
       it != eit;
       it++){

    delete it->second;
  }
 
 _edgeMap.clear();
  
  _adjList.clear();
  
}

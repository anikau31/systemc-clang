#include "Automata.h"

using namespace scpar;

/// Node class.

Node::Node ():_id (-1)
{

}

Node::Node (int i
):
_id (i
)
{

}

int
  Node::getId (
)
{
	return _id;
}

void
  Node::addSuccessor (Node * s
)
{
	_succs.insert (Node::connectPairType (s->getId (), s));
	s->addPredecessor (this);
}

void
  Node::addPredecessor (Node * p
)
{
	_preds.insert (Node::connectPairType (p->getId (), p));
}

vector < int >
Node::getSuccessors (int fromId
)
{

	vector < int >tmpSuccs;

	for (Node::connectMapType::iterator it = _succs.begin (), eit =
			 _succs.end (); it != eit; it++)
		{
			//  if(it->first == fromId) {
			tmpSuccs.push_back (it->first);
			//  }
		}
	return tmpSuccs;

}

vector < int >
Node::getPredecessors (int toId
)
{

	vector < int >tmpPreds;

	for (Node::connectMapType::iterator it = _preds.begin (), eit =
			 _preds.end (); it != eit; it++)
		{
			//  if(it->first == fromId) {
			tmpPreds.push_back (it->first);
			//  }
		}
	return tmpPreds;

}

void
  Node::dump (raw_ostream & os, int tabn
)
{
	os << "Node " << getId () << "\n";
	os << "  preds: ";
	for (Node::connectMapType::iterator it = _preds.begin (), eit =
			 _preds.end (); it != eit; it++)
		{
			os << it->first << " ";
		}
	os << "\n  succs: ";
	for (Node::connectMapType::iterator it = _succs.begin (), eit =
			 _succs.end (); it != eit; it++)
		{
			os << it->first << " ";
		}
	os << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////
/// Edge class.
Edge::Edge (Node * f, Node * t):_id (-1), _from (f), _to (t)
{

}

Edge::Edge (Node * f, Node * t, int i
):
_id (i
),
_from (f
),
_to (t
)
{

}

void
  Edge::updateSuspensionTime (timePairType timePair
)
{
	_timeAdvanceVector.push_back (timePair);
}

int
  Edge::getId (
)
{
	return _id;
}

void
  Edge::dump (raw_ostream & os, int tabn
)
{
	os << "Edge (" << _from->getId () << "," << _to->getId () << ")\n";
}

int
  Edge::getToId (
)
{
	return _to->getId ();
}

int
  Edge::getFromId (
)
{
	return _from->getId ();
}

Edge::timeAdvanceVectorType Edge::getTimeAdvanceVector ()
{
	return _timeAdvanceVector;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Graph class

Graph::Graph ():_nNodes (0), _nEdges (0)
{

}

Node *
Graph::addNode (
)
{
	Node *n = new Node (_nNodes);

	_nodeMap.insert (Graph::nodePairType (_nNodes, n));
	_nodeVector.push_back (n);
	_nodeIDVector.push_back (_nNodes);
	++_nNodes;
	return n;
}

Node *
Graph::addNode (int id
)
{
	Node *n = new Node (id);

	_nodeMap.insert (Graph::nodePairType (id, n));
	_nodeVector.push_back (n);
	_nodeIDVector.push_back (id);
	++_nNodes;
	return n;
}

Edge *
Graph::addEdge (Node * f, Node * t
)
{
	Edge *e = new Edge (f, t, _nEdges);

	_edgeMap.insert (Graph::edgePairType (_nEdges, e));

	// Update preds and succs in nodes.
	f->addSuccessor (t);
	++_nEdges;

	// Insert in adjacency list.
	_adjList.insert (Graph::adjPairType
									 (Graph::twoNodePairType (f->getId (), t->getId ()), e));
	return e;
}

Edge *
Graph::addEdge (int fID, int tID
)
{
	Node *f, *t;

	if (_nodeMap.find (fID) != _nodeMap.end ())
		{
			nodeMapType::iterator nodeFound = _nodeMap.find (fID);
			f = nodeFound->second;
		}
	else
		{
			f = new Node (fID);
			_nodeMap.insert (nodePairType (fID, f));
			_nodeVector.push_back (f);
			_nodeIDVector.push_back (fID);
			_nNodes++;
		}
	if (_nodeMap.find (tID) != _nodeMap.end ())
		{
			nodeMapType::iterator nodeFound = _nodeMap.find (tID);
			t = nodeFound->second;
		}
	else
		{
			t = new Node (tID);
			_nodeMap.insert (nodePairType (tID, t));
			_nodeVector.push_back (t);
			_nodeIDVector.push_back (tID);
			_nNodes++;
		}
	Edge *e = new Edge (f, t, _nEdges);

	_edgeMap.insert (Graph::edgePairType (_nEdges, e));

	// Update preds and succs in nodes.
	f->addSuccessor (t);
	++_nEdges;

	// Insert in adjacency list.
	_adjList.insert (Graph::adjPairType
									 (Graph::twoNodePairType (f->getId (), t->getId ()), e));
	return e;
}

int
  Graph::getNodeID (Node * n
)
{
	for (nodeMapType::iterator it = _nodeMap.begin (), eit = _nodeMap.end ();
			 it != eit; it++)
		{
			if (n == it->second)
				{
					return it->first;
				}
		}
	return -1;
}

int
  Graph::getEdgeID (Edge * e
)
{
	for (edgeMapType::iterator it = _edgeMap.begin (), eit = _edgeMap.end ();
			 it != eit; it++)
		{
			if (it->second == e)
				{
					return it->first;
				}
		}
	return -1;
}

int
  Graph::getEdgeID (Node * f, Node * t
)
{
	if (_adjList.find (twoNodePairType (f->getId (), t->getId ())) !=
			_adjList.end ())
		{
			adjMapType::iterator edgeFound =
				_adjList.find (twoNodePairType (f->getId (), t->getId ()));
			return getEdgeID (edgeFound->second);
		}
	return -1;
}

int
  Graph::getEdgeID (int fID, int tID
)
{
	if (_adjList.find (twoNodePairType (fID, tID)) != _adjList.end ())
		{
			adjMapType::iterator edgeFound =
				_adjList.find (twoNodePairType (fID, tID));
			return getEdgeID (edgeFound->second);
		}
	return -1;
}

Edge *
Graph::getEdge (int f, int t
)
{
	Graph::adjMapType::iterator fit =
		_adjList.find (Graph::twoNodePairType (f, t));
	if (fit == _adjList.end ())
		{
			return NULL;
		}
	return fit->second;
}

Edge *
Graph::getEdge (Node * f, Node * t
)
{
	Graph::adjMapType::iterator fit =
		_adjList.find (Graph::twoNodePairType (f->getId (), t->getId ()));
	if (fit == _adjList.end ())
		{
			return NULL;
		}

	return fit->second;
}

Node *
Graph::getNode (int nodeID
)
{
	if (_nodeMap.find (nodeID) != _nodeMap.end ())
		{
			nodeMapType::iterator nodeFound = _nodeMap.find (nodeID);
			return nodeFound->second;
		}
	return NULL;
}

vector < Edge * >Graph::getEdgesFromSource (int sourceID)
{
	vector < Edge * >edges;
	for (adjMapType::iterator it = _adjList.begin (), eit = _adjList.end ();
			 it != eit; it++)
		{
			twoNodePairType nodePair = it->first;

			if (nodePair.first == sourceID)
				{
					edges.push_back (it->second);
				}
		}
	return edges;
}

vector < Edge * >Graph::getEdgesFromDest (int destID)
{
	vector < Edge * >edges;
	for (adjMapType::iterator it = _adjList.begin (), eit = _adjList.end ();
			 it != eit; it++)
		{
			twoNodePairType nodePair = it->first;

			if (nodePair.second == destID)
				{
					edges.push_back (it->second);
				}
		}
	return edges;
}

vector < Edge * >Graph::getEdgesFromSource (Node * sourceNode)
{
	for (nodeMapType::iterator it = _nodeMap.begin (), eit = _nodeMap.end ();
			 it != eit; it++)
		{
			if (it->second == sourceNode)
				{
					getEdgesFromSource (it->first);
				}
		}
}


vector < Edge * >Graph::getEdgesFromDest (Node * destNode)
{
	for (nodeMapType::iterator it = _nodeMap.begin (), eit = _nodeMap.end ();
			 it != eit; it++)
		{
			if (it->second == destNode)
				{
					getEdgesFromDest (it->first);
				}
		}
}

void
  Graph::dump (raw_ostream & os, int tabn
)
{

	// Print all nodes.
	os << "Node map: " << _nNodes << "\n";
	for (Graph::nodeMapType::iterator it = _nodeMap.begin (), eit =
			 _nodeMap.end (); it != eit; it++)
		{

			os << it->first << " " << it->second << " ";
			it->second->dump (os, tabn++);
		}

	// Print all Edges.
	os << "Edge map: " << _nEdges << "\n";
	for (Graph::edgeMapType::iterator it = _edgeMap.begin (), eit =
			 _edgeMap.end (); it != eit; it++)
		{

			os << it->first << " " << it->second << " ";
//    it->second->dump(os, tabn++);
		}

	os << "Adjacency list: " << _adjList.size () << "\n";
	for (Graph::adjMapType::iterator it = _adjList.begin (), eit =
			 _adjList.end (); it != eit; it++)
		{
			Graph::twoNodePairType p = it->first;
			Edge *
				e = it->second;

			os << "Edge (" << p.first << "," << p.second << ") \n";
			os << "TimeAdvance : \n";
			Edge::timeAdvanceVectorType timeAdvanceVector =
				e->getTimeAdvanceVector ();
			os << "\n Size of timeAdvanceVector : " << timeAdvanceVector.size ();
			for (unsigned int i = 0; i < timeAdvanceVector.size (); i++)
				{
					Edge::timePairType timePair = timeAdvanceVector.at (i);
					os << " " << timePair.first << " " << timePair.second << "\n";
				}
//    e->dump(os,tabn++);
		}
}

void
  Graph::dumpSauto (raw_ostream & os, int tabn
)
{

	LangOptions
		LO;

	LO.CPlusPlus = true;
	PrintingPolicy
	Policy (LO
	);

	for (Graph::adjMapType::iterator it = _adjList.begin (), eit =
			 _adjList.end (); it != eit; it++)
		{
			Graph::twoNodePairType p = it->first;
			Edge *
				e = it->second;

			os << " Edge (" << p.first << ", " << p.second << ")\n";

			/*
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
			 */
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
		Graph::twoNodePairType p = it->first;
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

Graph::adjMapType Graph::returnAdjList ()
{
	return _adjList;
}

Graph::nodeIDVector Graph::returnNodeIDs ()
{
	return _nodeIDVector;
}

Graph::edgeIDVector Graph::returnEdgeIDs ()
{
	return _edgeIDVector;
}

Graph::nodeVector Graph::returnNodeVector ()
{
	return _nodeVector;
}

Graph::edgeVector Graph::returnEdgeVector ()
{
	return _edgeVector;
}

Graph::~Graph ()
{
// Responsible for cleaning everything up. 
	for (Graph::nodeMapType::iterator it = _nodeMap.begin (), eit =
			 _nodeMap.end (); it != eit; it++)
		{

			delete it->second;

//    os <<  it->first << " " << it->second;
//    it->second->dump(os, tabn++);
		}
	_nodeMap.clear ();

	// Print all Edges.
	for (Graph::edgeMapType::iterator it = _edgeMap.begin (), eit =
			 _edgeMap.end (); it != eit; it++)
		{
			delete it->second;
		}
	_edgeMap.clear ();
	_adjList.clear ();
}

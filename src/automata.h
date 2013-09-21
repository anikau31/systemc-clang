#ifndef _AUTOMATA_H_
#define _AUTOMATA_H_

#include <vector>
#include <deque>
#include <map>
#include "clang/AST/DeclCXX.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CFGStmtMap.h"
#include "clang/AST/ParentMap.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/Expr.h"
#include <stdio.h>
namespace scpar {
  using namespace clang;
  using namespace std;
  
  class Node {
  public:
    typedef pair<int, Node* > connectPairType;    
    typedef map<int, Node* > connectMapType;
		
  public:
    Node();
    Node(int);
		
    void addSuccessor(Node* );
    void addPredecessor(Node* );
    vector<int> getSuccessors(int);
    vector<int> getPredecessors(int);
    int getId();
		
    void dump(raw_ostream&, int);
  protected:
    int _id;
    connectMapType _preds;
    connectMapType _succs;    
  };
 
  class Edge {
  public:
    Edge (Node*, Node*);
    Edge (Node*, Node*, int);
		Edge (Node*, Node*, int, vector<CFGBlock*>);
  	Edge (Node*, Node*, int, vector<CFGBlock*>, Optional <CFGStmt>);

    int getId();
    int getToId();
    int getFromId();
  
    vector <CFGBlock*> getPreStmtBlks();     
		Optional <CFGStmt> getWaitStmt();
    void dump(raw_ostream&, int);

  protected:
    int _id;
    Node* _from;
    Node* _to;
    vector<CFGBlock*> _preStmtBlks;   
 		Optional <CFGStmt> _waitStmt;
 };
  
 class Graph {
  public:

    typedef vector <int> nodes;
    
		typedef pair<const CFGBlock*, int> cfgBlockNodeIDPairType;
		typedef map<const CFGBlock*, int> cfgBlockNodeIDMapType;

		typedef map<int, Node*> nodeMapType;
    typedef pair<int, Node*> nodePairType;
   
    typedef map<int, Edge*> edgeMapType;
    typedef pair<int, Edge*> edgePairType;
  
    typedef pair<int, int> twoIntPairType;
    
    typedef pair< twoIntPairType, Edge* > adjPairType;
    typedef map<pair<int, int>, Edge* > adjMapType;

    typedef pair<int, vector<Edge*> > adjEdgesPairType;
    typedef map <int, vector<Edge*> > adjEdgesMapType;

  public:
    Graph();
    ~Graph();
    
    Node* addNode();
    Node* addNode(int);
		void updateNode(const CFGBlock*, int);
		unsigned int getNodeID(const CFGBlock*);
    Edge* addEdge(Node*, Node*);    
		Edge *addEdge(Node*, Node*, vector<CFGBlock*>);
		Edge *addEdge(Node*, Node*, vector<CFGBlock*>, Optional <CFGStmt>);
    Edge *addEdge(int, int, vector<CFGBlock*>);
		Edge *addEdge(int, int, vector<CFGBlock*>, Optional <CFGStmt>);

		Edge* getEdge(Node*, Node*);
    Edge* getEdge(int, int);
    vector <Edge*> getEdges(int);
    adjMapType returnAdjList();
    nodes returnNodeIds();
    
		void dump(raw_ostream&, int tabn = 0);
		void dumpSauto(raw_ostream&, int tabn = 0); 
  protected:
    adjMapType _adjList;
    nodeMapType _nodeMap;
    edgeMapType _edgeMap;
    nodes _nodeIds;
    adjEdgesMapType _adjEdges;
		cfgBlockNodeIDMapType _cfgBlockNodeIDMap;
    int _nNodes;
    int _nEdges;
  };
  
}

#endif

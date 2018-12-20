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
    typedef pair < int, Node * >connectPairType;
    typedef map < int, Node * >connectMapType;

    Node ( );
    Node (int );

    void addSuccessor (Node * );
    void addPredecessor (Node * );

    vector < int >getSuccessors (int );
    vector < int >getPredecessors (int );
    int getId( );

    void dump (raw_ostream &, int );
  protected:
    int _id;
    connectMapType _preds;
    connectMapType _succs;

  };

  class Edge {
  public:

    typedef pair < unsigned int, unsigned int >timePairType;
    typedef vector < timePairType > timeAdvanceVectorType;

    Edge (Node *, Node * );
    Edge (Node *, Node *, int );

    void updateSuspensionTime(timePairType );

    int getId();
    int getToId();
    int getFromId();
    timeAdvanceVectorType getTimeAdvanceVector();

    void dump (raw_ostream &, int);

  protected:
    int _id;
    timeAdvanceVectorType _timeAdvanceVector;
    Node *_from;
    Node *_to;
  };

  class Graph {
  public:

    typedef vector < int >nodeIDVector;
    typedef vector < int >edgeIDVector;

    typedef vector < Node * >nodeVector;
    typedef vector < Edge * >edgeVector;

    typedef map < int, Node * >nodeMapType;
    typedef pair < int, Node * >nodePairType;

    typedef map < int, Edge * >edgeMapType;
    typedef pair < int, Edge * >edgePairType;

    typedef pair < int, int >twoNodePairType;

    typedef pair < twoNodePairType, Edge * >adjPairType;
    typedef map < twoNodePairType, Edge * >adjMapType;

    typedef pair < int, vector < Edge * > >adjEdgesPairType;
    typedef map < int, vector < Edge * > >adjEdgesMapType;

    Graph();
    ~Graph();

    Node *addNode();
    Node *addNode(int );
    Edge *addEdge(Node *, Node * );
    Edge *addEdge(int, int );

    int getEdgeID(Edge * );
    int getEdgeID (Node *, Node * );
    int getEdgeID (int, int );
    int getNodeID (Node * );

    Edge *getEdge (Node *, Node * );
    Edge *getEdge (int, int );
    Node *getNode (int );

    vector < Edge * >getEdgesFromSource (int  );
    vector < Edge * >getEdgesFromSource (Node *  );
    vector < Edge * >getEdgesFromDest (int );
    vector < Edge * >getEdgesFromDest (Node * );

    adjMapType returnAdjList();
    nodeIDVector returnNodeIDs();
    edgeIDVector returnEdgeIDs();
    nodeVector returnNodes();
    edgeVector returnEdges();
    edgeVector returnEdgeVector();
    nodeVector returnNodeVector();

    void dump (raw_ostream &, int tabn = 0);
    void dumpSauto (raw_ostream &, int tabn = 0);
  protected:
    adjMapType _adjList;
    nodeMapType _nodeMap;
    edgeMapType _edgeMap;
    adjEdgesMapType _adjEdges;
    int _nNodes;
    int _nEdges;
    nodeIDVector _nodeIDVector;
    edgeIDVector _edgeIDVector;
    nodeVector _nodeVector;
    edgeVector _edgeVector;
  };
}
#endif

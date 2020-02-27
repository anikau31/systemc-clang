#ifndef VECTOR_TREE_H_
#define VECTOR_TREE_H_

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

using namespace std;

//
// class TreeNode
//
//
class TreeNode {
 private:
  std::string data_;
  // std::vector<TreeNode *> child_;
  bool discovered_;

 public:
  // friend class Tree;

  TreeNode(string data) : data_{data}, discovered_{false} {};

  ~TreeNode() {}

  std::string getData() const { return data_; }

  bool isDiscovered() const { return discovered_; }
  void setDiscovered() { discovered_ = true; }

  void dump() { cout << ": " << data_ << " "; }

  virtual void visit() { cout << data_ << " "; }
};

//////////////////////
// class Tree
//
//
class Tree {
 public:
  typedef TreeNode *TreeNodePtr;
  typedef std::vector<TreeNodePtr> VectorTreePtr;

 private:
  // Adjacency list.
  // This is a reasonable structure since types are
  // typically going to be of small depth.
  std::map<TreeNodePtr, std::vector<TreeNodePtr> > adj_list_;
  TreeNodePtr root_;

 public:
  void dump() {
    for (auto const &entry : adj_list_) {
      auto node{entry.first};
      auto edges{entry.second};
      cout << node->getData() << " => size: " << edges.size() << "\n";
      for (auto const &edge_node : edges) {
        cout << "  " << edge_node->getData();
      }
      cout << "\n";
    }
  }

  std::size_t size() const { return adj_list_.size(); }

  void setRoot(const TreeNodePtr from) { root_ = from; }
  const TreeNodePtr getRoot() const { return root_; }

  TreeNodePtr addNode(std::string data) {
    TreeNodePtr new_node{new TreeNode(data)};
    VectorTreePtr empty_edges{};
    adj_list_.emplace(std::make_pair(new_node, empty_edges));
    return new_node;
  }

  void addEdge(const TreeNodePtr from, const TreeNodePtr to) {
    auto source{adj_list_.find(from)};

    if (source == adj_list_.end()) {
      return;
    }

    auto &edges{source->second};
    edges.push_back(to);
  }

  void dft(TreeNodePtr root) {
    std::stack<TreeNodePtr> visit{};
    visit.push(root);

    while (!visit.empty()) {
      // node is a TreeNodePtr
      auto &node{visit.top()};
      node->visit();
      // Call back function.
      visit.pop();


      if (!node->isDiscovered()) {
        node->setDiscovered();

        auto source{adj_list_.find(node)};

        if (source == adj_list_.end()) {
          return;
        }

        auto edges{ source->second };
        for ( auto & node: edges ) {
          visit.push( node );
        }

      }
    }
  }
};

#endif

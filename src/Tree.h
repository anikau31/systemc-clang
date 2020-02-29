#ifndef VECTOR_TREE_H_
#define VECTOR_TREE_H_

#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>

//#include "llvm/Support/raw_ostream.h"

using namespace std;

//
// class TreeNode
//
//
template <typename T>
class TreeNode {
 private:
  T data_;
  // std::vector<TreeNode *> child_;
  bool discovered_;

 public:
  // friend class Tree;

  TreeNode(T data) : data_{data}, discovered_{false} {};

  TreeNode(const TreeNode &from) {
    data_ = from.data_;
    discovered_ = from.discovered_;
  }
  ~TreeNode() {}

  T getData() const { return data_; }
  std::string getStringData() const { return data_.getTypeName(); }

  bool isDiscovered() const { return discovered_; }
  void setDiscovered() { discovered_ = true; }
  void resetDiscovered() { discovered_ = false; }

  void dump() { cout << "[" << getStringData() << "] "; }

  virtual void visit() { cout << " " << getStringData() << " "; }
};

//////////////////////
// class Tree
//
//
template <typename T>
class Tree {
 public:
  typedef TreeNode<T> *TreeNodePtr;
  typedef std::vector<TreeNodePtr> VectorTreePtr;

 private:
  // Adjacency list.
  // This is a reasonable structure since types are
  // typically going to be of small depth.
  std::map<TreeNodePtr, std::vector<TreeNodePtr> > adj_list_;
  TreeNodePtr root_;

 public:
  Tree() : root_{nullptr}, run_dft_{false}, run_bft_{false} {}
  virtual ~Tree() {
    for (auto node : adj_list_) {
      delete node.first;
    }
    adj_list_.clear();
  }

  void dump() {
    for (auto const &entry : adj_list_) {
      auto node{entry.first};
      auto edges{entry.second};
      cout << node->getStringData() << " => size: " << edges.size() << "\n";
      for (auto const &edge_node : edges) {
        cout << "   " << edge_node->getStringData();
      }
      cout << "\n";
    }
  }

  std::size_t size() const { return adj_list_.size(); }

  void setRoot(const TreeNodePtr from) { root_ = from; }

  const TreeNodePtr getRoot() const { return root_; }

  bool foundNode(TreeNodePtr node) const {
    auto found_node{adj_list_.find(node)};
    if (found_node == adj_list_.end()) {
      return false;
    }
    return true;
  }

  bool hasChildren(TreeNodePtr node) {
    if (!foundNode(node)) {
      return false;
    }

    return (adj_list_[node].size() > 0);
  }

  const VectorTreePtr &getChildren(TreeNodePtr node) { return adj_list_[node]; }

  TreeNodePtr addNode(T data) {
    TreeNodePtr new_node{new TreeNode<T>(data)};
    VectorTreePtr empty_edges{};
    adj_list_.insert(adj_list_.begin(), std::make_pair(new_node, empty_edges));
    return new_node;
  }

  void addEdge(const TreeNodePtr from, const TreeNodePtr to) {
    auto source{adj_list_.find(from)};

    if (source == adj_list_.end()) {
      return;
    }

    auto &edges{source->second};
    // Insert it into the beginning of the vector.
    // edges.insert(edges.begin(), to);
    edges.push_back(to);
  }

  void resetDiscovered() {
    for (auto const &node : adj_list_) {
      node.first->resetDiscovered();
    }
  }

  std::string bft(TreeNodePtr root) {
    resetDiscovered();
    std::string return_string{};

    std::queue<TreeNodePtr> que{};
    root->setDiscovered();
    que.push(root);

    while (!que.empty()) {
      auto node{que.front()};
      node->visit();
      nodes_bft_.push_back(node);
      return_string += " ";
      return_string += node->getStringData();
      que.pop();

      auto source{adj_list_.find(node)};

      if (source == adj_list_.end()) {
        return "";
      }

      auto const &edges{source->second};
      for (auto const &edge_node : edges) {
        if (!edge_node->isDiscovered()) {
          edge_node->setDiscovered();
          que.push(edge_node);
        }
      }
    }
    return return_string;
  }

  std::string dft(TreeNodePtr root) {
    run_dft_ = true;
    resetDiscovered();
    std::string return_string;

    std::stack<TreeNodePtr> visit{};
    visit.push(root);

    int sp{0};
    while (!visit.empty()) {
      auto &node{visit.top()};
      node->visit();
      return_string += " ";
      return_string += node->getStringData();
      nodes_dft_.push_back(node);

      for (int i{0}; i < sp; ++i) {
        llvm::outs() << " ";
      }
      llvm::outs() << sp << ": " << node->getStringData() << "\n";
      sp += 2;
      // Call back function.
      visit.pop();

      if (!node->isDiscovered()) {
        node->setDiscovered();

        auto source{adj_list_.find(node)};
        if (source == adj_list_.end()) {
          return "";
        }

        auto const &edges{source->second};
        if (edges.size() == 0 ) {
          sp -=2;
        }
        for (auto &node : edges) {
          visit.push(node);
        }
      }
    }
    return return_string;
  }

 public:
  // Iterators
  class dft_iterator {
   public:
    typedef std::vector<TreeNodePtr> *TreeDFTPtr;

   private:
    TreeDFTPtr nodes_dft_;
    std::size_t pos_;

   public:
    dft_iterator(const TreeDFTPtr nodes_dft, std::size_t pos)
        : nodes_dft_{nodes_dft}, pos_{pos} {}

    std::string operator*() {
      return (nodes_dft_)->operator[](pos_)->getData();
    }

    dft_iterator &operator++() {
      ++pos_;
      return *this;
    }

    dft_iterator begin() {
      pos_ = 0;
      return *this;
    }

    dft_iterator end() {
      pos_ = nodes_dft_->size();
      return *this;
    }

    bool operator!=(const dft_iterator &it) { return pos_ != it.pos_; }
  };

  dft_iterator begin() {
    if (!run_dft_ && root_) {
      run_dft_ = true;
      dft(root_);
    }
    return dft_iterator{&nodes_dft_, 0};
  }

  dft_iterator end() { return dft_iterator{&nodes_dft_, nodes_dft_.size()}; }

 private:
  bool run_dft_;
  bool run_bft_;
  std::vector<TreeNodePtr> nodes_bft_;
  std::vector<TreeNodePtr> nodes_dft_;
};

#endif

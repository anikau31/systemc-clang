#ifndef VECTOR_TREE_H_
#define VECTOR_TREE_H_

#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include "llvm/Support/raw_ostream.h"

namespace systemc_clang {
//
/// class TreeNode<T>
//
//
template <typename T>
class TreeNode {
 private:
  T data_;            /// Data item.
  bool discovered_;   /// Whether this node was discovered or not.
  TreeNode *parent_;  /// Parent node.

 public:
  /// Constructor.
  TreeNode(T data) : data_{data}, discovered_{false}, parent_{this} {};

  TreeNode(const TreeNode &from) {
    data_ = from.data_;
    discovered_ = from.discovered_;
    parent_ = from.parent_;
  }

  virtual ~TreeNode() {
    // parent_ = nullptr;
  }

  T getData() const { return data_; }
  const T *getDataPtr() { return &data_; }
  std::string toString() const { return data_.toString(); }

  bool isDiscovered() const { return discovered_; }
  void setDiscovered() { discovered_ = true; }
  void resetDiscovered() { discovered_ = false; }

  void setParent(TreeNode *from) { parent_ = from; }

  TreeNode *getParent() const { return parent_; }

  void dump() { llvm::outs() << "[" << toString() << "] "; }

  virtual void visit() {
    llvm::outs() << "(" << parent_->toString() << ")"
                 << " " << toString() << " ";
  }
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

  bool run_dft_;
  bool run_bft_;
  std::vector<TreeNodePtr> nodes_bft_;
  std::vector<TreeNodePtr> nodes_dft_;

 public:
  Tree() : root_{nullptr}, run_dft_{false}, run_bft_{false} {}

  Tree(const Tree &from) {
    adj_list_ = from.adj_list_;
    root_ = from.root_;
    run_dft_ = from.run_dft_;
    run_bft_ = from.run_bft_;
    nodes_bft_ = from.nodes_bft_;
    nodes_dft_ = from.nodes_dft_;
  }

  ~Tree() {
    for (auto &node : adj_list_) {
      delete node.first;
      node.second.clear();
    }
    adj_list_.clear();
    nodes_dft_.clear();
    nodes_bft_.clear();
    root_ = nullptr;
  }

  void dump() {
    for (auto const &entry : adj_list_) {
      auto node{entry.first};
      auto edges{entry.second};
      llvm::outs() << node->toString() << " => size: " << edges.size() << "\n";
      for (auto const &edge_node : edges) {
        llvm::outs() << "   " << edge_node->toString();
      }
      llvm::outs() << "\n";
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
    //
    // Clang parses the template arguments in left-to-right manner.  Inserting
    // these types at the beginning works to represent this correctly for the
    // DFT.  This is because the DFT algorithm picks up from begin() to end()
    // the edges, and inserts them into a queue by pushing them on.
    edges.insert(edges.begin(), to);
    to->setParent(from);
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
      return_string += node->toString();
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

  std::string dft(TreeNodePtr root = nullptr) {
    resetDiscovered();
    std::string return_string;

    // If the argument is not specified, then just use the root.
    if (root == nullptr) {
      root = root_;
    }

    // If the root itself is not found then we can't run DFT.
    if (root == nullptr) {
      return return_string;
    }

    std::stack<TreeNodePtr> visit{};
    visit.push(root);

    while (!visit.empty()) {
      auto &node{visit.top()};
      node->visit();
      return_string += node->toString();
      return_string += " ";

      // llvm::outs() << "@@ " << node->toString() << "\n";
      // For repeated calls to dft...
      // Check if node is already in there
      if (!run_dft_) {
        nodes_dft_.push_back(node);
      }

      // Call back function.
      visit.pop();

      if (!node->isDiscovered()) {
        node->setDiscovered();

        auto source{adj_list_.find(node)};
        if (source == adj_list_.end()) {
          return "";
        }

        auto const &edges{source->second};
        for (auto &node : edges) {
          visit.push(node);
        }
      }
    }
    if (!run_dft_) {
      run_dft_ = true;
    }
    return return_string;
  }

 public:
  /// Iterators
  //
  //
  //
  /// class dft_iterator
  //
  class const_dft_iterator {
   public:
    typedef std::vector<TreeNodePtr> *TreeDFTPtr;

   private:
    Tree<T> *tree_;
    TreeDFTPtr nodes_dft_;
    std::size_t pos_;

   public:
    const_dft_iterator(Tree<T> *tree, std::size_t pos)
        : tree_{tree}, nodes_dft_{&tree->nodes_dft_}, pos_{pos} {
      tree->dft();
    }

    const TreeNodePtr &operator*() { return (nodes_dft_)->operator[](pos_); }

    const_dft_iterator &operator++() {
      ++pos_;
      return *this;
    }

    const_dft_iterator begin() {
      pos_ = 0;
      return *this;
    }

    const_dft_iterator end() {
      pos_ = nodes_dft_->size();
      return *this;
    }

    bool operator!=(const const_dft_iterator &it) { return pos_ != it.pos_; }
  };

  const_dft_iterator begin() const { return const_dft_iterator{this, 0}; }

  const_dft_iterator end() const {
    return const_dft_iterator{this, nodes_dft_.size()};
  }

  //
  /// class dft_iterator
  ///
  class dft_iterator {
   public:
    typedef std::vector<TreeNodePtr> *TreeDFTPtr;

   private:
    Tree<T> *tree_;
    TreeDFTPtr nodes_dft_;
    std::size_t pos_;

   public:
    dft_iterator(Tree<T> *tree, std::size_t pos)
        : tree_{tree}, nodes_dft_{&tree->nodes_dft_}, pos_{pos} {
      tree_->dft();
    }

    TreeNodePtr &operator*() { return (nodes_dft_)->operator[](pos_); }

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

  dft_iterator begin() { return dft_iterator{this, 0}; }

  dft_iterator end() { return dft_iterator{this, nodes_dft_.size()}; }
};
};  // namespace systemc_clang
#endif

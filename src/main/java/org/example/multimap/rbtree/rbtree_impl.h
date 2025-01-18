/*
\author superboy
\date 26.01.2024

srcs:
    https://www.programiz.com/dsa/red-black-tree
    https://habr.com/ru/articles/557328/
    https://habr.com/ru/articles/573502/
*/

#ifndef S21_CONTAINER_SRC_SET_RBTREE_H_
#define S21_CONTAINER_SRC_SET_RBTREE_H_

#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>

enum color_t { RED_NODE, BLACK_NODE };

template <typename Key, typename T>
class RBNode {
  using value_type = std::pair<const Key, T>;

 public:
  value_type* value;
  RBNode* parent;
  RBNode* left;
  RBNode* right;
  color_t color;

  RBNode(value_type* val, RBNode* p, RBNode* l, RBNode* r, color_t color)
      : value(val), color(color), parent(p), left(l), right(r){};

  bool operator==(RBNode<Key, T>& other) const noexcept {
    return (value == other.value);
  };

  bool operator!=(RBNode<Key, T>& other) const noexcept {
    return (value != other.value);
  };
};

template <typename Key, typename T, bool is_const>
class RBIterator;

template <typename Key, typename T, typename Compare = std::less<T>,
          typename Allocator = std::allocator<T>>
class RBTree {
  friend class RBNode<Key, T>;
  friend class RBIterator<Key, T, true>;
  friend class RBIterator<Key, T, false>;

 public:
  using value_type = std::pair<const Key, T>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = RBIterator<Key, T, false>;
  using const_iterator = RBIterator<Key, T, true>;
  using size_type = size_t;
  using node_type = RBNode<Key, T>;

  using NodeAlloc = typename Allocator::template rebind<RBNode<Key, T>>::other;
  using ValAlloc =
      typename Allocator::template rebind<std::pair<const Key, T>>::other;
  using node_traits = std::allocator_traits<NodeAlloc>;
  using val_traits = std::allocator_traits<ValAlloc>;

  RBTree()
      : val_alloc_(), node_alloc_(), compare_(), root_(nullptr), size_(0){};

  ~RBTree() noexcept {
    if (!empty() && size_ != 0) {
      DeallocateDestroyTree(root_);
    }
    size_ = 0;
    root_ = nullptr;
  };

  RBTree(const RBTree<Key, T>& other) : root_(nullptr), size_(0) {
    auto it = other.cbegin();
    for (size_t i = 0; i < other.size_; ++i) {
      if (it == other.cend()) {
        break;
      }
      insert((*it).first, (*it).second);
      it++;
    }
  };

  iterator begin() noexcept {
    node_type* begin = root_;
    while (begin != nullptr && begin->left != nullptr) {
      begin = begin->left;
    }
    auto i = iterator(begin, this);
    return i;
  };

  const_iterator cbegin() const noexcept {
    node_type* begin = root_;
    while (begin != nullptr && begin->left != nullptr) {
      begin = begin->left;
    }
    auto i = const_iterator(begin, this);
    return i;
  };

  iterator end() noexcept { return iterator(nullptr, this); };

  const_iterator cend() const noexcept {
    return const_iterator(nullptr, this);
  };

  iterator find(const Key& key) noexcept {
    if (!empty()) {
      for (iterator itr = begin(); itr != end(); ++itr) {
        if ((*itr).first == key) {
          return itr;
        }
      }
    }
    return end();
  };

  bool contains(const Key& key) const noexcept {
    if (!empty()) {
      for (const_iterator itr = cbegin(); itr != cend(); ++itr) {
        if ((*itr).first == key) {
          return true;
        }
      }
    }
    return false;
  };

  bool empty() const noexcept { return (root_ == nullptr); };

  size_type size() const noexcept { return size_; };

  node_type* max() const noexcept {
    node_type* parent = root_;
    node_type* next = nullptr;
    while (parent->right != nullptr) {
      next = parent;
      parent = next->right;
    }
    return parent;
  };

  node_type* min() const noexcept {
    node_type* parent = root_;
    node_type* next = nullptr;
    while (parent->left != nullptr) {
      next = parent;
      parent = next->left;
    }
    return parent;
  };

  node_type* min(node_type* node) noexcept {
    node_type* parent = node;
    node_type* next = nullptr;
    while (parent->left != nullptr) {
      next = node;
      parent = next->left;
    }
    return parent;
  };

  void clear() noexcept {
    if (!empty()) {
      DeallocateDestroyTree(root_);
    }
    size_ = 0;
    root_ = nullptr;
  };

  std::pair<iterator, bool> insert(const Key& key, const T& value) {
    ++size_;
    node_type* new_node = nullptr;
    try {
      new_node = alloc(key, value);
    } catch (...) {
      throw;
    }
    if (empty()) {
      root_ = new_node;
      root_->color = BLACK_NODE;
      return std::make_pair<iterator, bool>(begin(), true);
    }
    node_type* current = root_;
    node_type* parent = nullptr;
    while (current != nullptr) {
      parent = current;
      if (key < current->value->first) {
        current = current->left;
      } else if (key > current->value->first) {
        current = current->right;
      } else {
        return std::make_pair<iterator, bool>(end(), false);
      }
    }
    new_node->parent = parent;
    (key < parent->value->first) ? parent->left = new_node
                                 : parent->right = new_node;
    if (new_node->parent->color == RED_NODE) {
      InsertFixUp(new_node);
    }
    return std::make_pair<iterator, bool>(iterator(new_node, this), true);
  };

  void erase(iterator pos) noexcept {
    if (pos == end()) {
      return;
    }
    node_type* erase_node = find((*pos).first).itr_node_;
    color_t erase_color = erase_node->color;

    if (erase_node == root_ && size_ == 1) {
      dealloc(erase_node);
      size_ = 0;
      return;
    }

    if (erase_color == RED_NODE && erase_node->left == nullptr &&
        erase_node->right == nullptr) {
      (erase_node->parent->left == erase_node)
          ? erase_node->parent->left = nullptr
          : erase_node->parent->right = nullptr;
      dealloc(erase_node);
    } else if (erase_color == BLACK_NODE && erase_node->left == nullptr &&
               erase_node->right == nullptr) {
      EraseFixUp(erase_node, erase_node->parent);
      (erase_node->parent->left == erase_node)
          ? erase_node->parent->left = nullptr
          : erase_node->parent->right = nullptr;
      dealloc(erase_node);
    } else if (erase_node->left != nullptr && erase_node->right != nullptr) {
      node_type* min_node = min(erase_node->right);
      Replace(erase_node, min_node);
      if (erase_node->left == nullptr && erase_node->right == nullptr) {
        if (erase_node->color == RED_NODE) {
          (erase_node->parent->left == erase_node)
              ? erase_node->parent->left = nullptr
              : erase_node->parent->right = nullptr;
          dealloc(erase_node);
        } else {
          EraseFixUp(erase_node, erase_node->parent);
          (erase_node->parent->left == erase_node)
              ? erase_node->parent->left = nullptr
              : erase_node->parent->right = nullptr;
          dealloc(erase_node);
        }
      } else {
        Replace(erase_node, (erase_node->left == nullptr) ? erase_node->right
                                                          : erase_node->left);
        if (erase_node->color == BLACK_NODE) {
          EraseFixUp(erase_node, erase_node->parent);
          (erase_node->parent->left == erase_node)
              ? erase_node->parent->left = nullptr
              : erase_node->parent->right = nullptr;
          dealloc(erase_node);
        } else {
          (erase_node->parent->left == erase_node)
              ? erase_node->parent->left = nullptr
              : erase_node->parent->right = nullptr;
          dealloc(erase_node);
        }
      }
    } else if (erase_color == BLACK_NODE &&
               (erase_node->left != nullptr || erase_node->right != nullptr)) {
      Replace(erase_node, (erase_node->left == nullptr) ? erase_node->right
                                                        : erase_node->left);
      if (erase_node->color == BLACK_NODE) {
        EraseFixUp(erase_node, erase_node->parent);
        (erase_node->parent->left == erase_node)
            ? erase_node->parent->left = nullptr
            : erase_node->parent->right = nullptr;
        dealloc(erase_node);
      } else {
        (erase_node->parent->left == erase_node)
            ? erase_node->parent->left = nullptr
            : erase_node->parent->right = nullptr;
        dealloc(erase_node);
      }
    }
    --size_;
  };

  void swap(RBTree<Key, T>& other) {
    std::swap(val_alloc_, other.val_alloc_);
    std::swap(node_alloc_, other.node_alloc_);
    std::swap(compare_, other.compare_);
    std::swap(root_, other.root_);
    std::swap(size_, other.size_);
  };

  void Move(RBTree<Key, T>&& other) noexcept {
    root_ = other.root_;
    size_ = other.size_;
    val_alloc_ = other.val_alloc_;
    node_alloc_ = other.node_alloc_;
    compare_ = other.compare_;
    other.root_ = nullptr;
    other.size_ = 0;
  };

 private:
  ValAlloc val_alloc_;
  NodeAlloc node_alloc_;
  Compare compare_;
  node_type* root_;
  size_type size_;

  void Replace(node_type* dest, node_type* src) noexcept {
    color_t col = dest->color;
    node_type* parent = dest->parent;
    node_type* left = dest->left;
    node_type* right = dest->right;

    if (dest->parent == nullptr) {
      root_ = src;
      root_->color = BLACK_NODE;
    } else if (dest == dest->parent->left) {
      dest->parent->left = src;
    } else {
      dest->parent->right = src;
    }
    if (src != nullptr) {
      dest->color = src->color;
      if (src->parent == dest) {
        dest->parent = src;
      } else {
        dest->parent = src->parent;
      }
      dest->right = src->right;
      if (src->right != nullptr) {
        src->right->parent = dest;
      }
      dest->left = src->left;
      if (src->left != nullptr) {
        src->left->parent = dest;
      }
      src->parent = parent;
      src->color = col;

      if (left == src) {
        src->left = dest;
      } else {
        src->left = left;
        if (left != nullptr) {
          left->parent = src;
        }
      }
      if (right == src) {
        src->right = dest;
      } else {
        src->right = right;
        if (right != nullptr) {
          right->parent = src;
        }
      }
    }
  };

  /*
  https://habr.com/ru/articles/573502/
  */
  void EraseFixUp(node_type* node, node_type* parent) noexcept {
    node_type* brother = nullptr;

    if (parent == root_ && size_ == 2) {
      if (parent->left == node) {
        if (parent->right != nullptr) {
          parent->right->color = RED_NODE;
        }
      } else {
        if (parent->left != nullptr) {
          parent->left->color = RED_NODE;
        }
      }
      return;
    }

    while (node != root_ && (node == nullptr || node->color == BLACK_NODE)) {
      if (node == parent->left) {
        brother = parent->right;
        if (brother->color == RED_NODE) {
          brother->color = BLACK_NODE;
          parent->color = RED_NODE;
          RotateL(parent);
          brother = parent->right;
          if (brother == nullptr) {
            break;
          }
        }
        if ((brother->left == nullptr || brother->left->color == BLACK_NODE) &&
            (brother->right == nullptr ||
             brother->right->color == BLACK_NODE)) {
          brother->color = RED_NODE;
          node = parent;
          parent = parent->parent;
        } else {
          if (brother->right == nullptr ||
              brother->right->color == BLACK_NODE) {
            brother->left->color = BLACK_NODE;
            brother->color = RED_NODE;
            RotateR(brother);
            brother = parent->right;
          }
          brother->color = parent->color;
          parent->color = BLACK_NODE;
          if (brother->right != nullptr) {
            brother->right->color = BLACK_NODE;
            RotateL(parent);
          }
          node = root_;
        }
      } else {
        brother = parent->left;
        if (brother->color == RED_NODE) {
          brother->color = BLACK_NODE;
          parent->color = RED_NODE;
          RotateR(parent);
          brother = parent->left;
          if (brother == nullptr) {
            break;
          }
        }
        if ((brother->right == nullptr ||
             brother->right->color == BLACK_NODE) &&
            (brother->left == nullptr || brother->left->color == BLACK_NODE)) {
          brother->color = RED_NODE;
          node = parent;
          parent = parent->parent;
        } else {
          if (brother->left == nullptr || brother->left->color == BLACK_NODE) {
            brother->right->color = BLACK_NODE;
            brother->color = RED_NODE;
            RotateL(brother);
            brother = parent->left;
          }
          brother->color = parent->color;
          parent->color = BLACK_NODE;
          if (brother->left != nullptr) {
            brother->left->color = BLACK_NODE;
            RotateR(parent);
          }
          node = root_;
        }
      }
    }
    node->color = BLACK_NODE;
  };

  void InsertFixUp(node_type* new_node) noexcept {
    node_type* uncle = nullptr;
    node_type* parent = nullptr;
    node_type* grandparent = nullptr;
    while (new_node->parent->color == RED_NODE) {
      parent = new_node->parent;
      grandparent = new_node->parent->parent;
      if (parent == grandparent->right) {
        uncle = new_node->parent->parent->left;
        if (uncle != nullptr && uncle->color == RED_NODE) {
          uncle->color = BLACK_NODE;
          parent->color = BLACK_NODE;
          grandparent->color = RED_NODE;
          new_node = grandparent;
        } else {
          if (new_node == parent->left) {
            new_node = parent;
            RotateR(new_node);
            parent = new_node->parent;
            grandparent = new_node->parent->parent;
          }
          parent->color = BLACK_NODE;
          grandparent->color = RED_NODE;
          RotateL(grandparent);
        }
      } else {
        uncle = new_node->parent->parent->right;
        if (uncle != nullptr && uncle->color == RED_NODE) {
          uncle->color = BLACK_NODE;
          parent->color = BLACK_NODE;
          grandparent->color = RED_NODE;
          new_node = grandparent;
        } else {
          if (new_node == parent->right) {
            new_node = parent;
            RotateL(new_node);
            parent = new_node->parent;
            grandparent = new_node->parent->parent;
          }
          parent->color = BLACK_NODE;
          grandparent->color = RED_NODE;
          RotateR(grandparent);
        }
      }
      if (new_node == root_) {
        break;
      }
    }
    root_->color = BLACK_NODE;
  };

  void RotateL(node_type* parent) noexcept {
    node_type* child = nullptr;
    child = parent->right;
    parent->right = child->left;
    if (child->left != nullptr) {
      child->left->parent = parent;
    }
    child->parent = parent->parent;
    if (parent->parent == nullptr) {
      root_ = child;
    } else if (parent == parent->parent->left) {
      parent->parent->left = child;
    } else {
      parent->parent->right = child;
    }
    child->left = parent;
    parent->parent = child;
  };

  void RotateR(node_type* parent) noexcept {
    node_type* child = parent->left;
    parent->left = child->right;
    if (child->right != nullptr) {
      child->right->parent = parent;
    }
    child->parent = parent->parent;
    if (parent->parent == nullptr) {
      root_ = child;
    } else if (parent == parent->parent->right) {
      parent->parent->right = child;
    } else {
      parent->parent->left = child;
    }
    child->right = parent;
    parent->parent = child;
  };

  void ColorSwap(node_type* red_parent) noexcept {
    if (red_parent->left == nullptr || red_parent->right == nullptr) {
      return;
    }
    red_parent->left->color = BLACK_NODE;
    red_parent->right->color = BLACK_NODE;
    (red_parent == root_) ? red_parent->color = BLACK_NODE
                          : red_parent->color = RED_NODE;
  };

  node_type* alloc(const Key& key, const T& value) {
    node_type* new_node = nullptr;
    new_node = node_traits::allocate(node_alloc_, 1);
    try {
      new_node->value = val_traits::allocate(val_alloc_, 1);
      val_traits::construct(val_alloc_, std::addressof(new_node->value->first),
                            std::move_if_noexcept(key));
      val_traits::construct(val_alloc_, std::addressof(new_node->value->second),
                            std::move_if_noexcept(value));
      new_node->left = nullptr;
      new_node->right = nullptr;
      new_node->parent = nullptr;
      node_traits::construct(node_alloc_, std::addressof(new_node->color),
                             RED_NODE);  // node_traits::construct(node_alloc_,
                                         // std::addressof(new_node), );
    } catch (...) {
      node_traits::deallocate(node_alloc_, new_node, 1);
      throw;
    }
    return new_node;
  };

  void dealloc(node_type* node) noexcept {
    node->right = nullptr;
    node->left = nullptr;
    node->parent = nullptr;
    val_traits::destroy(val_alloc_, std::addressof(node->value->first));
    val_traits::destroy(val_alloc_, std::addressof(node->value->second));
    val_traits::deallocate(val_alloc_, node->value, 1);
    node_traits::destroy(node_alloc_, node);
    node_traits::deallocate(node_alloc_, node, 1);
    node = nullptr;
  };

  void DeallocateDestroyTree(node_type* ptr) {
    if (ptr == nullptr) {
      return;
    }
    if (ptr->left != nullptr) {
      DeallocateDestroyTree(ptr->left);
    }

    if (ptr->right != nullptr) {
      DeallocateDestroyTree(ptr->right);
    }
    dealloc(ptr);
  };
};

template <typename Key, typename T, bool is_const>
class RBIterator {
 public:
  friend class RBTree<Key, T>;
  friend class RBNode<Key, T>;
  using value_type = std::pair<const Key, T>;
  using node_type = RBNode<Key, T>;
  using tree_type = RBTree<Key, T>;

  using conditional_node_ptr =
      std::conditional_t<is_const, const node_type*, node_type*>;
  using conditional_node_ref =
      std::conditional_t<is_const, const node_type&, node_type&>;
  using conditional_tree_ptr =
      std::conditional_t<is_const, const tree_type*, tree_type*>;

  RBIterator() noexcept : itr_node_(nullptr), itr_tree_(nullptr){};

  RBIterator(conditional_node_ptr node, conditional_tree_ptr tree)
      : itr_node_(node), itr_tree_(tree){};

  RBIterator(const RBIterator<Key, T, is_const>& other)
      : itr_node_(other.itr_node_), itr_tree_(other.itr_tree_){};

  RBIterator& operator=(const RBIterator&& other) noexcept {
    if (this != &other) {
      itr_node_ = std::move(other.itr_node_);
      itr_tree_ = std::move(other.itr_tree_);
    }
    return *this;
  };

  value_type& operator*() const { return *(itr_node_->value); };

  value_type* operator->() const { return &(itr_node_->value); };

  conditional_node_ptr node() { return itr_node_; };

  RBIterator& operator++() {
    if (itr_node_ == nullptr) {
      return *this;
    };
    if (itr_node_->right != nullptr) {
      itr_node_ = itr_node_->right;
      while (itr_node_->left != nullptr) {
        itr_node_ = itr_node_->left;
      }
    } else {
      node_type* parent = itr_node_->parent;
      while (parent != nullptr && itr_node_ == parent->right) {
        itr_node_ = parent;
        parent = parent->parent;
      }
      itr_node_ = parent;
    }
    return *this;
  };

  RBIterator& operator++(int) {
    /*RBIterator tmp(*this);
    ++(*this);
    return tmp;*/
    ++(*this);
    return *this;
  };

  RBIterator& operator--() {
    if (itr_node_ == nullptr && itr_node_ != itr_tree_->root_) {
      RBIterator t(itr_tree_->max(), itr_tree_);
      this->itr_node_ = t.itr_node_;
      this->itr_tree_ = t.itr_tree_;
      return *this;
    }
    if (itr_node_->left != nullptr) {
      itr_node_ = itr_node_->left;
      while (itr_node_->right != nullptr) {
        itr_node_ = itr_node_->right;
      }
    } else {
      node_type* parent = itr_node_->parent;
      while (parent != nullptr && itr_node_ == parent->left) {
        itr_node_ = parent;
        parent = parent->parent;
      }
      itr_node_ = parent;
    }
    return *this;
  };

  RBIterator& operator--(int) {
    /*RBIterator tmp(*this);
    --(*this);
    return tmp;*/
    --(*this);
    return *this;
  };

  bool operator==(const RBIterator& other) {
    return itr_node_ == other.itr_node_;
  };

  bool operator!=(const RBIterator& other) { return !(*this == other); };

 private:
  conditional_node_ptr itr_node_;
  conditional_tree_ptr itr_tree_;
};

#endif  // S21_CONTAINER_SRC_SET_RBTREE_H_
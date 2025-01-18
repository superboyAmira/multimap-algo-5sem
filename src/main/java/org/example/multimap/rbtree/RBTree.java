package org.example.multimap.rbtree;

import java.util.Iterator;
import java.util.NoSuchElementException;

public class RBTree<Key extends Comparable<Key>, T> implements Iterable<RBNode<Key, T>> {
    private RBNode<Key, T> root;
    private int size;

    public RBTree() {
        this.root = null;
        this.size = 0;
    }

    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public void clear() {
        root = null;
        size = 0;
    }

    public boolean containsKey(Key key) {
        return findNode(key) != null;
    }

    public T get(Key key) {
        RBNode<Key, T> node = findNode(key);
        return node != null ? node.value : null;
    }

    public void insert(Key key, T value) {
        RBNode<Key, T> newNode = new RBNode<>(key, value, Color.RED, null);
        if (root == null) {
            root = newNode;
            root.color = Color.BLACK;
            size++;
            return;
        }
        RBNode<Key, T> current = root;
        RBNode<Key, T> parent = null;
        while (current != null) {
            parent = current;
            if (key.compareTo(current.key) < 0) {
                current = current.left;
            } else if (key.compareTo(current.key) > 0) {
                current = current.right;
            } else {
                current.value = value;
                return;
            }
        }
        newNode.parent = parent;
        if (key.compareTo(parent.key) < 0) {
            parent.left = newNode;
        } else {
            parent.right = newNode;
        }
        size++;
        fixInsert(newNode);
    }

    private void fixInsert(RBNode<Key, T> node) {
        while (node.parent != null && node.parent.color == Color.RED) {
            RBNode<Key, T> grandparent = node.parent.parent;
            if (node.parent == grandparent.left) {
                RBNode<Key, T> uncle = grandparent.right;
                if (uncle != null && uncle.color == Color.RED) {
                    node.parent.color = Color.BLACK;
                    uncle.color = Color.BLACK;
                    grandparent.color = Color.RED;
                    node = grandparent;
                } else {
                    if (node == node.parent.right) {
                        node = node.parent;
                        rotateLeft(node);
                    }
                    node.parent.color = Color.BLACK;
                    grandparent.color = Color.RED;
                    rotateRight(grandparent);
                }
            } else {
                RBNode<Key, T> uncle = grandparent.left;
                if (uncle != null && uncle.color == Color.RED) {
                    node.parent.color = Color.BLACK;
                    uncle.color = Color.BLACK;
                    grandparent.color = Color.RED;
                    node = grandparent;
                } else {
                    if (node == node.parent.left) {
                        node = node.parent;
                        rotateRight(node);
                    }
                    node.parent.color = Color.BLACK;
                    grandparent.color = Color.RED;
                    rotateLeft(grandparent);
                }
            }
        }
        root.color = Color.BLACK;
    }

    private void rotateLeft(RBNode<Key, T> node) {
        RBNode<Key, T> rightChild = node.right;
        node.right = rightChild.left;
        if (rightChild.left != null) {
            rightChild.left.parent = node;
        }
        rightChild.parent = node.parent;
        if (node.parent == null) {
            root = rightChild;
        } else if (node == node.parent.left) {
            node.parent.left = rightChild;
        } else {
            node.parent.right = rightChild;
        }
        rightChild.left = node;
        node.parent = rightChild;
    }

    private void rotateRight(RBNode<Key, T> node) {
        RBNode<Key, T> leftChild = node.left;
        node.left = leftChild.right;
        if (leftChild.right != null) {
            leftChild.right.parent = node;
        }
        leftChild.parent = node.parent;
        if (node.parent == null) {
            root = leftChild;
        } else if (node == node.parent.right) {
            node.parent.right = leftChild;
        } else {
            node.parent.left = leftChild;
        }
        leftChild.right = node;
        node.parent = leftChild;
    }

    public boolean remove(Key key) {
        RBNode<Key, T> node = findNode(key);
        if (node == null) return false;

        deleteNode(node);
        size--;
        return true;
    }

    private void deleteNode(RBNode<Key, T> node) {
        RBNode<Key, T> replacement, child;
        Color originalColor = node.color;

        if (node.left == null) {
            child = node.right;
            transplant(node, node.right);
        } else if (node.right == null) {
            child = node.left;
            transplant(node, node.left);
        } else {
            replacement = getMinNode(node.right);
            originalColor = replacement.color;
            child = replacement.right;
            if (replacement.parent == node) {
                if (child != null) {
                    child.parent = replacement;
                }
            } else {
                transplant(replacement, replacement.right);
                replacement.right = node.right;
                replacement.right.parent = replacement;
            }
            transplant(node, replacement);
            replacement.left = node.left;
            replacement.left.parent = replacement;
            replacement.color = node.color;
        }

        if (originalColor == Color.BLACK) {
            fixDelete(child, node.parent);
        }
    }

    private void fixDelete(RBNode<Key, T> node, RBNode<Key, T> parent) {
        while (node != root && (node == null || node.color == Color.BLACK)) {
            if (node == parent.left) {
                RBNode<Key, T> sibling = parent.right;

                if (sibling.color == Color.RED) {
                    sibling.color = Color.BLACK;
                    parent.color = Color.RED;
                    rotateLeft(parent);
                    sibling = parent.right;
                }

                if ((sibling.left == null || sibling.left.color == Color.BLACK) &&
                        (sibling.right == null || sibling.right.color == Color.BLACK)) {
                    sibling.color = Color.RED;
                    node = parent;
                    parent = node.parent;
                } else {
                    if (sibling.right == null || sibling.right.color == Color.BLACK) {
                        if (sibling.left != null) {
                            sibling.left.color = Color.BLACK;
                        }
                        sibling.color = Color.RED;
                        rotateRight(sibling);
                        sibling = parent.right;
                    }
                    sibling.color = parent.color;
                    parent.color = Color.BLACK;
                    if (sibling.right != null) {
                        sibling.right.color = Color.BLACK;
                    }
                    rotateLeft(parent);
                    node = root;
                }
            } else {
                RBNode<Key, T> sibling = parent.left;

                if (sibling.color == Color.RED) {
                    sibling.color = Color.BLACK;
                    parent.color = Color.RED;
                    rotateRight(parent);
                    sibling = parent.left;
                }

                if ((sibling.right == null || sibling.right.color == Color.BLACK) &&
                        (sibling.left == null || sibling.left.color == Color.BLACK)) {
                    sibling.color = Color.RED;
                    node = parent;
                    parent = node.parent;
                } else {
                    if (sibling.left == null || sibling.left.color == Color.BLACK) {
                        if (sibling.right != null) {
                            sibling.right.color = Color.BLACK;
                        }
                        sibling.color = Color.RED;
                        rotateLeft(sibling);
                        sibling = parent.left;
                    }
                    sibling.color = parent.color;
                    parent.color = Color.BLACK;
                    if (sibling.left != null) {
                        sibling.left.color = Color.BLACK;
                    }
                    rotateRight(parent);
                    node = root;
                }
            }
        }
        if (node != null) {
            node.color = Color.BLACK;
        }
    }

    private RBNode<Key, T> getMinNode(RBNode<Key, T> node) {
        while (node != null && node.left != null) {
            node = node.left;
        }
        return node;
    }


    private void transplant(RBNode<Key, T> u, RBNode<Key, T> v) {
        if (u.parent == null) {
            root = v;
        } else if (u == u.parent.left) {
            u.parent.left = v;
        } else {
            u.parent.right = v;
        }
        if (v != null) {
            v.parent = u.parent;
        }
    }


    private RBNode<Key, T> findNode(Key key) {
        RBNode<Key, T> current = root;
        while (current != null) {
            int cmp = key.compareTo(current.key);
            if (cmp < 0) {
                current = current.left;
            } else if (cmp > 0) {
                current = current.right;
            } else {
                return current;
            }
        }
        return null;
    }

    @Override
    public Iterator<RBNode<Key, T>> iterator() {
        return new RBTreeIterator();
    }

    private class RBTreeIterator implements Iterator<RBNode<Key, T>> {
        private RBNode<Key, T> current = getMinNode(root);

        @Override
        public boolean hasNext() {
            return current != null;
        }

        @Override
        public RBNode<Key, T> next() {
            if (!hasNext()) {
                throw new NoSuchElementException();
            }
            RBNode<Key, T> node = current;
            current = getSuccessor(current);
            return node;
        }

        private RBNode<Key, T> getMinNode(RBNode<Key, T> node) {
            while (node != null && node.left != null) {
                node = node.left;
            }
            return node;
        }

        private RBNode<Key, T> getSuccessor(RBNode<Key, T> node) {
            if (node.right != null) {
                return getMinNode(node.right);
            }
            RBNode<Key, T> parent = node.parent;
            while (parent != null && node == parent.right) {
                node = parent;
                parent = parent.parent;
            }
            return parent;
        }
    }
}

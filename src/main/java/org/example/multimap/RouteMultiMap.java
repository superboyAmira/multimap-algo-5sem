package org.example.multimap;

import org.example.multimap.list.DoublyLinkedList;
import org.example.multimap.rbtree.RBNode;
import org.example.multimap.rbtree.RBTree;

import java.util.Collection;


public class RouteMultiMap<Key extends Comparable<Key>, T> {
    private final RBTree<Key, DoublyLinkedList<T>> tree;
    private int size;

    public RouteMultiMap() {
        this.tree = new RBTree<>();
        this.size = 0;
    }

    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public boolean containsKey(Key key) {
        return tree.containsKey(key);
    }

    public boolean containsValue(T value) {
        for (RBNode<Key, DoublyLinkedList<T>> node : tree) {
            if (node.getValue().contains(value)) {
                return true;
            }
        }
        return false;
    }

    public DoublyLinkedList<T> get(Key key) {
        DoublyLinkedList<T> values = tree.get(key);
        return values != null ? values : new DoublyLinkedList<>();
    }

    public void put(Key key, T value) {
        DoublyLinkedList<T> values = tree.get(key);
        if (values == null) {
            values = new DoublyLinkedList<>();
            tree.insert(key, values);
        }
        values.addLast(value);
        size++;
    }

    public void putAll(Key key, Collection<T> values) {
        for (T value : values) {
            put(key, value);
        }
    }

    public boolean remove(Key key, T value) {
        DoublyLinkedList<T> values = tree.get(key);
        if (values != null && values.remove(value)) {
            size--;
            if (values.getSize() == 0) {
                tree.remove(key);
            }
            return true;
        }
        return false;
    }

    public DoublyLinkedList<T> removeAll(Key key) {
        DoublyLinkedList<T> values = tree.get(key);
        if (values != null) {
            tree.remove(key);
            size -= values.getSize();
            return values;
        }
        return new DoublyLinkedList<>();
    }

    public void clear() {
        tree.clear();
        size = 0;
    }

    public DoublyLinkedList<T> values() {
        DoublyLinkedList<T> allValues = new DoublyLinkedList<>();
        for (RBNode<Key, DoublyLinkedList<T>> node : tree) {
            allValues.addAll(node.getValue());
        }
        return allValues;
    }
}


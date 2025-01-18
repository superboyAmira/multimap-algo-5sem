package org.example.multimap.rbtree;

public class RBNode<Key extends Comparable<Key>, T> {
    Key key;
    T value;
    RBNode<Key, T> left, right, parent;
    Color color;

    RBNode(Key key, T value, Color color, RBNode<Key, T> parent) {
        this.key = key;
        this.value = value;
        this.color = color;
        this.parent = parent;
        this.left = null;
        this.right = null;
    }

    public T getValue() {
        return value;
    }

    public void setValue(T value) {
        this.value = value;
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        this.color = color;
    }
}
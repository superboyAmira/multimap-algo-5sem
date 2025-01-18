package org.example.multimap.rbtree;

import java.util.Comparator;

public class RedBlackTree<T> {
    private static class Node<T> {
        T data;
        Node<T> left, right, parent;
        boolean isRed;

        public Node(T data) {
            this.data = data;
            this.isRed = true;
        }
    }

}


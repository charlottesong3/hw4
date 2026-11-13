#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item);
    virtual void remove(const Key& key);
protected:
    using BinarySearchTree<Key, Value>::nodeSwap;
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Balance is height(right subtree) - height(left subtree).
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* parent = NULL;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

    // Find where the new node belongs.
    while(current != NULL) {
        parent = current;
        if(new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else if(current->getKey() < new_item.first) {
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(
        new_item.first, new_item.second, parent);
    if(parent == NULL) {
        this->root_ = node;
        return;
    }
    if(new_item.first < parent->getKey()) parent->setLeft(node);
    else parent->setRight(node);

    // Walk upward, updating balances until the height stops changing.
    while(parent != NULL) {
        if(node == parent->getLeft()) parent->updateBalance(-1);
        else parent->updateBalance(1);

        if(parent->getBalance() == 0) return;
        if(parent->getBalance() == -2) {
            if(parent->getLeft()->getBalance() == 1) {
                rotateLeft(parent->getLeft());
            }
            rotateRight(parent);
            return;
        }
        if(parent->getBalance() == 2) {
            if(parent->getRight()->getBalance() == -1) {
                rotateRight(parent->getRight());
            }
            rotateLeft(parent);
            return;
        }
        node = parent;
        parent = parent->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* target =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(target == NULL) return;

    if(target->getLeft() != NULL && target->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            this->predecessor(target));
        nodeSwap(target, pred);
    }

    AVLNode<Key, Value>* parent = target->getParent();
    AVLNode<Key, Value>* child = target->getLeft();
    if(child == NULL) child = target->getRight();
    int8_t diff = 0;

    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == target) {
        parent->setLeft(child);
        diff = 1;  // The left subtree became shorter.
    }
    else {
        parent->setRight(child);
        diff = -1; // The right subtree became shorter.
    }
    if(child != NULL) child->setParent(parent);
    delete target;

    // Deletion can require repairs at several ancestors.
    AVLNode<Key, Value>* node = parent;
    while(node != NULL) {
        node->updateBalance(diff);
        if(node->getBalance() == -1 || node->getBalance() == 1) return;

        if(node->getBalance() == -2) {
            if(node->getLeft()->getBalance() > 0) {
                rotateLeft(node->getLeft());
            }
            node = rotateRight(node);
        }
        else if(node->getBalance() == 2) {
            if(node->getRight()->getBalance() < 0) {
                rotateRight(node->getRight());
            }
            node = rotateLeft(node);
        }

        // A nonzero balance after rotation means the height did not shrink.
        if(node->getBalance() != 0) return;
        parent = node->getParent();
        if(parent == NULL) return;
        if(node == parent->getLeft()) diff = 1;
        else diff = -1;
        node = parent;
    }
}

// Rotations reconnect the parent/root, move the middle subtree, and update
// balances in constant time without traversing either subtree.
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = pivot->getLeft();

    if(parent == NULL) this->root_ = pivot;
    else if(parent->getLeft() == node) parent->setLeft(pivot);
    else parent->setRight(pivot);
    pivot->setParent(parent);

    node->setRight(middle);
    if(middle != NULL) middle->setParent(node);
    pivot->setLeft(node);
    node->setParent(pivot);

    int nodeBalance = node->getBalance() - 1;
    if(pivot->getBalance() > 0) nodeBalance -= pivot->getBalance();
    int pivotBalance = pivot->getBalance() - 1;
    if(nodeBalance < 0) pivotBalance += nodeBalance;
    node->setBalance(static_cast<int8_t>(nodeBalance));
    pivot->setBalance(static_cast<int8_t>(pivotBalance));
    return pivot;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = pivot->getRight();

    if(parent == NULL) this->root_ = pivot;
    else if(parent->getLeft() == node) parent->setLeft(pivot);
    else parent->setRight(pivot);
    pivot->setParent(parent);

    node->setLeft(middle);
    if(middle != NULL) middle->setParent(node);
    pivot->setRight(node);
    node->setParent(pivot);

    int nodeBalance = node->getBalance() + 1;
    if(pivot->getBalance() < 0) nodeBalance -= pivot->getBalance();
    int pivotBalance = pivot->getBalance() + 1;
    if(nodeBalance > 0) pivotBalance += nodeBalance;
    node->setBalance(static_cast<int8_t>(nodeBalance));
    pivot->setBalance(static_cast<int8_t>(pivotBalance));
    return pivot;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif

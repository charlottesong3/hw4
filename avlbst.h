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

    // right height minus left height
    // these two helpers do the tree spins :3
    AVLNode<Key, Value>* derpLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* derpRight(AVLNode<Key, Value>* node);
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

    // first find a spot for the new node
    while (current != NULL) {
        parent = current;

        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else if (current->getKey() < new_item.first) {
            current = current->getRight();
        }
        else {
            // same key, so just change its value
            current->setValue(new_item.second);
            return;
        }
    }

    // make the new node and hook it to its parent
    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(
        new_item.first, new_item.second, parent);
    if (parent == NULL) {

        this->root_ = node;
        return;
    }
    if (new_item.first < parent->getKey()) {
        parent->setLeft(node);
    }
    else {
        parent->setRight(node);
    }

    // now go back up and fix any leaning nodes
    while (parent != NULL) {

        if (node == parent->getLeft()) {
            parent->updateBalance(-1);
        }
        else {
            parent->updateBalance(1);
        }

        if (parent->getBalance() == 0) {
            return;
        }
        if (parent->getBalance() == -2) {
            if (parent->getLeft()->getBalance() == 1) {
                derpLeft(parent->getLeft());
            }
            derpRight(parent);
            return;
        }
        if (parent->getBalance() == 2) {
            if (parent->getRight()->getBalance() == -1) {
                derpRight(parent->getRight());
            }
            derpLeft(parent);
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
    // nothing to remove if the key is missing
    if (target == NULL) {
        return;
    }

    // two kids: swap with the biggest node on the left first
    if (target->getLeft() != NULL && target->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            this->predecessor(target));
        nodeSwap(target, pred);
    }

    // now there is at most one kid to move up
    AVLNode<Key, Value>* parent = target->getParent();
    AVLNode<Key, Value>* child = target->getLeft();
    if (child == NULL) {
        child = target->getRight();
    }
    int8_t diff = 0;

    if (parent == NULL) {
        this->root_ = child;
    }
    else if (parent->getLeft() == target) {
        parent->setLeft(child);
        diff = 1;  // left side got shorter
    }
    else {
        parent->setRight(child);
        diff = -1; // right side got shorter
    }
    if (child != NULL) {
        child->setParent(parent);
    }

    delete target;

    // keep going up because removing can affect more than one parent
    AVLNode<Key, Value>* node = parent;
    while (node != NULL) {

        node->updateBalance(diff);
        if (node->getBalance() == -1 || node->getBalance() == 1) {
            return;
        }

        if (node->getBalance() == -2) {
            if (node->getLeft()->getBalance() > 0) {
                derpLeft(node->getLeft());
            }
            node = derpRight(node);
        }
        else if (node->getBalance() == 2) {
            if (node->getRight()->getBalance() < 0) {
                derpRight(node->getRight());
            }
            node = derpLeft(node);
        }

        // if the height stayed the same, we are done
        if (node->getBalance() != 0) {
            return;
        }
        parent = node->getParent();
        if (parent == NULL) {
            return;
        }
        if (node == parent->getLeft()) {
            diff = 1;
        }
        else {
            diff = -1;
        }

        node = parent;
    }
}

// spin left: the right buddy moves up
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::derpLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* buddy = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middleKid = buddy->getLeft();

    if (parent == NULL) {
        this->root_ = buddy;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(buddy);
    }
    else {
        parent->setRight(buddy);
    }
    buddy->setParent(parent);

    // pass the middle kid over, then connect the old root
    node->setRight(middleKid);
    if (middleKid != NULL) {
        middleKid->setParent(node);
    }
    buddy->setLeft(node);
    node->setParent(buddy);

    // update the two balances after the spin
    int nodeBalance = node->getBalance() - 1;
    if (buddy->getBalance() > 0) {
        nodeBalance -= buddy->getBalance();
    }
    int pivotBalance = buddy->getBalance() - 1;
    if (nodeBalance < 0) {
        pivotBalance += nodeBalance;
    }
    node->setBalance(static_cast<int8_t>(nodeBalance));
    buddy->setBalance(static_cast<int8_t>(pivotBalance));

    return buddy;
}

// spin right: the left buddy moves up
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::derpRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* buddy = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middleKid = buddy->getRight();

    if (parent == NULL) {
        this->root_ = buddy;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(buddy);
    }
    else {
        parent->setRight(buddy);
    }
    buddy->setParent(parent);

    // pass the middle kid over, then connect the old root
    node->setLeft(middleKid);
    if (middleKid != NULL) {
        middleKid->setParent(node);
    }
    buddy->setRight(node);
    node->setParent(buddy);

    // update the two balances after the spin
    int nodeBalance = node->getBalance() + 1;
    if (buddy->getBalance() < 0) {
        nodeBalance -= buddy->getBalance();
    }
    int pivotBalance = buddy->getBalance() + 1;
    if (nodeBalance > 0) {
        pivotBalance += nodeBalance;
    }
    node->setBalance(static_cast<int8_t>(nodeBalance));
    buddy->setBalance(static_cast<int8_t>(pivotBalance));

    return buddy;
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

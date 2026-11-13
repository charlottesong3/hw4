#ifndef RECCHECK
// Add any additional includes here.
#endif



#include "equal-paths.h"

namespace {
bool checkDerps(Node* node, int depth, int& leafDepth)
{
    if (node == nullptr) {
        return true; // base case
    }

    if (node->left == nullptr && node->right == nullptr) {

        if (leafDepth == -1) {


            leafDepth = depth;

        }

        // then recurse 
        return depth == leafDepth;
    }

    return checkDerps(node->left, depth + 1, leafDepth) &&
            checkDerps(node->right, depth + 1, leafDepth);
}
}

bool equalPaths(Node* root)
{
    int leafDepth = -1;
    // :3

    return checkDerps(root, 0, leafDepth);
}
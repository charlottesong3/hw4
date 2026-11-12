#ifndef RECCHECK
// Add any additional includes here.
#endif

#include "equal-paths.h"

namespace {
bool checkDepths(Node* node, int depth, int& leafDepth)
{
    if (node == nullptr) {
        return true;
    }

    if (node->left == nullptr && node->right == nullptr) {
        if (leafDepth == -1) {
            leafDepth = depth;
        }
        return depth == leafDepth;
    }

    return checkDepths(node->left, depth + 1, leafDepth) &&
           checkDepths(node->right, depth + 1, leafDepth);
}
}

bool equalPaths(Node* root)
{
    int leafDepth = -1;
    return checkDepths(root, 0, leafDepth);
}
#include <stdlib.h>
#include "bst.h"

BST* createBST(int (*compare)(void*, void*), void (*print)(void*), void (*freeData)(void*)) {
    BST* binarySearchTree = malloc(sizeof(BST));

    if (binarySearchTree == NULL)
        return NULL;

    binarySearchTree->root = NULL;
    binarySearchTree->compare = compare;
    binarySearchTree->print = print;
    binarySearchTree->freeData = freeData;

    return binarySearchTree;
}

BSTNode* bstInsert(BSTNode* root, void* data, int (*compare)(void*, void*)) {
    if (root == NULL) {
        root = malloc(sizeof(BSTNode));

        if (root == NULL) {
            return NULL;
        }

        root->left = NULL;
        root->right = NULL;
        root->data = data;

        return root;
    }

    if (compare(data, root->data) == 0)
        return root;

    if (compare(data, root->data) < 0) {
        root->left = bstInsert(root->left, data, compare);
    } else {
        root->right = bstInsert(root->right, data, compare);
    }

    return root;
};

void* bstFind(BSTNode* root, void* data, int (*compare)(void*, void*)) {
    if (root == NULL) {
        return NULL;
    }

    int compareValue = compare(data, root->data);

    if (compareValue == 0)
        return root->data;

    if (compareValue < 0) {
        return bstFind(root->left, data, compare);
    } else {
        return bstFind(root->right, data, compare);
    }
};

void bstPreorder(BSTNode* root, void (*print)(void*)) {
    if (root == NULL) {
        return;
    }

    print(root->data);
    bstPreorder(root->left, print);
    bstPreorder(root->right, print);
}

void bstInorder(BSTNode* root, void (*print)(void*)) {
    if (root == NULL) {
        return;
    }

    bstInorder(root->left, print);
    print(root->data);
    bstInorder(root->right, print);
}

void bstPostorder(BSTNode* root, void (*print)(void*)) {
    if (root == NULL) {
        return;
    }

    bstPostorder(root->left, print);
    bstPostorder(root->right, print);
    print(root->data);
}

void bstFree(BSTNode* root, void (*freeData)(void*)) {
    if (root == NULL) {
        return;
    }

    bstFree(root->left, freeData);
    bstFree(root->right, freeData);
    freeData(root->data);
    free(root);
};

void destroyBST(BST* binarySearchTree) {
    if (binarySearchTree == NULL) {
        return;
    }

    bstFree(binarySearchTree->root, binarySearchTree->freeData);
    free(binarySearchTree);
}
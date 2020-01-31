#include <iostream>


template <typename T>
class SearchTreeNode {
public:
    T value;
    SearchTreeNode* parent;
    SearchTreeNode* leftChild;
    SearchTreeNode* rightChild;


public:
    SearchTreeNode(T value) {
        this->value = value;
        this->parent = nullptr;
        this->leftChild = nullptr;
        this->rightChild = nullptr;
    }


// MARK: Queries
public:
    // Call this function on the root node to walk the entire tree.
    static void inorderTreeWalk(SearchTreeNode* currentNode) {
        if (currentNode) {
            SearchTreeNode::inorderTreeWalk(currentNode->leftChild);
            std::cout << currentNode->value << " " << std::flush;
            SearchTreeNode::inorderTreeWalk(currentNode->rightChild);
        }
    }

    // Done in O(h) time. `h` represents the tree's height.
    static SearchTreeNode* searchForValueRecursively(SearchTreeNode* rootNode, T value) {
        if (rootNode == nullptr) {
            return nullptr;
        }

        if (value == rootNode->value) {
            return rootNode;
        } else if (value < rootNode->value) {
            return searchForValueRecursively(rootNode->leftChild, value);
        } else {
            return searchForValueRecursively(rootNode->rightChild, value);
        }
    }

    static SearchTreeNode* searchForValueIteratively(SearchTreeNode* rootNode, T value) {
        auto currentNode = rootNode;
        while (currentNode != nullptr) {
            if (currentNode->value == value) {
                return currentNode;
            } else if (value < currentNode->value) {
                currentNode = currentNode->leftChild;
            } else {
                currentNode = currentNode->rightChild;
            }
        }
        
        return nullptr;
    }

    static SearchTreeNode* getMin(SearchTreeNode* rootNode) {
        if (rootNode == nullptr) {
            return nullptr;
        }

        auto currentNode = rootNode;
        while (currentNode->leftChild != nullptr) {
            currentNode = currentNode->leftChild;
        }

        return currentNode;
    }

    static SearchTreeNode* getMax(SearchTreeNode* rootNode) {
        if (rootNode == nullptr) {
            return nullptr;
        }

        auto currentNode = rootNode;
        while (currentNode->rightChild != nullptr) {
            currentNode = currentNode->rightChild;
        }

        return currentNode;
    }

    static SearchTreeNode* getPredecessor(SearchTreeNode* currentNode) {
        if (currentNode == nullptr) {
            return nullptr;
        }

        if (currentNode->leftChild) {
            return SearchTreeNode::getMin(currentNode->leftChild);
        }

        // Find the first ancestor with the current node as right child.
        auto ancestor = currentNode->parent;
        while ((ancestor != nullptr) && (ancestor->leftChild == currentNode)) {
            currentNode = ancestor;
            ancestor = ancestor->parent;
        }

        return ancestor;
    }

    static SearchTreeNode* getSuccessor(SearchTreeNode* currentNode) {
        if (currentNode == nullptr) {
            return nullptr;
        }

        if (currentNode->rightChild) {
            return SearchTreeNode::getMax(currentNode->rightChild);
        }

        // Find the first ancestor with the current node as left child.
        auto ancestor = currentNode->parent;
        while ((ancestor != nullptr) && (ancestor->rightChild == currentNode)) {
            currentNode = ancestor;
            ancestor = ancestor->parent;
        }

        return ancestor;
    }


// MARK: Insertions
public:
    // The inserted node is surely a leaf node.
    static SearchTreeNode* insertRecursively(SearchTreeNode* rootNode, const T& newValue) {
        if (rootNode == nullptr) {
            return nullptr;
        }

        if (newValue <= rootNode->value) {
            if (rootNode->leftChild == nullptr) {
                auto newNode = new SearchTreeNode(newValue);
                rootNode->leftChild = newNode;
                newNode->parent = rootNode;
                return newNode;
            } else {
                insertRecursively(rootNode->leftChild, newValue);
            }
        } else {
            if (rootNode->rightChild == nullptr) {
                auto newNode = new SearchTreeNode(newValue);
                rootNode->rightChild = newNode;
                newNode->parent = rootNode;
                return newNode;
            } else {
                insertRecursively(rootNode->rightChild, newValue);
            }
        }

        return nullptr;
    }

    static SearchTreeNode* insertIteratively(SearchTreeNode* rootNode, const T& newValue) {
        // This function does not handle an empty tree.
        if (rootNode == nullptr) {
            return nullptr;
        }

        auto currentNode = rootNode;
        auto parentNode = rootNode;

        while (currentNode != nullptr) {
            parentNode = currentNode;
            if (newValue <= currentNode->value) {
                currentNode = currentNode->leftChild;
            } else {
                currentNode = currentNode->rightChild;
            }
        }

        auto newNode = new SearchTreeNode(newValue);
        if (newValue <= parentNode->value) {
            parentNode->leftChild = newNode;
            newNode->parent = parentNode;
        } else {
            parentNode->rightChild = newNode;
            newNode->parent = parentNode;
        }
        return newNode;
    }


// MARK: - Deletion
private:
    static void transplantSubtree(SearchTreeNode** rootNode, SearchTreeNode* oldSubtree, SearchTreeNode* newSubtree) {
        if (oldSubtree->parent == nullptr) {
            // The old subtree is the root.
            *rootNode = newSubtree;
            return;
        }

        if (oldSubtree == oldSubtree->parent->leftChild) {
            oldSubtree->parent->leftChild = newSubtree;
        } else {
            oldSubtree->parent->rightChild = newSubtree;
        }

        if (newSubtree) {
            newSubtree->parent = oldSubtree->parent;
        }
    }

public:
    static void deleteNode(SearchTreeNode** rootNode, SearchTreeNode* nodeToDelete) {
        // Note that `nodeToDelete` might be the root node.
        if (nodeToDelete->leftChild == nullptr) {
            // Simplest case. Use right node.
            SearchTreeNode<int>::transplantSubtree(rootNode, nodeToDelete, nodeToDelete->rightChild);
        } else if (nodeToDelete->rightChild == nullptr) {
            // Only has left child. Also simple.
            SearchTreeNode<int>::transplantSubtree(rootNode, nodeToDelete, nodeToDelete->leftChild);
        } else {
            // Has both left and right children.
            // Find the successor of `nodeToDelete` from its right subtree.
            auto replacementNode = SearchTreeNode<int>::getMin(nodeToDelete->rightChild);
            if (replacementNode->parent != nodeToDelete) {
                // `replacementNode` is not the direct right child of `nodeToDelete`.
                // Apparently `replacementNode` has no left child, but may have a right child.
                // Thus, we need to deal with `replacementNode`'s right child.
                SearchTreeNode<int>::transplantSubtree(rootNode, replacementNode, replacementNode->rightChild);
                replacementNode->rightChild = nodeToDelete->rightChild;
                replacementNode->rightChild->parent = replacementNode;
            }
            // If `replacementNode` is the direct right child of `nodeToDelete`, we don't need to care about its right child.

            SearchTreeNode<int>::transplantSubtree(rootNode, nodeToDelete, replacementNode);
            replacementNode->leftChild = nodeToDelete->leftChild;
            replacementNode->leftChild->parent = replacementNode;
        }
    }
};


// template <typename T>
// class SearchTree {
// public:
//     SearchTreeNode<T>* rootNode;

// public:
//     SearchTree(T rootValue) {
//         rootNode = new SearchTreeNode<T>(rootValue);
//     }


// // MARK: Queries
// public:
//     void inorderTreeWalk() {
//         if (rootNode) {
//             SearchTreeNode::inorderTreeWalk(rootNode->leftChild);
//             std::cout << rootNode->value << " " << std::flush;
//             SearchTreeNode::inorderTreeWalk(rootNode->rightChild);
//         }
//     }
// };


int main() {
    auto rootNode = new SearchTreeNode<int>(8);

    // SearchTreeNode<int>::insertRecursively(rootNode, 6);
    // SearchTreeNode<int>::insertRecursively(rootNode, 12);
    // SearchTreeNode<int>::insertRecursively(rootNode, 2);
    // SearchTreeNode<int>::insertRecursively(rootNode, 4);
    SearchTreeNode<int>::insertIteratively(rootNode, 6);
    SearchTreeNode<int>::insertIteratively(rootNode, 12);
    SearchTreeNode<int>::insertIteratively(rootNode, 2);
    SearchTreeNode<int>::insertIteratively(rootNode, 4);

    SearchTreeNode<int>::inorderTreeWalk(rootNode);
    std::cout << std::endl;

    auto searchResult1 = SearchTreeNode<int>::searchForValueIteratively(rootNode, 8);
    if (searchResult1) {
        std::cout << "Result 1: " << searchResult1->value << std::endl;
    }
    SearchTreeNode<int>::deleteNode(&rootNode, searchResult1);
    auto searchResult2 = SearchTreeNode<int>::searchForValueIteratively(rootNode, 8);
    if (searchResult2) {
        std::cout << "Result 2: " << searchResult2->value << std::endl;
    }
    SearchTreeNode<int>::inorderTreeWalk(rootNode);
    std::cout << std::endl;

    auto searchResult3 = SearchTreeNode<int>::searchForValueIteratively(rootNode, 4);
    if (searchResult3) {
        std::cout << "Result 3: " << searchResult3->value << std::endl;
    }
    SearchTreeNode<int>::deleteNode(&rootNode, searchResult3);
    auto searchResult4 = SearchTreeNode<int>::searchForValueIteratively(rootNode, 4);
    if (searchResult4) {
        std::cout << "Result 4: " << searchResult3->value << std::endl;
    }
    SearchTreeNode<int>::inorderTreeWalk(rootNode);
    std::cout << std::endl;

    return 0;
}

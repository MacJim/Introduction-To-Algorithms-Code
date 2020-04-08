#include <iostream>
#include <vector>
#include <stack>
#include <memory>
#include <random>
#include <chrono>


class RBNode {
public:
    int value;

    RBNode* parent;
    RBNode* leftChild;
    RBNode* rightChild;

    bool isRed;

public:
    /// Black-colored sentinel `nil` node. Initialization after class definition.
    static RBNode* nilNode;

public:
    RBNode(int value, bool isRed) {
        this->value = value;
        this->parent = RBNode::nilNode;
        this->leftChild = RBNode::nilNode;
        this->rightChild = RBNode::nilNode;
        this->isRed = isRed;
    }

    RBNode(int value, RBNode* parent, RBNode* leftChild, RBNode* rightChild, bool isRed) {
        this->value = value;
        this->parent = parent;
        this->leftChild = leftChild;
        this->rightChild = rightChild;
        this->isRed = isRed;
    }
};

RBNode* RBNode::nilNode = new RBNode(0, nullptr, nullptr, nullptr, false);


class RBTree {
public:
    RBNode* rootNode;

public:
    RBTree() {
        rootNode = RBNode::nilNode;
        // nilNode = new RBNode(0, false);
    }


#pragma mark Walk
private:
    static void inOrderWalkRecursively(RBNode* currentNode, std::vector<int>& returnValue) {
        if (currentNode == RBNode::nilNode) {
            return;
        }

        inOrderWalkRecursively(currentNode->leftChild, returnValue);
        returnValue.push_back(currentNode->value);
        inOrderWalkRecursively(currentNode->rightChild, returnValue);
    }

public:
    std::vector<int> inOrderWalk() {
        if (rootNode == RBNode::nilNode) {
            return {};
        }

        auto returnValue = std::vector<int>();

        inOrderWalkRecursively(rootNode, returnValue);
        
        return returnValue;
    }


#pragma mark Min & Max
public:
    static RBNode* getMinNodeOfSubtree(RBNode* rootNode) {
        if (rootNode == RBNode::nilNode) {
            return RBNode::nilNode;
        }

        auto currentNode = rootNode;
        while (currentNode->leftChild != RBNode::nilNode) {
            currentNode = currentNode->leftChild;
        }

        return currentNode;
    }

    static RBNode* getMaxNodeOfSubtree(RBNode* rootNode) {
        if (rootNode == RBNode::nilNode) {
            return RBNode::nilNode;
        }

        auto currentNode = rootNode;
        while (currentNode->rightChild != RBNode::nilNode) {
            currentNode = currentNode->rightChild;
        }

        return currentNode;
    }

public:
    RBNode* getMinNode() {
        return RBTree::getMinNodeOfSubtree(rootNode);
    }

    RBNode* getMaxNode() {
        return RBTree::getMaxNodeOfSubtree(rootNode);
    }


#pragma mark Search
public:
    RBNode* searchForValue(int value) {
        auto currentNode = rootNode;
        while (currentNode != RBNode::nilNode) {
            if (currentNode->value == value) {
                return currentNode;
            } else if (currentNode->value > value) {
                currentNode = currentNode->leftChild;
            } else {
                currentNode = currentNode->rightChild;
            }
        }
        
        return RBNode::nilNode;
    }


#pragma mark Predecessor & Successor
public:
    static RBNode* getPredecessor(RBNode* node) {
        if (node == RBNode::nilNode) {
            return RBNode::nilNode;
        }

        if (node->leftChild != RBNode::nilNode) {
            return RBTree::getMaxNodeOfSubtree(node->leftChild);
        }

        // Find the first ancestor with the current node as right child.
        auto currentNode = node;
        auto ancestor = node->parent;
        while ((ancestor != RBNode::nilNode) && (ancestor->leftChild == currentNode)) {
            currentNode = ancestor;
            ancestor = ancestor->parent;
        }
        
        return ancestor;
    }

    static RBNode* getSuccessor(RBNode* node) {
        if (node == RBNode::nilNode) {
            return RBNode::nilNode;
        }

        if (node->rightChild != RBNode::nilNode) {
            return RBTree::getMinNodeOfSubtree(node->rightChild);
        }

        // Find the first ancestor with the current node as left child.
        auto currentNode = node;
        auto ancestor = node->parent;
        while ((ancestor != RBNode::nilNode) && (ancestor->rightChild == currentNode)) {
            currentNode = ancestor;
            ancestor = ancestor->parent;
        }
        
        return ancestor;
    }


#pragma mark Rotation
private:
    /// Refer to page 334 of "Introduction to Algorithms".
    void rotateLeft(RBNode* x) {
        auto y = x->rightChild;
        
        // Move beta.
        x->rightChild = y->leftChild;
        if (y->leftChild != RBNode::nilNode) {
            y->leftChild->parent = x;
        }

        // Move x and y.
        y->parent = x->parent;
        if (x->parent == RBNode::nilNode) {
            rootNode = y;
        } else {
            if (x == x->parent->leftChild) {
                x->parent->leftChild = y;
            } else {
                x->parent->rightChild = y;
            }
        }
        
        x->parent = y;
        y->leftChild = x;
    }

    void rotateRight(RBNode* y) {
        auto x = y->leftChild;

        // Move beta.
        y->leftChild = x->rightChild;
        if (x->rightChild != RBNode::nilNode) {
            x->rightChild->parent = y;
        }

        // Move x and y.
        x->parent = y->parent;
        if (y->parent == RBNode::nilNode) {
            rootNode = x;
        } else {
            if (y == y->parent->leftChild) {
                y->parent->leftChild = x;
            } else {
                y->parent->rightChild = x;
            }
        }

        y->parent = x;
        x->rightChild = y;
    }


#pragma mark Insertion
private:
    /**
     * @param z The newly inserted red node.
     */
    void fixUpInsertion(RBNode* z) {
        // Root node's parent is RBNode::nilNode, which is black.
        while (z->parent->isRed) {
            // Both z and z->parent are red.
            // z->parent->parent must be black when z->parent is a red node.
            if (z->parent == z->parent->parent->leftChild) {
                // z's parent on left, uncle on right.
                
                /// Uncle of z.
                auto y = z->parent->parent->rightChild;

                if (y->isRed) {
                    // Case 1. Parent and uncle are red.
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;

                    continue;
                } else {
                    if (z == z->parent->rightChild) {
                        // Case 2. z is the right child.
                        // Rotate and treat z's parent as the new z.
                        z = z->parent;
                        RBTree::rotateLeft(z);
                    }

                    // Case 3.
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    RBTree::rotateRight(z->parent->parent);

                    break;
                }
            } else {
                // z's parent on right, uncle on left.

                /// Uncle of z.
                auto y = z->parent->parent->leftChild;

                if (y->isRed) {
                    // Case 1. Parent and uncle are red.
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;

                    continue;
                } else {
                    if (z == z->parent->leftChild) {
                        // Case 2. z is the left child.
                        // Rotate and treat z's parent as the new z.
                        z = z->parent;
                        RBTree::rotateRight(z);
                    }

                    // Case 3.
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    RBTree::rotateLeft(z->parent->parent);

                    break;
                }
            }
        }

        // Red node propagation may convert the root node into a red node.
        // In this case, we simply set it to black.
        rootNode->isRed = false;
    }

public:
    RBNode* insertValue(int newValue) {
        // 1. Create the new node.
        // The new node is by default red.
        auto newNode = new RBNode(newValue, true);

        // 2. Insert the new node.
        if (rootNode == RBNode::nilNode) {
            rootNode = newNode;
            rootNode->isRed = false;
            return newNode;
        }

        auto parentNode = RBNode::nilNode;
        auto currentNode = rootNode;

        while (currentNode != RBNode::nilNode) {
            parentNode = currentNode;

            if (newValue <= currentNode->value) {
                currentNode = currentNode->leftChild;
            } else {
                currentNode = currentNode->rightChild;
            }
        }

        newNode->parent = parentNode;
        if (newValue <= parentNode->value) {
            parentNode->leftChild = newNode;
        } else {
            parentNode->rightChild = newNode;
        }

        // 3. Fix up colors.
        fixUpInsertion(newNode);

        return newNode;
    }


#pragma mark - Deletion
private:
    void transplantDuringDeletion(RBNode* oldNode, RBNode* newNode) {
        if (oldNode->parent == RBNode::nilNode) {
            rootNode = newNode;
        } else if (oldNode == oldNode->parent->leftChild) {
            oldNode->parent->leftChild = newNode;
        } else {
            oldNode->parent->rightChild = newNode;
        }

        newNode->parent = oldNode->parent;    // This also assigns the shared sentinel as the parent of the root node.
    }

    void fixUpDeletion(RBNode* x) {
        while ((x != rootNode) && (!x->isRed)) {
            if (x == x->parent->leftChild) {
                /// Called `w` in the textbook.
                auto sibling = x->parent->rightChild;
                
                if (sibling->isRed) {
                    // Case 1 in textbook.
                    // Preprocess. Rotate to produce a black sibling.
                    // Sibling is red. Thus parent must be black. Sibling's children must be black.

                    sibling->isRed = false;
                    x->parent->isRed = true;
                    rotateLeft(x->parent);
                    // The new black sibling.
                    sibling = x->parent->rightChild;
                }

                if ((!sibling->leftChild->isRed) && (!sibling->rightChild->isRed)) {
                    // Case 2 in textbook.
                    sibling->isRed = true;
                    x = x->parent;
                    continue;
                } else {
                    if (!sibling->rightChild->isRed) {
                        // Case 3 in textbook.
                        // Preprocess. Turns the sibling's right child into a red node.
                        sibling->leftChild->isRed = false;
                        sibling->isRed = true;
                        rotateRight(sibling);
                        sibling = x->parent->rightChild;
                    }

                    // Case 4 in textbook.
                    sibling->isRed = x->parent->isRed;
                    x->parent->isRed = false;    // This adds an additional black node to the left subtree.
                    sibling->rightChild->isRed = false;    // Adds a new black node on the right subtree as compensation.
                    rotateLeft(x->parent);
                    
                    x = rootNode;    // This makes no sense but to terminate the while loop...
                }
            } else {
                auto sibling = x->parent->leftChild;

                if (sibling->isRed) {
                    sibling->isRed = false;
                    x->parent->isRed = true;
                    rotateRight(x->parent);
                    sibling = x->parent->leftChild;
                }

                if ((!sibling->leftChild->isRed) && (!sibling->rightChild->isRed)) {
                    sibling->isRed = true;
                    x = x->parent;
                    continue;
                } else {
                    if (!sibling->leftChild->isRed) {
                        sibling->rightChild->isRed = false;
                        sibling->isRed = true;
                        rotateLeft(sibling);
                        sibling = x->parent->leftChild;
                    }

                    sibling->isRed = x->parent->isRed;
                    x->parent->isRed = false;
                    sibling->leftChild->isRed = false;
                    rotateRight(x->parent);

                    x = rootNode;
                }
            }
        }

        x->isRed = false;    // x is either root or a red node that compensates for black loss.
    }

public:
    void deleteNode(RBNode* z) {
        /// The node that moves into `y`'s original location.
        RBNode* x = nullptr;

        /**
         * The removed node or the replacement node, depending on the case.
         * 
         * If this is a black node, we'll need to re-evaluate the Red Black Tree properties.
         */
        auto y = z;
        bool isYOriginallyRed = y->isRed;

        if (z->leftChild == RBNode::nilNode) {
            // y represents the removed node (the same as z).
            x = z->rightChild;
            transplantDuringDeletion(z, z->rightChild);
        } else if (z->rightChild == RBNode::nilNode) {
            // y represents the removed node (the same as z).
            x = z->leftChild;
            transplantDuringDeletion(z, z->leftChild);
        } else {
            // y represents the node that replaces z.
            y = RBTree::getMinNodeOfSubtree(z->rightChild);
            isYOriginallyRed = y->isRed;

            // We are sure here that y must not have a left child.
            // So x here is either y's right child or the nil sentinel.
            x = y->rightChild;

            if (y->parent == z) {
                x->parent = y;
            } else {
                transplantDuringDeletion(y, y->rightChild);
                y->rightChild = z->rightChild;
                y->rightChild->parent = y;
            }

            transplantDuringDeletion(z, y);
            y->leftChild = z->leftChild;
            y->leftChild->parent = y;
            y->isRed = z->isRed;
        }

        if (!isYOriginallyRed) {
            fixUpDeletion(x);
        }

        delete z;
    }

    bool deleteValue(int value) {
        auto node = searchForValue(value);
        if (node == RBNode::nilNode) {
            return false;
        } else {
            deleteNode(node);
            return true;
        }
    }
};


#pragma mark - Helpers
void printVector(std::vector<int> v) {
    for (const int& num: v) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}


#pragma mark - Tests
#pragma mark Rotation
void testRotation() {
}

#pragma mark Insertion
void testInsertion1() {
    auto randomSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto generator = std::default_random_engine(randomSeed);

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::shuffle(nums.begin(), nums.end(), generator);

    // auto tree = std::unique_ptr<RBTree>(new RBTree());
    auto tree = new RBTree();
    for (const int& num: nums) {
        auto newNode = tree->insertValue(num);
        std::cout << "New value: " << newNode->value << std::endl;

        auto result = tree->inOrderWalk();
        printVector(result);
    }

    std::cout << "Min value: " << tree->getMinNode()->value << std::endl;
    std::cout << "Max value: " << tree->getMaxNode()->value << std::endl;

    auto node6 = tree->searchForValue(6);
    auto predecessor = tree->getPredecessor(node6);
    auto successor = tree->getSuccessor(node6);
    std::cout << "Found value 6: " << node6->value << std::endl;
    std::cout << "Predecessor: " << predecessor->value << std::endl;
    std::cout << "Successor: " << successor->value << std::endl;

    delete tree;
}

void testInsertion2() {
    auto distribution = std::uniform_int_distribution(1, 21);

    auto randomSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto generator = std::default_random_engine(randomSeed);

    auto dice = std::bind(distribution, generator);

    auto tree = new RBTree();

    for (int i = 0; i < 20; i += 1) {
        const auto num = dice();
        auto newNode = tree->insertValue(num);

        std::cout << "New value: (" << num << ", " << newNode->value << ")" << std::endl;

        auto result = tree->inOrderWalk();
        printVector(result);
    }

    delete tree;
}

#pragma mark Insertion and Deletion
void testInsertionAndDeletion() {
    auto tree = new RBTree();

    auto nums = std::vector<int>(1000);
    std::iota(nums.begin(), nums.end(), 1);    // 1 ~ 1000

    auto randomSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto generator = std::default_random_engine(randomSeed);

    for (int i = 0; i < 100; i += 1) {
        auto numsCopy = nums;

        std::shuffle(numsCopy.begin(), numsCopy.end(), generator);
        for (const auto& num: numsCopy) {
            tree->insertValue(num);
            // printVector(tree->inOrderWalk());
        }
        auto result1 = tree->inOrderWalk();
        if (result1 == nums) {
            std::cout << "Insertion success! ";
        } else {
            std::cout << "Insertion failed. ";
        }

        std::shuffle(numsCopy.begin(), numsCopy.end(), generator);
        for (const auto& num: numsCopy) {
            tree->deleteValue(num);
            // printVector(tree->inOrderWalk());
        }
        auto result2 = tree->inOrderWalk();
        if (result2.size() == 0) {
            std::cout << "Deletion success!" << std::endl;
        } else {
            std::cout << "Deletion failed." << std::endl;
        }
    }

    delete tree;
}


int main() {
    // auto tree = new RBTree();
    // std::cout << RBNode::nilNode->isRed << std::endl;
    // testInsertion1();
    testInsertionAndDeletion();

    return 0;
}

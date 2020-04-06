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
    RBNode* insert(int newValue) {
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
        auto newNode = tree->insert(num);
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
        auto newNode = tree->insert(num);

        std::cout << "New value: (" << num << ", " << newNode->value << ")" << std::endl;

        auto result = tree->inOrderWalk();
        printVector(result);
    }

    delete tree;
}


int main() {
    // auto tree = new RBTree();
    // std::cout << RBNode::nilNode->isRed << std::endl;
    testInsertion1();

    return 0;
}

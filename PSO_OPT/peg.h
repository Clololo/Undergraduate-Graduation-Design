#ifndef PEG_H
#define PEG_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

class Node {
public:
    int matrixIdx;
    int id;
    std::string label;
    std::vector<Node*> connections;
    std::string group;

    Node(int idx, int identifier, const std::string& lbl, const std::string& grp)
        : matrixIdx(idx), id(identifier), label(lbl), group(grp) {}
};

class TannerGraph {
public:
    std::vector<std::vector<int>> matrix;
    std::vector<Node*> symbolNodes;
    std::vector<Node*> checkNodes;
    std::vector<std::pair<int, int>> edges;

    TannerGraph(const std::vector<std::vector<int>>& mat) : matrix(mat) {
        // Create symbol nodes
        for (size_t i = 0; i < matrix[0].size(); ++i) {
            symbolNodes.push_back(new Node(i, i, "S" + std::to_string(i), "symbol"));
        }

        // Create check nodes
        for (size_t i = 0; i < matrix.size(); ++i) {
            checkNodes.push_back(new Node(i, i + matrix[0].size(), "C" + std::to_string(i), "check"));
        }

        // Create edges based on the parity check matrix
        for (size_t i = 0; i < matrix.size(); ++i) {
            for (size_t j = 0; j < matrix[i].size(); ++j) {
                if (matrix[i][j] == 1) {
                    checkNodes[i]->connections.push_back(symbolNodes[j]);
                    symbolNodes[j]->connections.push_back(checkNodes[i]);
                    edges.push_back({checkNodes[i]->id, symbolNodes[j]->id});
                }
            }
        }
    }
    
    Node* getNode(int id) {
        for (auto* node : symbolNodes) {
            if (node->id == id) return node;
        }
        for (auto* node : checkNodes) {
            if (node->id == id) return node;
        }
        return nullptr;
    }

    TannerGraph* getClone() {
        std::vector<std::vector<int>> cloneMatrix = matrix;
        return new TannerGraph(cloneMatrix);
    }

    void createEdge(int symbolNodeId, int checkNodeId) {
        Node* symbolNode = getNode(symbolNodeId);
        Node* checkNode = getNode(checkNodeId);

        if (symbolNode && checkNode) {
            symbolNode->connections.push_back(checkNode);
            checkNode->connections.push_back(symbolNode);
            edges.push_back({checkNode->id, symbolNode->id});
            matrix[checkNode->matrixIdx][symbolNode->matrixIdx] = 1;
        }
    }

    Node* getCheckNodeWithLowestDegree() {
        return *std::min_element(checkNodes.begin(), checkNodes.end(),
            [](Node* a, Node* b) { return a->connections.size() < b->connections.size(); });
    }

    Node* getSymbolNodeWithLowestDegree() {
        return *std::min_element(symbolNodes.begin(), symbolNodes.end(),
            [](Node* a, Node* b) { return a->connections.size() < b->connections.size(); });
    }

    // SubGraph class definition
    class SubGraph {
        public:
            Node* rootNode;
            TannerGraph* _tannerGraph;
            Node* treeRoot;
            int level;
    
            SubGraph(TannerGraph* tannerGraph, int rootNodeId, int depth) {
                if (depth < 0) throw std::invalid_argument("depth cannot be negative!");
    
                rootNode = tannerGraph->getNode(rootNodeId);
                _tannerGraph = tannerGraph;
    
                treeRoot = new Node(rootNode->matrixIdx, rootNode->id, rootNode->label, rootNode->group);
                treeRoot->connections = rootNode->connections;
                treeRoot->group = rootNode->group;
    
                int currentLevel = 0;
                std::vector<Node*> usedNodes = {treeRoot};
                std::vector<Node*> queue = {treeRoot};
    
                while (!queue.empty() && currentLevel < depth) {
                    ++currentLevel;
                    std::vector<Node*> levelQueue;
                    for (auto* node : queue) {
                        for (auto* child : node->connections) {
                            if (std::find(usedNodes.begin(), usedNodes.end(), child) == usedNodes.end()) {
                                levelQueue.push_back(child);
                                usedNodes.push_back(child);
                            }
                        }
                    }
                    queue = levelQueue;
                }
    
                level = currentLevel;
            }
            
            std::vector<Node*> coveredCheckNodes() {
                std::vector<Node*> coveredCheckNodes;
                std::vector<Node*> queue = {treeRoot};
                while (!queue.empty()) {
                    Node* node = queue.back();
                    queue.pop_back();
                    if (node->group == "check") {
                        coveredCheckNodes.push_back(node);
                    }
                    for (auto* child : node->connections) {
                        queue.push_back(child);
                    }
                }
                return coveredCheckNodes;
            }
            
            bool allCheckNodesCovered() {
                return coveredCheckNodes().size() == _tannerGraph->checkNodes.size();
            }
    
            Node* getUCCheckNodeWithLowestDegree() {
                std::vector<Node*> uncoveredCheckNodes;
                for (auto* node : _tannerGraph->checkNodes) {
                    if (std::find(coveredCheckNodes().begin(), coveredCheckNodes().end(), node) == coveredCheckNodes().end()) {
                        uncoveredCheckNodes.push_back(node);
                    }
                }
                return *std::min_element(uncoveredCheckNodes.begin(), uncoveredCheckNodes.end(),
                    [](Node* a, Node* b) { return a->connections.size() < b->connections.size(); });
            }
        };

    SubGraph* getSubGraph(int nodeId, int depth) {
        return new SubGraph(this, nodeId, depth);
    }

};


#endif
#ifndef PEG_H
#define PEG_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <queue>
#include <unordered_set>

class Node {
public:
    int matrixIdx;
    int id;
    std::string label;
    std::vector<Node*> connections;
    std::string group;

    Node(int idx, int identifier, const std::string& lbl, const std::string& grp)
        : matrixIdx(idx), id(identifier), label(lbl), group(grp) {}
    
    bool operator==(const Node& other) const {
        return id == other.id && label == other.label && group == other.group;
    }

    //打印node的所有信息
    void print() const {
        std::cout << "Node ID: " << id << "\n"
                    << "Matrix Index: " << matrixIdx << "\n"
                    << "Label: " << label << "\n"
                    << "Group: " << group << "\n"
                    << "Connections: ";

        if (connections.empty()) {
            std::cout << "None";
        } else {
            for (const auto* conn : connections) {
                std::cout << conn->id << " "; // 打印与此节点相连的其他节点的ID
            }
        }
        std::cout << "\n";
    }
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


    void printAdjacencyMatrix(int m, int n) {
        std::vector<std::vector<int>> adjMatrix(m, std::vector<int>(n, 0));
        for (const auto& edge : this->edges) {
            adjMatrix[edge.first-n][edge.second] = 1;  // 由边 (edge.first -> edge.second) 设置为 1
        }
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << adjMatrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    
    // SubGraph class definition
    class SubGraph {
        public:
            Node* rootNode;
            TannerGraph* _tannerGraph;
            struct TreeNode {
                Node* ref;
                int id;
                std::string label;
                std::string group;
                int level;
                std::vector<TreeNode*> children;
        
                TreeNode(Node* node, int lvl)
                    : ref(node), id(node->id), label(node->label), group(node->group), level(lvl) {}
            };

            TreeNode* treeRoot;
            int level;
    
            SubGraph(TannerGraph* tannerGraph, int rootNodeId, int depth) {
               if (depth < 0) throw std::invalid_argument("depth cannot be negative!");
    
                rootNode = tannerGraph->getNode(rootNodeId);

                if (!rootNode) throw std::invalid_argument("Root node not found in graph");
                
                _tannerGraph = tannerGraph;
    
                treeRoot = new TreeNode(rootNode, 0);
    
                std::vector<TreeNode*> usedNodes = {treeRoot};
                std::queue<TreeNode*> queue;
                queue.push(treeRoot);

                level = 0;
    
                while (!queue.empty() && level < depth) {
                    ++level;
                    std::queue<TreeNode*> levelQueue;
                    while (!queue.empty()) {
                        TreeNode* node = queue.front();
                        queue.pop();

                        std::vector<TreeNode*> childrenNodes;

                        for (Node* conn : node->ref->connections) {
                            Node* child = tannerGraph->getNode(conn->id);  // 确保从 TannerGraph 获取节点
                            if (std::find_if(usedNodes.begin(), usedNodes.end(),
                                [child](TreeNode* n) { return n->id == child->id; }) == usedNodes.end()) {
                                
                                TreeNode* newChild = new TreeNode(child, level);
                                childrenNodes.push_back(newChild);
                                levelQueue.push(newChild);
                                usedNodes.push_back(newChild);
                                }
                        }
                        node->children = childrenNodes;
                    }
                    // Move to the next level
                    queue = levelQueue;
                }
    
                this->level = level;
            }
            
            std::vector<Node*> coveredCheckNodes() {
                std::vector<Node*> coveredCheckNodes;
                std::queue<TreeNode*> queue;
                queue.push(treeRoot); 
                
                while (!queue.empty()) {
                    TreeNode* node = queue.front();
                    queue.pop();
                    
                    // 如果是check节点，则加入到coveredCheckNodes
                    if (node->group == "check") {
                        coveredCheckNodes.push_back(node->ref);
                       // node->print(); // 打印该节点的信息
                    }
                    
                    // 遍历子节点
                    for (TreeNode* child : node->children) {
                        queue.push(child);
                    }
                }
                
                return coveredCheckNodes;
            }

            bool allCheckNodesCovered() {
                //printf("cover:%d   all:%d\n",coveredCheckNodes().size(),_tannerGraph->checkNodes.size());
                return coveredCheckNodes().size() == _tannerGraph->checkNodes.size();
            }

            Node* getUCCheckNodeWithLowestDegree() {
                std::vector<Node*> uncoveredCheckNodes;
                for (auto* node : _tannerGraph->checkNodes) {
                    bool coverFlag = false;
                    for(const auto* cvnode : coveredCheckNodes()){
                        if(node == cvnode){
                            coverFlag = true;break;
                        }
                    }
                    if (coverFlag == false) {
                        //node 不在coveredCheckNodes里
                        uncoveredCheckNodes.push_back(node);
                    }
                }   
                if (uncoveredCheckNodes.empty()) {
                    std::cout << "Error: No uncovered check nodes found!\n";   
                    return nullptr;  
                }
            
                Node* lowestDegreeNode = *std::min_element(
                    uncoveredCheckNodes.begin(), uncoveredCheckNodes.end(),
                    [](Node* a, Node* b) { return a->connections.size() < b->connections.size(); });
            
                // Print the information of the node with the lowest degree
            
                return lowestDegreeNode;
            }
        };

    SubGraph* getSubGraph(int nodeId, int depth) {
        return new SubGraph(this, nodeId, depth);
    }
    
};

#endif
#ifndef PEG_H
#define PEG_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <queue>
#include <unordered_set>
#include <fstream>

class Node {
public:
    int matrixIdx;
    int id;
    int deg_diff;  // 度数差
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
                    << "deg_diff: " << deg_diff << "\n"
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

    void print_id() const {
        std::cout << "Node ID: " << id << " ";
    }

    int get_def_diff(){
        return deg_diff;
    }

    void set_deg_diff(int value){
        deg_diff = value;
    }
};


class TannerGraph {
public:
    std::vector<std::vector<int>> matrix;
    std::vector<Node*> variableNodes;
    std::vector<Node*> checkNodes;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> optimalDegCN;

    TannerGraph(const std::vector<std::vector<int>>& mat, const std::vector<int>& optimalDeg)
        : matrix(mat), optimalDegCN(optimalDeg) {
        for (size_t i = 0; i < matrix[0].size(); ++i) {
            variableNodes.push_back(new Node(i, i, "S" + std::to_string(i), "symbol"));
        }

        for (size_t i = 0; i < matrix.size(); ++i) {
            checkNodes.push_back(new Node(i, i + matrix[0].size(), "C" + std::to_string(i), "check"));
        }

        //设置初始的目标度数差
        for(size_t i = 0; i < matrix.size(); i++){
            checkNodes[i]->set_deg_diff(optimalDegCN[i]);
           // checkNodes[i]->print();
        }

        

        for (size_t i = 0; i < matrix.size(); ++i) {
            for (size_t j = 0; j < matrix[i].size(); ++j) {
                if (matrix[i][j] == 1) {
                    // printf("-------------------------------");
                    checkNodes[i]->connections.push_back(variableNodes[j]);
                    int checkNode_diff = checkNodes[i]->get_def_diff();
                   // printf("checknode_diff = %d\n",checkNode_diff);
                    checkNodes[i]->set_deg_diff(checkNode_diff-1);
                    variableNodes[j]->connections.push_back(checkNodes[i]);
                    edges.push_back({checkNodes[i]->id, variableNodes[j]->id});
                }
            }
        }

        // for (size_t i = 0; i < matrix[0].size(); ++i) {
        //     variableNodes[i]->print();
        // }

        // for (size_t i = 0; i < matrix.size(); ++i) {
        //     checkNodes[i]->print();
        // }
    }
    
    Node* getNode(int id) {
        for (auto* node : variableNodes) {
            if (node->id == id) return node;
        }
        for (auto* node : checkNodes) {
            if (node->id == id) return node;
        }
        return nullptr;
    }

    TannerGraph* getClone() {
        std::vector<std::vector<int>> cloneMatrix = matrix;
        std::vector<int> cloneOpt = optimalDegCN;
        return new TannerGraph(cloneMatrix, cloneOpt);
    }

    void createEdge(int variableNodeId, int checkNodeId) {
        Node* variableNode = getNode(variableNodeId);
        Node* checkNode = getNode(checkNodeId);

        if (variableNode && checkNode) {
            variableNode->connections.push_back(checkNode);
            checkNode->connections.push_back(variableNode);
            int checkNode_diff = checkNode->get_def_diff();
            checkNode->set_deg_diff(checkNode_diff-1);
            edges.push_back({checkNode->id, variableNode->id});
            matrix[checkNode->matrixIdx][variableNode->matrixIdx] = 1;
        }
    }

    Node* getCheckNodeWithLowestDegree() {
        return *std::min_element(checkNodes.begin(), checkNodes.end(),
            [](Node* a, Node* b) { return a->connections.size() < b->connections.size(); });
    }

    Node* getvariableNodeWithLowestDegree() {
        return *std::min_element(variableNodes.begin(), variableNodes.end(),
            [](Node* a, Node* b) { return a->connections.size() < b->connections.size(); });
    }

    Node* getCheckNodeWithBiggestGap(){
        // std::cout<<"all check node:";
        // for(int i = 0;i < checkNodes.size();i++) checkNodes[i]->print_id();
        // std::cout<<std::endl;
        return *std::min_element(checkNodes.begin(), checkNodes.end(),
        [](Node* a, Node* b) { return a->deg_diff > b->deg_diff; });
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
    
    void printAdjacencyMatrixToCSV(int m, int n, const std::string& filename) {
        std::vector<std::vector<int>> adjMatrix(m, std::vector<int>(n, 0));
    
        for (const auto& edge : this->edges) {
            adjMatrix[edge.first-n][edge.second] = 1;  // 由边 (edge.first -> edge.second) 设置为 1
        }
    
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return;
        }
    
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                file << adjMatrix[i][j];
                if (j < n - 1) file << ",";  // 逗号分隔
            }
            file << "\n";  // 换行
        }
    
        file.close();
        std::cout << "邻接矩阵已保存到 " << filename << std::endl;
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

            Node* getUCCheckNodeWithBiggestGap() {
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
            //     std::cout<<"all UCcheck node:";
            //     for(int i = 0;i < uncoveredCheckNodes.size();i++) uncoveredCheckNodes[i]->print_id();
            //     std::cout<<std::endl;
                Node* BiggestGapNode = *std::min_element(
                    uncoveredCheckNodes.begin(), uncoveredCheckNodes.end(),
                    [](Node* a, Node* b) { return a->deg_diff > b->deg_diff; });
            
                // Print the information of the node with the lowest degree
            
                return BiggestGapNode;
            }
        };

    SubGraph* getSubGraph(int nodeId, int depth) {
        return new SubGraph(this, nodeId, depth);
    }
    
};

#endif
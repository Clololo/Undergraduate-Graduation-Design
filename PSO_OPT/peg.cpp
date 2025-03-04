#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "peg.h"

class PEG {
    
private:
    int checkNodeNumber;
    int symbolNodeNumber;
    std::vector<int> symbolNodeDegrees;
    TannerGraph* tannerGraph;
    std::function<void(TannerGraph*)> hook;

    void calculateEdges() {
        // Create an initial parity check matrix with all elements set to 0
        std::vector<std::vector<int>> parityCheckMatrix(checkNodeNumber, std::vector<int>(symbolNodeNumber, 0));

        // Use this parity check matrix to create the initial Tanner graph
        tannerGraph = new TannerGraph(parityCheckMatrix);

        for (int index = 0; index < symbolNodeDegrees.size(); ++index) {
            printf("the %d index begin...\n",index+1);
            int degree = symbolNodeDegrees[index];    
            Node* symbolNode = tannerGraph->getNode(index);
            for (int i = 0; i < degree; ++i) {
                if (i == 0) {     
                    // ���ڵ�һ���ߣ�ѡ��ǰͼ�ж�����͵�У��ڵ���Ϊ���Ӷ���
                    Node* lowest = tannerGraph->getCheckNodeWithLowestDegree();

                    //lowest->print();
                    tannerGraph->createEdge(symbolNode->id, lowest->id);
                //  if (hook) hook(tannerGraph);
                } else {
                    // ���ں����ıߣ�����չ�ӵ�ǰ���Žڵ㿪ʼ����ͼ
                    int depth = 0;
                    TannerGraph::SubGraph currentSubGraph(tannerGraph, symbolNode->id, depth);
                    while (true) {
                        if (currentSubGraph.allCheckNodesCovered()) {   
                            // ���м��ڵ��Ѿ�������
                            // select the check node with the lowest degree from previous subgraph
                            TannerGraph::SubGraph* previousSubGraph = tannerGraph->getSubGraph(symbolNode->id, depth - 1);
                            Node* lowest = previousSubGraph->getUCCheckNodeWithLowestDegree();
                            // ��μ��ڵ��Ѿ������ˣ����ϴ���û���ǵ����ϴ���չ��ͼ��δ���ǵ�У��ڵ�
            
                           // lowest->print();
                            tannerGraph->createEdge(symbolNode->id, lowest->id);
                          //  if (hook) hook(tannerGraph);
                            break;
                        }   
                        depth++;
                        TannerGraph::SubGraph nextSubGraph(tannerGraph, symbolNode->id, depth);
                        if (nextSubGraph.level == currentSubGraph.level) {    //��չֹͣ��
                            TannerGraph::SubGraph* previousSubGraph = tannerGraph->getSubGraph(symbolNode->id, depth - 1);
                            Node* lowest = currentSubGraph.getUCCheckNodeWithLowestDegree();
                            // ��ǰ��ͼ��δ���ǵ�У��ڵ�
                          //  lowest->print();
                            tannerGraph->createEdge(symbolNode->id, lowest->id);
                          //  if (hook) hook(tannerGraph);
                            break;
                        }
                        currentSubGraph = nextSubGraph;
                    }
                }
            }    
        }     
    }    

public:
    PEG(int n, int m, const std::vector<int>& degrees)
        : checkNodeNumber(m), symbolNodeNumber(n), symbolNodeDegrees(degrees), tannerGraph(nullptr) {}

    TannerGraph* create() {
        calculateEdges();
        return tannerGraph;
    }

    void hookFunction(const std::function<void(TannerGraph*)>& callback) {
        hook = callback;
    }
};

void printAdjacencyMatrix(const std::vector<std::pair<int, int>>& edges, int m, int n) {

    std::vector<std::vector<int>> matrix(m, std::vector<int>(n, 0));

    for (auto k : edges) {
        matrix[k.first][k.second] = 1;  
    }

    // ��ӡ����
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Example usage
int main() {
    int n = 8; // Number of symbol nodes    
    int m = 4; // Number of check nodes
    std::vector<int> degrees = {2, 2, 2, 2, 2, 2, 2, 2}; // Symbol node degrees
    //todo ��������Ҫ�ĳ�PSO�Ż�֮��Ķȷֲ�

    PEG peg(n, m, degrees);
    TannerGraph* tg = peg.create();
    // Optionally, define a hook function to observe intermediate steps
    peg.hookFunction([](TannerGraph* tg) {
        // Implement hook functionality here
    });
    printf("the peg algorithm finished\n");
    // Use the generated Tanner graph (tg) as needed
    tg->printAdjacencyMatrix(m, n);
    return 0;
}



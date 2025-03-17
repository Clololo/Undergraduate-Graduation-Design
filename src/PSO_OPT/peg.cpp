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
    std::vector<int> cn_best_deg;
    TannerGraph* tannerGraph;
    std::function<void(TannerGraph*)> hook;

    void calculateEdges() {
        // Create an initial parity check matrix with all elements set to 0
        std::vector<std::vector<int>> parityCheckMatrix(checkNodeNumber, std::vector<int>(symbolNodeNumber, 0));

        // Use this parity check matrix to create the initial Tanner graph
        tannerGraph = new TannerGraph(parityCheckMatrix, cn_best_deg);

        for (int index = 0; index < symbolNodeDegrees.size(); ++index) {
            printf("the %d index begin...\n",index+1);
            int degree = symbolNodeDegrees[index];    
            Node* symbolNode = tannerGraph->getNode(index);
            int goal_degree = degree;
            for (int i = 0; i < degree; ++i) {
                if (i == 0) {     
                    // ���ڵ�һ���ߣ�ѡ��ǰͼ�ж�����Ŀ����Զ��У��ڵ���Ϊ���Ӷ���
                    //Node* lowest = tannerGraph->getCheckNodeWithLowestDegree();
                    Node* biggest = tannerGraph->getCheckNodeWithBiggestGap();
                  //  biggest->print();

                    //tannerGraph->createEdge(symbolNode->id, lowest->id);
                    tannerGraph->createEdge(symbolNode->id, biggest->id);
                    goal_degree--;
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
                            //Node* lowest = previousSubGraph->getUCCheckNodeWithLowestDegree();
                            Node* biggest = previousSubGraph->getUCCheckNodeWithBiggestGap();
                            // ��μ��ڵ��Ѿ������ˣ����ϴ���û���ǵ����ϴ���չ��ͼ��δ���ǵ�У��ڵ�
            
                           // biggest->print();
                            if(biggest->deg_diff == 0) break; 
                           //tannerGraph->createEdge(symbolNode->id, lowest->id);
                           tannerGraph->createEdge(symbolNode->id, biggest->id);
                           goal_degree--;
                          //  if (hook) hook(tannerGraph);
                            break;
                        }   
                        depth++;
                        TannerGraph::SubGraph nextSubGraph(tannerGraph, symbolNode->id, depth);
                        if (nextSubGraph.level == currentSubGraph.level) {    //��չֹͣ��
                            TannerGraph::SubGraph* previousSubGraph = tannerGraph->getSubGraph(symbolNode->id, depth - 1);
                            //Node* lowest = currentSubGraph.getUCCheckNodeWithLowestDegree();
                            Node* biggest = currentSubGraph.getUCCheckNodeWithBiggestGap();
                            // ��ǰ��ͼ��δ���ǵ�У��ڵ�
                           // biggest->print();
                            if(biggest->deg_diff == 0) break; 
                           // tannerGraph->createEdge(symbolNode->id, lowest->id);                        
                            tannerGraph->createEdge(symbolNode->id, biggest->id);
                            goal_degree--;
                          //  if (hook) hook(tannerGraph);
                            break;
                        }
                        currentSubGraph = nextSubGraph;
                    }


                }
            }    
            //���Ҳ�޷�������������ģ�ֱ��ѡ��ǰͼ�ж�����Ŀ����Զ��У��ڵ���Ϊ���Ӷ���
            while(goal_degree){
                Node* biggest = tannerGraph->getCheckNodeWithBiggestGap();
                //biggest->print();
                //tannerGraph->createEdge(symbolNode->id, lowest->id);
                tannerGraph->createEdge(symbolNode->id, biggest->id);
                goal_degree--;
            }
        }     
    }    

public:
    PEG(int n, int m, const std::vector<int>& v_degrees, const std::vector<int>& c_degrees)
        : checkNodeNumber(m), symbolNodeNumber(n), symbolNodeDegrees(v_degrees), cn_best_deg(c_degrees), tannerGraph(nullptr) {}

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
    std::vector<int> cn_best_deg = {2, 4, 4, 6};
    //todo ��������Ҫ�ĳ�PSO�Ż�֮��Ķȷֲ�

    PEG peg(n, m, degrees, cn_best_deg);
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



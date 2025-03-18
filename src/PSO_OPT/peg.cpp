#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "peg.h"
#include "pso.h"
#include "../tools/print_tool.h"

extern int pred_vn_length;
extern int pred_cn_length;

class PEG {
    
private:
    int checkNodeNumber;
    int variableNodeNumber;
    std::vector<int> vn_best_deg;
    std::vector<int> cn_best_deg;
    TannerGraph* tannerGraph;
    std::function<void(TannerGraph*)> hook;

    void calculateEdges() {
        // Create an initial parity check matrix with all elements set to 0
        std::vector<std::vector<int>> parityCheckMatrix(checkNodeNumber, std::vector<int>(variableNodeNumber, 0));

        // Use this parity check matrix to create the initial Tanner graph
        tannerGraph = new TannerGraph(parityCheckMatrix, cn_best_deg);

        for (int index = 0; index < vn_best_deg.size(); ++index) {
            //printf("the %d index begin...\n",index+1);
            int degree = vn_best_deg[index];    
            Node* variableNode = tannerGraph->getNode(index);
            int goal_degree = degree;
            for (int i = 0; i < degree; ++i) {
                if (i == 0) {     
                    // ���ڵ�һ���ߣ�ѡ��ǰͼ�ж�����Ŀ����Զ��У��ڵ���Ϊ���Ӷ���
                    //Node* lowest = tannerGraph->getCheckNodeWithLowestDegree();
                    Node* biggest = tannerGraph->getCheckNodeWithBiggestGap();
                  //  biggest->print();

                    //tannerGraph->createEdge(variableNode->id, lowest->id);
                    tannerGraph->createEdge(variableNode->id, biggest->id);
                    goal_degree--;
                //  if (hook) hook(tannerGraph);
                } else {
                    // ���ں����ıߣ�����չ�ӵ�ǰ���Žڵ㿪ʼ����ͼ
                    int depth = 0;
                    TannerGraph::SubGraph currentSubGraph(tannerGraph, variableNode->id, depth);
                    while (true) {
                        if (currentSubGraph.allCheckNodesCovered()) {   
                            // ���м��ڵ��Ѿ�������
                            // select the check node with the lowest degree from previous subgraph
                            TannerGraph::SubGraph* previousSubGraph = tannerGraph->getSubGraph(variableNode->id, depth - 1);
                            //Node* lowest = previousSubGraph->getUCCheckNodeWithLowestDegree();
                            Node* biggest = previousSubGraph->getUCCheckNodeWithBiggestGap();
                            // ��μ��ڵ��Ѿ������ˣ����ϴ���û���ǵ����ϴ���չ��ͼ��δ���ǵ�У��ڵ�
            
                           // biggest->print();
                            if(biggest->deg_diff == 0) break; 
                           //tannerGraph->createEdge(variableNode->id, lowest->id);
                           tannerGraph->createEdge(variableNode->id, biggest->id);
                           goal_degree--;
                          //  if (hook) hook(tannerGraph);
                            break;
                        }   
                        depth++;
                        TannerGraph::SubGraph nextSubGraph(tannerGraph, variableNode->id, depth);
                        if (nextSubGraph.level == currentSubGraph.level) {    //��չֹͣ��
                            TannerGraph::SubGraph* previousSubGraph = tannerGraph->getSubGraph(variableNode->id, depth - 1);
                            //Node* lowest = currentSubGraph.getUCCheckNodeWithLowestDegree();
                            Node* biggest = currentSubGraph.getUCCheckNodeWithBiggestGap();
                            // ��ǰ��ͼ��δ���ǵ�У��ڵ�
                           // biggest->print();
                            if(biggest->deg_diff == 0) break; 
                           // tannerGraph->createEdge(variableNode->id, lowest->id);                        
                            tannerGraph->createEdge(variableNode->id, biggest->id);
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
                //tannerGraph->createEdge(variableNode->id, lowest->id);
                tannerGraph->createEdge(variableNode->id, biggest->id);
                goal_degree--;
            }
        }     
    }    

public:
    PEG(int n, int m, const std::vector<int>& v_degrees, const std::vector<int>& c_degrees)
        : checkNodeNumber(m), variableNodeNumber(n), vn_best_deg(v_degrees), cn_best_deg(c_degrees), tannerGraph(nullptr) {}

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
    //int n = pred_vn_length; // Number of symbol nodes    
    //int m = pred_cn_length; // Number of check nodes
    int n, m;
    read_number_from_csv("temp/pred_vnl.csv", &n);
    read_number_from_csv("temp/pred_cnl.csv", &m);

    int cnd[pred_code_length];
    int vnd[pred_code_length];

    int cn_num = read_csv_column("temp/sim_rho.csv", cnd, pred_code_length);
    int vn_num = read_csv_column("temp/sim_lambda.csv", vnd, pred_code_length);

    std::vector<int> cn_best_deg;
    std::vector<int> vn_best_deg;

    for(int i = 0; i < cn_num; i++){
        cn_best_deg.push_back(cnd[i]);
    }

    for(int i = 0; i < vn_num; i++){
        vn_best_deg.push_back(vnd[i]);
    }
    // std::vector<int> cn_best_deg = read_csv_column("out/sim_rho.csv");
    // std::vector<int> vn_best_deg = read_csv_column("out/sim_lambda.csv"); 

    PEG peg(n, m, vn_best_deg, cn_best_deg);
    TannerGraph* tg = peg.create();
    // Optionally, define a hook function to observe intermediate steps
    peg.hookFunction([](TannerGraph* tg) {
        // Implement hook functionality here
    });
    printf("the peg algorithm finished\n");
    // Use the generated Tanner graph (tg) as needed
    //tg->printAdjacencyMatrix(m, n);
    tg->printAdjacencyMatrixToCSV(m, n, "output/optim_checkmatrix.csv");

    return 0;
}



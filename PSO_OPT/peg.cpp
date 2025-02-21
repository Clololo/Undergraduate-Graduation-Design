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
        printf("calculateEdges is running---\n");
        // Create an initial parity check matrix with all elements set to 0
        std::vector<std::vector<int>> parityCheckMatrix(checkNodeNumber, std::vector<int>(symbolNodeNumber, 0));

        // Use this parity check matrix to create the initial Tanner graph
        tannerGraph = new TannerGraph(parityCheckMatrix);

        for (int index = 0; index < symbolNodeDegrees.size(); ++index) {
            int degree = symbolNodeDegrees[index];
            Node* symbolNode = tannerGraph->getNode(index);
            for (int i = 0; i < degree; ++i) {
                printf("i = %d\n",i);
                if (i == 0) {
                    // Find the check node with the lowest degree
                    Node* lowest = tannerGraph->getCheckNodeWithLowestDegree();
                    tannerGraph->createEdge(symbolNode->id, lowest->id);
                    if (hook) hook(tannerGraph);
                } else {
                    // Gradually deepen the subgraph expanding from this symbol node
                    int depth = 0;
                    TannerGraph::SubGraph currentSubGraph(tannerGraph, symbolNode->id, depth);
                    while (true) {
                        if (currentSubGraph.allCheckNodesCovered()) {
                            // Select the check node with the lowest degree
                            Node* lowest = currentSubGraph.getUCCheckNodeWithLowestDegree();
                            tannerGraph->createEdge(symbolNode->id, lowest->id);
                            if (hook) hook(tannerGraph);
                            break;
                        }
                        depth++;
                        TannerGraph::SubGraph nextSubGraph(tannerGraph, symbolNode->id, depth);
                        if (nextSubGraph.level == currentSubGraph.level) {
                            Node* lowest = currentSubGraph.getUCCheckNodeWithLowestDegree();
                            tannerGraph->createEdge(symbolNode->id, lowest->id);
                            if (hook) hook(tannerGraph);
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

// Example usage
int main() {
    printf("this is a clue\n");
    int n = 8; // Number of symbol nodes
    int m = 4; // Number of check nodes
    std::vector<int> degrees = {2, 2, 2, 2, 2, 2, 2, 2}; // Symbol node degrees

    PEG peg(n, m, degrees);
    TannerGraph* tg = peg.create();

    // Optionally, define a hook function to observe intermediate steps
    peg.hookFunction([](TannerGraph* tg) {
        // Implement hook functionality here
    });
    printf("the peg algorithm finished");
    // Use the generated Tanner graph (tg) as needed

    return 0;
}

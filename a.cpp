#include <CL/sycl.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sycl;

class Graph {
  int numVertices;
  vector<vector<int>> adjLists;

 public:
  Graph(int vertices);
  void addEdge(int src, int dest);
  void BFS(int startVertex);
};

Graph::Graph(int vertices) {
  numVertices = vertices;
  adjLists.resize(vertices);
}

void Graph::addEdge(int src, int dest) {
  adjLists[src].push_back(dest);
  adjLists[dest].push_back(src);
}

void Graph::BFS(int startVertex) {
  queue myQueue{default_selector{}};
  buffer<bool, 1> visited_buf(numVertices);
  buffer<int, 1> queue_buf(numVertices);

  myQueue.submit([&](handler& cgh) {
    auto visited_acc = visited_buf.get_access<access::mode::read_write>(cgh);
    auto queue_acc = queue_buf.get_access<access::mode::read_write>(cgh);

    cgh.single_task([=]() {
      for (int i = 0; i < numVertices; i++) {
        visited_acc[i] = false;
      }

      int queue_front = 0;
      int queue_back = 0;

      visited_acc[startVertex] = true;
      queue_acc[queue_back++] = startVertex;

      while (queue_front != queue_back) {
        int currVertex = queue_acc[queue_front++];
        cout << "Visited " << currVertex << " ";

        for (int adjVertex : adjLists[currVertex]) {
          if (!visited_acc[adjVertex]) {
            visited_acc[adjVertex] = true;
            queue_acc[queue_back++] = adjVertex;
          }
        }
      }
    });
  });
  myQueue.wait_and_throw();
}

int main() {
  Graph g(4);
  g.addEdge(0, 1);
  g.addEdge(0, 2);
  g.addEdge(1, 2);
  g.addEdge(2, 0);
  g.addEdge(2, 3);
  g.addEdge(3, 3);

  g.BFS(2);

  return 0;
}


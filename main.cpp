#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <chrono>
#include <fstream>
#define Wmax 100
#define Wmin 20

using namespace std;
using namespace std::chrono;

// data structure for graph edge
struct edge {
	int src, dest, weight;
};

bool compare(edge a, edge b) {
	return a.weight < b.weight;
}

// disjoint sets code 
class DisjointSet {
	unordered_map<int, int> parent;
	unordered_map<int, int> rank;
public:
	// makeSet operation
	void makeSet(vector<int> const& universe) {
		// create n disjoint sets
		for (int i : universe) {
			parent[i] = i;
			rank[i] = 0;
		}
	}
	// find root of element k
	int Find(int k) {
		// if k root
		if (k == parent[k]) {
			return k;
		}
		return parent[k] = Find(parent[k]);
	}
	//improved code
	void Union(int a, int b) {
		int x = Find(a), y = Find(b);
		// if same parent then return
		if (x == y) return;
		// check which parent has higher rank, add rank and change parent
		if (x != y) {
			if (rank[a] < rank[b]) {
				swap(a, b);
			}
			parent[b] = a;
			if (rank[a] == rank[b]) {
				rank[a]++;
			}
		}
	}

};

// graph class
class Graph {
public:
	// list to hold edges
	vector<edge> edges;
	int n;
	vector<vector<int>> matrix;

	Graph(int N) {
		n = N;
		vector<vector<int>> newmat(N, vector<int>(N, 0));
		matrix = newmat;
		srand(time(0));
		for (int i = 0; i < n; i++) {
			for (int j = i; j < n; j++) {
				if (i == j) {
					matrix[i][j] = 0;
				}
				else if (i < j) {
					matrix[i][j] = (rand() % Wmax) + 1;
					if (matrix[i][j] < Wmin) {
						matrix[i][j] = 0;
					}
					else {
						edges.push_back({ i, j, matrix[i][j] });
						matrix[j][i] = matrix[i][j];
					}
				}
			}
		}
	}
	// graph constructor
	Graph(vector<edge> const& edges_, int N) {
		edges = edges_;
		vector<vector<int>> newmat(N, vector<int>(N, 0));
		matrix = newmat;
		for (auto x : edges) {
			matrix[x.src][x.dest] = x.weight;
			matrix[x.dest][x.src] = x.weight;
		}
		n = N;
	}
	// kruskal's algorithm
	vector<edge> kruskalsMST(int & cost, bool flag) {
		sort(edges.begin(), edges.end(), compare);
		vector<edge> MST;
		DisjointSet DS;
		vector<int> k;
		for (int i = 0; i < n; i++) {
			k.push_back(i);
		}
		DS.makeSet(k);
		for (auto &x : edges) {
			int u = x.src;
			int v = x.dest;
			int parent_u = DS.Find(u);
			int parent_v = DS.Find(v);
			// check for cycle
			if (parent_u != parent_v) {
				MST.push_back(x);
				cost += x.weight;
				DS.Union(parent_u, parent_v);
			}
		}
		if (flag) {
			cout << "Edges of Tree are: " << endl;
			for (auto& x : MST) {
				cout << x.src << " -- " << x.dest << " -- " << x.weight << endl;
			}
			cout << "cost: " << cost << endl;
		}
		
		return MST;
	}
	int valid(int a, int b, vector<bool> & mstvst) {
		if (a == b) {
			return false;
		}
		if (!mstvst[a] && !mstvst[b]) {
			return false;
		}
		else if (mstvst[a] && mstvst[b]) {
			return false;
		}
		return true;


	}
	int minIndex(vector<int> key, vector<bool> inMST) {
		int min = INT_MAX, min_index = 0;

		for (int v = 0; v < n; v++)
			if (inMST[v] == false && key[v] < min)
				min = key[v], min_index = v;

		return min_index;
	}

	vector<edge> primsMST(int& cost, bool flag) {
		vector<edge> T;
		vector<bool> inMST(n, false);
		vector<int> parent(n, -1);
		vector<int> key(n, INT_MAX);
		key[0] = 0;
		parent[0] = -1;
		inMST[0] = true;
		for (int i = 0; i < n - 1; i++) {
			int u = minIndex(key, inMST);
			inMST[u] = true;
			for (int v = 0; v < n; v++) {
				if (matrix[u][v] && !inMST[v] && matrix[u][v] < key[v]) {
					parent[v] = u;
					key[v] = matrix[u][v];
				}
			}
		}
		for (int i = 1; i < n; i++) {
			T.push_back({ parent[i], i, matrix[i][parent[i]] });
		}
		if (flag) {
			cout << "Edges of Tree are: " << endl;
			for (auto& x : T) {
				cout << x.src << " -- " << x.dest << " -- " << x.weight << endl;
				cost += x.weight;
			}
			cout << "cost: " << cost << endl;
		}
		
		return T;
	}
	
	
	void print_graph() {
		cout << "The adjacency matrix of the graph is: " << endl;
		for (int i = 0; i < n; i++) {
			cout << "\t" << i;
		}
		cout << "\n";
		for (int i = 0; i < n; i++) {
			cout << i << "\t";
			for (int j = 0; j < n; j++) {
				cout << matrix[i][j] << "\t";
			}
			cout << "\n";
		}
	}
	double timekruskal() {
		auto start = high_resolution_clock::now();
		int cost = 0;
		kruskalsMST(cost, false);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		return duration.count();
	}

	double timeprims() {
		auto start = high_resolution_clock::now();
		int cost = 0;
		primsMST(cost, false);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		return duration.count();
	}

};

 //read from file
Graph read_file() {
	ifstream f("graph.txt");
	int num;
	f >> num;
	vector<edge> edges;
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			int weight;
			f >> weight;
			if (i < j && weight != 0) {
				edges.push_back({ i, j, weight });
			}
			
		}
	}
	Graph G(edges, num);
	f.close();
	return G;
}

void writefile(Graph G) {
	ofstream f("output.txt");
	f << "Results for generated graph: " << "\n";
	f << "Adjacency Matrix" << "\n";
	for (int i = 0; i < G.n; i++) {
		f << "\t" << i;
	}
	f << "\n";
	for (int i = 0; i < G.n; i++) {
		f << i << "\t";
		for (int j = 0; j < G.n; j++) {
			f << G.matrix[i][j] << "\t";
		}
		f << "\n";
	}
	f << "Number of vertices = " << G.n << "\n";
	f << "Number of non-zero edges = " << G.edges.size() << "\n";
	f << "\n";
	f << "These " << G.n << "  edges are: " << "\n";
	for (auto& x : G.edges) {
		f << x.src << " -- " << x.dest << " -- " << x.weight << "\n";
	}
	f << "\n";
	f << "Kruskal's minimum spanning tree is: " << "\n";
	f << "\n";
	int cost2 = 0;
	vector<edge> kruskals = G.kruskalsMST(cost2, false);
	for (auto& x : kruskals) {
		f << x.src << " -- " << x.dest << " -- " << x.weight << "\n";
	}
	f << "The minimum total cost is " << cost2;

	f << "\n";
	f << "\n";
	f << "Prim's minimum spanning tree is: " << "\n";
	f << "\n";
	int cost1 = 0;
	vector<edge> prims = G.primsMST(cost1, false);
	for (auto& x : prims) {
		f << x.src << " -- " << x.dest << " -- " << x.weight << "\n";
	}
	f << "The minimum total cost is " << cost2 << "\n";
	
	f.close();
}

void run() {
	int num;
	cout << "Welcome to the Minimum Spanning Tree Utility" << "\n";
	cout << "What would you like to do: " << endl;
	cout << "1. generate graph and find its minimum spanning trees" << endl;
	cout << "2. compare the Prim's and Kruskal's algorithms" << endl;
	cout << "3. Read graph from textfile" << endl;
	cout << "4. Exit program" << endl;
	cin >> num;
	while (true) {
		if (num == 1) {
			cout << "Please enter the number of vertices of the graph you would like to be generated:";
			int V;
			cin >> V;
			Graph G(V);
			G.print_graph();
			cout << "Which algorithm would you like to use to find the minimum spanning tree:" << endl;
			cout << "1.Prim's" << endl;
			cout << "2.Kruskal's" << endl;
			cout << "3.Both" << endl;
			int num2;
			cin >> num2;
			if (num2 == 1) {
				int cost = 0;
				G.primsMST(cost, true);
			}
			else if (num2 == 2) {
				int cost = 0;
				G.kruskalsMST(cost, true);
			}
			else if (num2 == 3) {
				int cost1 = 0;
				int cost2 = 0;
				cout << "Prim's: " << endl;
				G.primsMST(cost1, true);
				cout << "Kruskal's: " << endl;
				G.kruskalsMST(cost2, true);
				cout << "\n";
				cout << "Would you like to save the data generated in an output file? Enter 1 if yes:";
				int num4;
				cin >> num4;
				if (num4 == 1) {
					writefile(G);
					cout << "File output.txt generated successfully." << endl;
				}
			}
		}
		else if (num == 2) {
			vector<Graph> v;
			for (int i = 30; i < 100; i++) {
				v.push_back(Graph(i));
			}
			int i = 30;
			cout << "Graph size" << "\t" << "Time taken by Prim in microseconds" << "\t" << "Time taken by Kruskal in milliseconds" << endl;
			for (auto& x : v) {
				cout << i << "\t" << "\t" << "\t" << "\t" << "\t" << x.timeprims() << "\t" << "\t" << "\t" << "\t" << "\t" << x.timekruskal() << endl;
				i++;
			}
			cout << endl;
		}
		else if (num == 3) {
			cout << "Please paste text file containing graph info in directory and name it \"graph.txt\" " << endl;
			cout << "Enter 1 when done: ";
			int num3;
			cin >> num3;
			while (num3 != 1) {
				cout << "Enter 1 when ready: ";
				cin >> num3;
			}
			Graph G = read_file();
			G.print_graph();
			int cost1 = 0;
			cout << "\n";
			cout << "Kruskal's Tree: " << endl;
			G.kruskalsMST(cost1, true);
			cout << "\n";
			cout << "Prim's Tree: " << endl;
			int cost2 = 0;
			G.primsMST(cost2, true);
			cout << "\n";
			cout << "Would you like to save the data generated in an output file? Enter 1 if yes:";
			int num4;
			cin >> num4;
			if (num4 == 1) {
				writefile(G);
				cout << "File output.txt generated successfully." << endl;
			}
		}
		else if (num == 4) {
			cout << "Thank you" << endl;
		}
		else {
			cout << "Please enter a valid number: " << endl;
			cin >> num;
			continue;
		}
		cout << "Would you like to do anything else: " << endl;
		cout << "1. generate graph and find its minimum spanning trees" << endl;
		cout << "2. compare the Prim's and Kruskal's algorithms algorithms" << endl;
		cout << "3. Read graph from textfile" << endl;
		cout << "4. Exit program" << endl;
		cin >> num;
	}
}

// print graph

int main() {
	run();
	return 0;
}

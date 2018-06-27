#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <string>
#include <tuple>

using namespace std;

const int inf = 1000000007;

struct Edge {
    int u;
    int back;
    int flow;
    int cap;
    int cost;
    int num;
    Edge(int u, int back, int cap, int cost, int num) : u(u), back(back), flow(0), cap(cap), cost(cost), num(num) {}
};

struct Graph {
    int sz;
    int k;
    vector<vector<Edge>> gr;
    vector<int> pot;
    vector<int> dist;
    vector<int> par;
    vector<int> last_edge;
    Graph() {}
    Graph(int n, int k) :sz(n), k(k) {
        gr.resize(n);
        pot.resize(n);
        dist.resize(n);
        par.resize(n);
        last_edge.resize(n);
    }
    void addEdge(int u, int v, int cap, int cost, int num) {
        gr[u].push_back(Edge(v, gr[v].size(), cap, cost, num));
        gr[v].push_back(Edge(u, gr[u].size() - 1, 0, -cost, num));
    }
    vector<Edge>& operator [](size_t pos) {
        return gr[pos];
    }
    const vector<Edge>& operator [](size_t pos) const {
        return gr[pos];
    }
    pair<int, int> getFlow(int s, int t) {
        pot.assign(sz, inf);
        pot[s] = 0;
        for (int i = 0; i < sz; ++i) {
            for (int v = 0; v < sz; ++v) {
                if (pot[v] < inf) {
                    for (size_t k = 0; k < gr[v].size(); ++k) {
                        if (gr[v][k].cap > 0 && pot[gr[v][k].u] > pot[v] + gr[v][k].cost) {
                            pot[gr[v][k].u] = pot[v] + gr[v][k].cost;
                        }
                    }
                }
            }
        }
        int flow = 0;
        int cost = 0;
        while (flow < k) {
            dist.assign(sz, inf);
            dist[s] = 0;
            set<pair<int, int>> st;
            st.insert({ 0, s });
            while (!st.empty()) {
                int v = st.begin()->second;
                st.erase(st.begin());
                for (size_t i = 0; i < gr[v].size(); ++i) {
                    int u = gr[v][i].u;
                    if (gr[v][i].cap > gr[v][i].flow && dist[u] > dist[v] + gr[v][i].cost + pot[v] - pot[u]) {
                        st.erase({ dist[u], u });
                        dist[u] = dist[v] + gr[v][i].cost + pot[v] - pot[u];
                        par[u] = gr[v][i].back;
                        st.insert({ dist[u], u });
                    }
                }
            }
            if (dist[t] == inf) {
                break;
            }
            for (int i = 0; i < sz; ++i) {
                if (dist[i] != inf) {
                    pot[i] += dist[i];
                } else {
                    pot[i] += dist[t];
                }
            }
            int plflow = k - flow;
            int plcost = 0;
            int cur_v = t;
            while (cur_v != s) {
                int p = gr[cur_v][par[cur_v]].u;
                int edge = gr[cur_v][par[cur_v]].back;
                plflow = min(plflow, gr[p][edge].cap - gr[p][edge].flow);
                plcost += gr[p][edge].cost;
                cur_v = p;
            }
            cur_v = t;
            while (cur_v != s) {
                int p = gr[cur_v][par[cur_v]].u;
                int edge = gr[cur_v][par[cur_v]].back;
                gr[p][edge].flow += plflow;
                gr[cur_v][par[cur_v]].flow -= plflow;
                cur_v = p;
            }
            flow += plflow;
            cost += plflow * plcost;
        }
        return make_pair(flow, cost);
    }
    vector<vector<int>> decompose(int s, int t) { //only if capacities are equal to one
        vector<vector<int>> ans;
        last_edge.assign(sz, 0);
        while (true) {
            if (last_edge[s] == gr[s].size()) {
                break;
            }
            int cur_v = s;
            vector<int> cur;
            while (cur_v != t) {
                while (last_edge[cur_v] < static_cast<int>(gr[cur_v].size()) && gr[cur_v][last_edge[cur_v]].flow != 1) {
                    last_edge[cur_v]++;
                }
                if (last_edge[cur_v] == gr[cur_v].size()) {
                    break;
                }
                cur.push_back(gr[cur_v][last_edge[cur_v]].num);
                gr[cur_v][last_edge[cur_v]].flow = 0;
                gr[gr[cur_v][last_edge[cur_v]].u][gr[cur_v][last_edge[cur_v]].back].flow = 0;
                cur_v = gr[cur_v][last_edge[cur_v]].u;
            }
            if (cur_v == t) {
                ans.push_back(cur);
            }
        }
        return ans;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    int n, m, k;
    cin >> n >> m >> k;
    Graph gr(n, k);
    for (int i = 0; i < m; ++i) {
        int u, v, tme;
        cin >> u >> v >> tme;
        u--;
        v--;
        gr.addEdge(u, v, 1, tme, i + 1);
        gr.addEdge(v, u, 1, tme, i + 1);
    }
    int flow, cost;
    tie(flow, cost) = gr.getFlow(0, n - 1);
    if (flow < k) {
        cout << -1 << endl;
        return 0;
    }
    cout.precision(10);
    cout << cost * 1.0 / k << endl;
    vector<vector<int>> ans = gr.decompose(0, n - 1);
    for (int i = 0; i < k; ++i) {
        cout << ans[i].size() << ' ';
        for (int q : ans[i]) {
            cout << q << ' ';
        }
        cout << endl;
    }
    system("pause");
    return 0;
}

#ifndef SIMPLE_KTRUSS_INDEX
#define SIMPLE_KTRUSS_INDEX
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <algorithm>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <set>
#include <deque>
#include <ctime>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/tags/tagged.hpp>

using namespace boost::bimaps;
using namespace boost;
using namespace std;


// bimap 仅仅支持只读的结构
// 改成bimap双向map
// block info to unique id
extern bimap<string, int> block_info_id;
bimap<string, int> block_info_id = bimap<string, int> ();

// 用于双向存储
// edge vertex to edge unique id
extern bimap<vector<int>, int> Appear_Edge_id;
bimap<vector<int>, int> Appear_Edge_id =bimap<vector<int>, int> ();


// 用于记录所有的Block的hash_id
extern int block_count;
extern int global_k_max;
int global_k_max = 0;
int block_count = 0;
extern int edge_threshold;
extern double time_threshold;
// 用于记录边的结构

// 全局变量
extern set<int> Appear_Vertexs;
extern map<int, int> Weight;
set<int> Appear_Vertexs = set<int> ();
map<int, int> Weight = map<int, int> ();




set<int> get_map_key_intersection(map<int, int> a, map<int, int> b) {
    set<int> result = set<int> ();
    map<int, int>::iterator i = a.begin();
    map<int, int>::iterator j = b.begin();
    for(; i != a.end() && j != b.end();  ) {
        if (i->first == j->first) {
            result.insert(i->first);
            i++;
            j++;
        }
        else if (i->first < j->first) {
            i++;
        }
        else {
            j++;
        }
    }
    return result;
}

// ascending
bool ascending_cmp(const pair<int, set<int> > &p1, const pair<int, set<int>> &p2) {
	return p1.second.size()< p2.second.size();
}

bool ascending_cmp2(const pair<int, int> &p1, const pair<int, int> &p2) {
    return p1.second< p2.second;
}

bool descending_cmp(const pair<int, int> &p1, const pair<int, int> &p2) {
    return p1.second > p2.second;
}

vector<int> get_edge_help(int u, int v) {
    if(u > v) {
        swap(u, v);
    }
    vector<int> result = {u, v};
    return result;
}

vector<pair<int, int> > sort_map(map<int, int> NBs, bool ascending=false) {
    vector<pair<int, int> > result(NBs.begin(), NBs.end());
    if(ascending) {
        sort(result.begin(), result.end(), ascending_cmp2);
    }
    else {
        sort(result.begin(), result.end(), descending_cmp);
    }
    return result;
}


struct Simple_Ktruss
{
    /* data */
    int unique_id;
    map<int, int> NB2trussness;
    int k_max;

    Simple_Ktruss(int unique_id) {
        this->unique_id = unique_id;
        NB2trussness = map<int, int> ();
        k_max = 0;
    }

    void insert_neighbor(int n) {
        NB2trussness[n] = 0;
    }

    // 插入邻居点的同时获得trussness值
    void get_trussness(int NB, int trussness) {
        NB2trussness[NB] = trussness;
        k_max = max(k_max, trussness);
        global_k_max = max(global_k_max, k_max);
    }

    map<int, int> getNB() {
        return NB2trussness;
    }

    vector<pair<int, int>> get_sort_vector() {
        return sort_map(NB2trussness);
    }

    void display() {
        cout << "----------------------------------------" << endl;
        cout << "Block_id is " << unique_id << endl;
        cout << "Neighbors have : ";
        for(map<int, int>::iterator i = NB2trussness.begin(); i != NB2trussness.end(); i++) {
            cout << i->first << " ";
        }
        cout << endl << "k_max is : " << k_max << endl;
    }
};


struct Real_Graph {
    map<int, Simple_Ktruss *> Real_Vertexs;
    set<int> Used_Edges;
    Real_Graph() {
        Real_Vertexs = map<int, Simple_Ktruss *> ();
        Used_Edges = set<int> ();
    }
    void display() {
        for(map<int, Simple_Ktruss *>::iterator i = Real_Vertexs.begin(); i != Real_Vertexs.end(); i++) {
            i->second->display();
        }
    }
    

    void insert(vector<int> edge) {
        int edge_index = Appear_Edge_id.left.find(edge)->second;
        if (Used_Edges.count(edge_index) == 0) {
            insert_vertex(edge[0], edge[1]);
            insert_vertex(edge[1], edge[0]);
            Used_Edges.insert(edge_index);
        }
    }

    void insert_vertex(int u, int v) {
        if (Real_Vertexs.count(u) == 0) {
            Real_Vertexs[u] = new Simple_Ktruss(u);
        }
        Real_Vertexs[u]->insert_neighbor(v);
    }

    deque<pair<int, set<int> > > get_sup(int & max_size) {
        map<int, set<int>> result = map<int, set<int>> ();
        max_size = 0;
        for(set<int>::iterator i = Used_Edges.begin(); i != Used_Edges.end(); i++) {
            vector<int> Edge_index = Appear_Edge_id.right.find(*i)->second;
            result[*i] = get_map_key_intersection(Real_Vertexs[Edge_index[0]]->getNB(), Real_Vertexs[Edge_index[1]]->getNB());
            max_size = max(max_size, int(result[*i].size()));            
        }
        deque<pair<int, set<int> > > temp_result(result.begin(), result.end());
        // TODO 暂时用快排
        sort(temp_result.begin(), temp_result.end(), ascending_cmp);
        return temp_result;
    }

    // TODO 待改进
    void decrese_one(int edge_index,  deque<pair<int, set<int> > >& sups, int vertex) {
        int i = 0;
        for(; i != sups.size(); i++) {
            if (sups[i].first == edge_index) {
                sups[i].second.erase(vertex);
                break;
            }
        }
        int now_index = i;
        int now_size = sups[i].second.size();
        i--;
        for(; i >= 0; i--) {
            if(sups[i].second.size() == now_size && i != now_index) {
                swap(sups[i+1], sups[now_index]);
            }
        }
    }

    void display(deque<pair<int, set<int> > > sups) {
        for(int i = 0; i < sups.size(); i++) {
            cout << "(" << sups[i].first << ", " << sups[i].second.size() <<")";
        }
        cout << endl;
    }

    void truss_decomposition() {
        clock_t startTime = clock();
        int max_size = 0;
        deque<pair<int, set<int> > > sups = get_sup(max_size);
        set<int> Used_Edges_temp = Used_Edges;
        int k = 2;
        while(Used_Edges_temp.size() > 0) {
            // display(sups);
            // cout << "k is: " <<  k << " sups.size is " << sups.size() << endl;
            while(sups.size() > 0 && sups[0].second.size() <= (k - 2)) {
                pair<int, set<int> > lowest_e = sups[0];
                sups.pop_front();
                int u = Appear_Edge_id.right.find(lowest_e.first)->second[0];
                int v = Appear_Edge_id.right.find(lowest_e.first)->second[1];
                if (Real_Vertexs[u]->getNB().size() > Real_Vertexs[v]->getNB().size()) {
                    swap(u, v);
                }
                for(set<int>::iterator i = lowest_e.second.begin(); i != lowest_e.second.end(); i++) {
                    decrese_one(Appear_Edge_id.left.find(get_edge_help(*i, u))->second, sups, v);
                    decrese_one(Appear_Edge_id.left.find(get_edge_help(*i, v))->second, sups, u);
                }
                Real_Vertexs[u]->get_trussness(v, k);
                Real_Vertexs[v]->get_trussness(u, k);
                Used_Edges_temp.erase(lowest_e.first);
            }
            // 获取trussness值
            k = k + 1;
        }
        cout << "The truss decomposition time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }


    set<int> query_processing(int Vq, int k=0) {
        if( Real_Vertexs.count(Vq) == 0) {
            cout << "this Vertex dose not exist in the real graph" << endl;
            return set<int>();
        }
        clock_t startTime = clock();
        set<int> result = set<int> ();
        if (k == 0) {
            k = Real_Vertexs[Vq]->k_max;
        }
        set<int> visited = set<int> ();
        int l = 0;
        vector<pair<int, int>> NB2trussness = Real_Vertexs[Vq]->get_sort_vector();
        for(vector<pair<int, int>>::iterator i = NB2trussness.begin(); i != NB2trussness.end(); i++) {
            int edge_tmp = Appear_Edge_id.left.find(get_edge_help(i->first, Vq))->second;
            if (i->second >= k && visited.count(edge_tmp) == 0) {
                l = l + 1;
                stack<int> Q;
                Q.push(edge_tmp);
                visited.insert(edge_tmp);
                while(Q.size() > 0) {
                    int x_y = Q.top();
                    Q.pop();
                    int x = Appear_Edge_id.right.find(x_y)->second[0];
                    int y = Appear_Edge_id.right.find(x_y)->second[1];
                    result.insert(x);
                    result.insert(y);
                    set<int> Zs = get_map_key_intersection(Real_Vertexs[x]->getNB(), Real_Vertexs[y]->getNB());
                    for(set<int>::iterator z = Zs.begin(); z != Zs.end(); z++) {
                        if (Real_Vertexs[x]->NB2trussness[*z] >= k && Real_Vertexs[y]->NB2trussness[*z] >= k) {
                            int x_z = Appear_Edge_id.left.find(get_edge_help(x, *z))->second;
                            int y_z = Appear_Edge_id.left.find(get_edge_help(y, *z))->second;
                            if (visited.count(x_z) == 0) {
                                Q.push(x_z);
                                visited.insert(x_z);
                            }
                            if (visited.count(y_z) == 0) {
                                Q.push(y_z);
                                visited.insert(y_z);
                            }
                        }
                    }

                }

            }
            else if(i->second < k) {
                break;
            }
        }
        cout << "The query processing time is : " << (double) (clock() - startTime) / (1.0 * CLOCKS_PER_SEC) << "s" << endl;
        return result;
    }
};


struct Appear_Graph {
    Real_Graph * real_graph;

    Appear_Graph() {
        real_graph = new Real_Graph();
    }

    // 插入一条新出现的边
    void insert(int a, int b) {
        vector<int> index = get_edge_help(a, b);
        if(Appear_Edge_id.left.count(index) == 0) {
            int edge_index = Appear_Edge_id.left.size();
            Appear_Edge_id.left.insert(make_pair(index, edge_index));
            Weight[edge_index] = 1;
            
        }
        else{
            Weight[Appear_Edge_id.left.find(index)->second]++;
        }
        if(Weight[Appear_Edge_id.left.find(index)->second] >= edge_threshold) {
                real_graph->insert(index);
        }
    }

    void display_detail(set<int> results) {
        for(set<int>::iterator i = results.begin(); i != results.end(); i++) {
            cout << block_info_id.right.find(*i)->second << endl;
        }
        cout << "---------------------------" << endl;
    }

};
#endif
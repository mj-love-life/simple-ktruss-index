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
#include "res.hpp"

using namespace boost::bimaps;
using namespace boost;
using namespace std;



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
    set<int> NBs;
    map<int, int> NB2trussness;
    map<int, int> node2edge_id;
    int k_max;

    Simple_Ktruss(int unique_id) {
        this->unique_id = unique_id;
        NB2trussness = map<int, int> ();
        node2edge_id = map<int, int> ();
        NBs = set<int> ();
        k_max = 0;
    }

    void insert_neighbor(int n, int edge_id) {
        NBs.insert(n);
        NB2trussness[n] = 0;
        node2edge_id[n] = edge_id;
    }

    // 插入邻居点的同时获得trussness值
    void get_trussness(int NB, int trussness) {
        NB2trussness[NB] = trussness;
        k_max = max(k_max, trussness);
        global_k_max = max(global_k_max, k_max);
    }

    set<int> get_NBset() {
        return NBs;
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
    

    void insert(vector<int> edge, int edge_index) {
        if (Used_Edges.count(edge_index) == 0) {
            insert_vertex(edge[0], edge[1], edge_index);
            insert_vertex(edge[1], edge[0], edge_index);
            Used_Edges.insert(edge_index);
        }
    }

    void insert_vertex(int u, int v, int edge_index) {
        if (Real_Vertexs.count(u) == 0) {
            Real_Vertexs[u] = new Simple_Ktruss(u);
        }
        Real_Vertexs[u]->insert_neighbor(v, edge_index);
    }


    // 使用桶排进行排序sup
    // 协助进行排序
    void bucket_sort() {

    }

    // 重写一遍get_sup2函数
    map<int, set<int> > get_sup2(map<int, pair<int, int> > & k_value_current_index, bimap<int, int> &edge2index, vector<int> &bucket_sort) {
        cout << "getting support ..." << endl;
        clock_t startTime = clock();
        map<int, set<int> > result = map<int, set<int> > ();
        map<int, int> k_value_size_temp = map<int, int> ();
        int temp_value = 0;

        int count_num = 0;
        double find_time = 0;
        double insert_time = 0;
        int inbalance_time = 0;
        for(set<int>::iterator i = Used_Edges.begin(); i != Used_Edges.end(); i++) {
            count_num++;
            clock_t startTime2 = clock();
            vector<int> Edge_index = Appear_Edge_id.right.find(*i)->second;
            find_time += ((double) (clock() - startTime2) / CLOCKS_PER_SEC);
            startTime2 = clock();
            result[*i] = set<int> ();
            set<int> u_nb = Real_Vertexs[Edge_index[0]]->get_NBset();
            set<int> v_nb = Real_Vertexs[Edge_index[1]]->get_NBset();
            if(int(u_nb.size()) >= 4 * int(v_nb.size()) ||  4 * int(u_nb.size()) <= int(v_nb.size())) inbalance_time++;
            set_intersection(u_nb.begin(), u_nb.end(), v_nb.begin(), v_nb.end(), inserter(result[*i], result[*i].begin()));
            // result.insert(make_pair(*i, get_map_key_intersection(Real_Vertexs[Edge_index[0]]->getNB(), Real_Vertexs[Edge_index[1]]->getNB())));
            insert_time += ((double) (clock() - startTime2) / CLOCKS_PER_SEC);
            temp_value = int(result[*i].size());
            k_value_size_temp.count(temp_value) == 0 ? k_value_size_temp[temp_value] = 1 : k_value_size_temp[temp_value]++;
            // if(count_num % 10000 == 0) {
            //     cout << "The " << count_num << "edge support time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
            //     cout << "find edge time is: "  << find_time << "s" << endl;
            //     cout << "insert edge time is: " << insert_time << "s" << endl;
            //     cout << "inbalance_time: " << inbalance_time << endl;
            //     find_time = 0;
            //     insert_time = 0;
            // }
        }
        int now_index = 0;
        // k -> start_index, len
        k_value_current_index = map<int, pair<int, int> >();
        for(map<int, int>::iterator i = k_value_size_temp.begin(); i != k_value_size_temp.end(); i++) {
            temp_value = now_index;
            k_value_current_index[i->first] = make_pair(temp_value, i->second);
            now_index += i->second;
            i->second = temp_value;
        }
        // k_value_size_temp: k->start_index
        // 桶排存索引
        for(map<int, set<int> >::iterator i = result.begin(); i != result.end(); i++) {
            int i_size = i->second.size();
            edge2index.left.insert(make_pair(i->first, k_value_size_temp[i_size]));
            bucket_sort[k_value_size_temp[i_size]] = i->first;
            k_value_size_temp[i_size]++;
        }
        return result;
    }

    void decrese_one3(int edge_index,  map<int, set<int> >& sups, int vertex, int delete_num, 
        map<int, pair<int, int> > & k_value_index, bimap<int, int> & edge2index, vector<int> &bucket_sort) {
        int now_index = edge2index.left.find(edge_index)->second;
        sups[edge_index].erase(vertex);
        int now_size = sups[edge_index].size();
        // 插在k_value_index[now_size+1].first交换
        int swap_index = k_value_index[now_size + 1].first;
        int edge_index2 = bucket_sort[swap_index];
        if (swap_index != now_index) {
            swap(bucket_sort[now_index], bucket_sort[swap_index]);
            auto pos = edge2index.left.find(edge_index);
            edge2index.left.replace_data(pos, -1);
            auto pos2 = edge2index.left.find(edge_index2);
            edge2index.left.replace_data(pos2, now_index);
            edge2index.left.replace_data(pos, swap_index);
        }
        if (k_value_index.count(now_size) == 0) {   
            k_value_index[now_size] = make_pair(swap_index, 1);
            // k_value_index的更新
        }
        else {
            k_value_index[now_size].second++;
        }
        k_value_index[now_size + 1].first++;
        k_value_index[now_size + 1].second--;
        if(k_value_index[now_size + 1].second == 0) {
            k_value_index.erase(now_size + 1);
        }
    }
    void truss_decomposition2() {
        cout << "truss decomposing ..." << endl;
        clock_t startTime = clock();
        int max_size = 0;
        // 排完序记录每个k值出现的位置以及长度
        // k-> appear_index, length
        map<int, pair<int, int> > k_value_index = map<int, pair<int, int> > ();
        // edge_index -> deque_index
        bimap<int, int> edge2index = bimap<int, int> ();

        // 创建一定大小的vector
        vector<int> bucket_sort(int(Used_Edges.size()));
        map<int, set<int> > sups = get_sup2(k_value_index, edge2index, bucket_sort);
        set<int> Used_Edges_temp = Used_Edges;
        int k = 2;
        int count = 0;
        cout << "The get support value time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
        int delete_num = 0;
        int sups_size = sups.size();
        while(Used_Edges_temp.size() > 0) {
            while(delete_num < sups_size && sups[bucket_sort[delete_num]].size() <= (k - 2)) {
                int delete_sups_size = sups[bucket_sort[delete_num]].size();
                int edge_index = bucket_sort[delete_num];
                count++;
                k_value_index[delete_sups_size].first++;
                k_value_index[delete_sups_size].second--;
                if(k_value_index[delete_sups_size].second == 0) {
                    k_value_index.erase(delete_sups_size);
                }
                int u = Appear_Edge_id.right.find(edge_index)->second[0];
                int v = Appear_Edge_id.right.find(edge_index)->second[1];
                if (Real_Vertexs[u]->getNB().size() > Real_Vertexs[v]->getNB().size()) {
                    swap(u, v);
                }
                for(set<int>::iterator i =  sups[edge_index].begin(); i !=  sups[edge_index].end(); i++) {
                    decrese_one3(Appear_Edge_id.left.find(get_edge_help(*i, u))->second, sups, v, delete_num, k_value_index, edge2index, bucket_sort);
                    decrese_one3(Appear_Edge_id.left.find(get_edge_help(*i, v))->second, sups, u, delete_num, k_value_index, edge2index, bucket_sort);
                }
                Real_Vertexs[u]->get_trussness(v, k);
                Real_Vertexs[v]->get_trussness(u, k);
                Used_Edges_temp.erase(edge_index);
                if(count % 100000 == 0) {
                    cout << "The 100000 edge deal time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
                }
                delete_num++;
            }
            // 获取trussness值
            k = k + 1;
        }
        cout << "The truss decomposition time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }


    deque<pair<int, set<int> > > get_sup(int & max_size, map<int, pair<int, int> > & k_value_index, bimap<int, int> &edge2index, queue<int>& left_k_value) {
        map<int, set<int>> result = map<int, set<int>> ();
        max_size = 0;
        map<int, set<int> > buckets = map<int, set<int> >();
        // bucket用于存储 k_value index
        // 该值用于统计每个k值对应的数量
        map<int, int> k_value_size = map<int, int> ();
        int temp_value = 0;
        int Used_Edge_index = 0;
        for(set<int>::iterator i = Used_Edges.begin(); i != Used_Edges.end(); i++) {
            vector<int> Edge_index = Appear_Edge_id.right.find(*i)->second;
            result[*i] = get_map_key_intersection(Real_Vertexs[Edge_index[0]]->getNB(), Real_Vertexs[Edge_index[1]]->getNB());
            temp_value = int(result[*i].size());
            max_size = max(max_size, temp_value);
            k_value_size.count(temp_value) == 0 ? k_value_size[temp_value] = 1 : k_value_size[temp_value]++;
            if(buckets.count(temp_value) == 0) {
                buckets[temp_value] = set<int> {Used_Edge_index};
            }
            else {
                buckets[temp_value].insert(Used_Edge_index);
            }
            Used_Edge_index++;
        }

        // 上面的过程统计了每个k值对应的大小，后面进行交换排序就好了
        // TODO 以下部分是桶排，是否因为删除问题带来了时间消耗类似？另外是否算法正确
        deque<pair<int, set<int> > > temp_result(result.begin(), result.end());
        Used_Edge_index = 0;
        for(map<int, set<int> >::iterator i = buckets.begin(); i != buckets.end(); i++) {
            while(i->second.size() > 0) {
                int now_index_size = temp_result[Used_Edge_index].second.size();
                if(now_index_size == i->first){
                    i->second.erase(Used_Edge_index);
                }
                else{
                    int swap_index = *(i->second.begin());
                    i->second.erase(swap_index);
                    buckets[now_index_size].erase(Used_Edge_index);
                    swap(temp_result[swap_index], temp_result[Used_Edge_index]);
                    buckets[now_index_size].insert(swap_index);
                }
                Used_Edge_index++;
            }
        }

        // TODO 暂时用快排
        //sort(temp_result.begin(), temp_result.end(), ascending_cmp);
        int value_index = 0;
        vector<int> k_value = vector<int> ();
        for(deque<pair<int, set<int> > >::iterator i = temp_result.begin(); i != temp_result.end(); i++) {
            // 记录edge对应的index下标
            edge2index.left.insert(make_pair(i->first, value_index));
            int size_value = i->second.size();
            if(k_value_index.count(size_value) == 0 ) {
                if (k_value.size() > 0) {
                    int temp_k = k_value[k_value.size() - 1];
                    k_value_index[temp_k].second = value_index - k_value_index[temp_k].first;
                }
                k_value_index[size_value] = make_pair(value_index, 1);
                k_value.push_back(size_value);
                left_k_value.push(size_value);
            }
            value_index++;
        }
        int temp_k = k_value[k_value.size() - 1];
        k_value_index[temp_k].second = temp_result.size() - k_value_index[temp_k].first;
        // for(map<int, pair<int, int> >::iterator i = k_value_index.begin(); i != k_value_index.end(); i++) {
        //     cout << i->first << " " << i->second.first << " " << i->second.second << endl;
        // }
        return temp_result;
    }

    // TODO 待改进
    // 指数前进搜索
    void decrese_one(int edge_index,  deque<pair<int, set<int> > >& sups, int vertex) {
        int now_index = 0;
        for(; now_index != sups.size(); now_index++) {
            if (sups[now_index].first == edge_index) {
                sups[now_index].second.erase(vertex);
                break;
            }
        }
        if (now_index == 0) return ;
        int now_size = sups[now_index].second.size();
        if (sups[0].second.size() > now_size) {
            swap(sups[0], sups[now_index]);
            return ;
        }
        if (sups[now_index - 1].second.size() == now_size) return ;
        int diff_len = 1;
        int target_index = now_index - diff_len;
        while(sups[target_index].second.size() > now_size) {
            target_index -= diff_len;
        }
        if (sups[target_index].second.size() <= now_size) {
            swap(sups[target_index + 1], sups[now_index]);
            return;
        }
        // int now_index = i;
        // int now_size = sups[i].second.size();
        // i--;
        // for(; i >= 0; i--) {
        //     if(sups[i].second.size() == now_size && i != now_index) {
        //         swap(sups[i+1], sups[now_index]);
        //     }
        // }
        //sort(sups.begin(), sups.end(), ascending_cmp);
    }

    void decrese_one2(int edge_index,  deque<pair<int, set<int> > >& sups, int vertex, int delete_num, 
        map<int, pair<int, int> > & k_value_index, bimap<int, int> & edge2index, queue<int> & left_k_value) {
        int now_index = edge2index.left.find(edge_index)->second;
        sups[now_index].second.erase(vertex);
        int now_size = sups[now_index].second.size();
        // 插在k_value_index[now_size+1].first交换
        int swap_index = k_value_index[now_size + 1].first;
        int edge_index2 = sups[swap_index].first;
        if (swap_index != now_index) {
            swap(sups[now_index], sups[swap_index]);
            auto pos = edge2index.left.find(edge_index);
            edge2index.left.replace_data(pos, -1);
            auto pos2 = edge2index.left.find(edge_index2);
            edge2index.left.replace_data(pos2, now_index);
            edge2index.left.replace_data(pos, swap_index);
        }
        if (k_value_index.count(now_size) == 0) {   
            k_value_index[now_size] = make_pair(swap_index, 1);
            // k_value_index的更新
        }
        else {
            k_value_index[now_size].second++;
        }
        k_value_index[now_size + 1].first++;
        k_value_index[now_size + 1].second--;
        if(k_value_index[now_size + 1].second == 0) {
            k_value_index.erase(now_size + 1);
        }
    }

    void display(deque<pair<int, set<int> > > sups) {
        for(int i = 0; i < sups.size(); i++) {
            cout << "(" << sups[i].first << ", " << sups[i].second.size() <<")";
        }
        cout << endl;
    }

    void truss_decomposition() {
        cout << "truss decomposing ..." << endl;
        clock_t startTime = clock();
        int max_size = 0;
        // 排完序记录每个k值出现的位置以及长度
        // k-> appear_index, length
        map<int, pair<int, int> > k_value_index = map<int, pair<int, int> > ();
        // edge_index -> deque_index
        bimap<int, int> edge2index = bimap<int, int> ();
        // left_k_value 也就是对应的k值的索引
        queue<int> left_k_value = queue<int> ();
        deque<pair<int, set<int> > > sups = get_sup(max_size, k_value_index, edge2index, left_k_value);
        set<int> Used_Edges_temp = Used_Edges;
        int k = 2;
        int count = 0;
        cout << "The get support value time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
        int delete_num = 0;
        int sups_size = sups.size();
        while(Used_Edges_temp.size() > 0) {
            // display(sups);
            // cout << "k is: " <<  k << " sups.size is " << sups.size() << " " << delete_num << endl;
            while(delete_num < sups_size && sups[delete_num].second.size() <= (k - 2)) {
                count++;
                pair<int, set<int> > lowest_e = sups[delete_num];
                delete_num++;
                // index, size
                k_value_index[lowest_e.second.size()].first++;
                k_value_index[lowest_e.second.size()].second--;
                if(k_value_index[lowest_e.second.size()].second == 0) {
                    k_value_index.erase(lowest_e.second.size());
                }
                int u = Appear_Edge_id.right.find(lowest_e.first)->second[0];
                int v = Appear_Edge_id.right.find(lowest_e.first)->second[1];
                if (Real_Vertexs[u]->getNB().size() > Real_Vertexs[v]->getNB().size()) {
                    swap(u, v);
                }
                for(set<int>::iterator i = lowest_e.second.begin(); i != lowest_e.second.end(); i++) {
                    // delete_num, k_value_index, edge2index, left_k_value
                    decrese_one2(Appear_Edge_id.left.find(get_edge_help(*i, u))->second, sups, v, delete_num, k_value_index, edge2index, left_k_value);
                    decrese_one2(Appear_Edge_id.left.find(get_edge_help(*i, v))->second, sups, u, delete_num, k_value_index, edge2index, left_k_value);
                }
                Real_Vertexs[u]->get_trussness(v, k);
                Real_Vertexs[v]->get_trussness(u, k);
                Used_Edges_temp.erase(lowest_e.first);
                if(count % 100000 == 0) {
                    cout << "The 100000 edge deal time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
                }
            }
            // while(sups.size() > 0 && sups[0].second.size() <= (k - 2)) {
            //     count++;
            //     delete_num++;
            //     pair<int, set<int> > lowest_e = sups[0];
            //     sups.pop_front();
            //     int u = Appear_Edge_id.right.find(lowest_e.first)->second[0];
            //     int v = Appear_Edge_id.right.find(lowest_e.first)->second[1];
            //     if (Real_Vertexs[u]->getNB().size() > Real_Vertexs[v]->getNB().size()) {
            //         swap(u, v);
            //     }
            //     for(set<int>::iterator i = lowest_e.second.begin(); i != lowest_e.second.end(); i++) {
            //         decrese_one(Appear_Edge_id.left.find(get_edge_help(*i, u))->second, sups, v);
            //         decrese_one(Appear_Edge_id.left.find(get_edge_help(*i, v))->second, sups, u);
            //     }
            //     Real_Vertexs[u]->get_trussness(v, k);
            //     Real_Vertexs[v]->get_trussness(u, k);
            //     Used_Edges_temp.erase(lowest_e.first);
            //     if(count % 1000 == 0) {
            //         cout << "The 1000 edge deal time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
            //     }
            // }
        // for(map<int, pair<int, int> >::iterator i = k_value_index.begin(); i != k_value_index.end(); i++) {
        //     cout << i->first << " " << i->second.first << " " << i->second.second << endl;
        // }
            // 获取trussness值
            k = k + 1;
        }
        cout << "The truss decomposition time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
    }

    //TODO 返回结果使用weight进行排序
    set<int> query_processing(int Vq, int k=0) {
        if( Real_Vertexs.count(Vq) == 0) {
            cout << "this Vertex dose not exist in the real graph" << endl;
            return set<int>();
        }
        // clock_t startTime = clock();
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
        // cout << "The query processing time is : " << (double) (clock() - startTime) / (1.0 * CLOCKS_PER_SEC) << "s" << endl;
        return result;
    }
    ~Real_Graph() {
        for(map<int, Simple_Ktruss *>::iterator i = Real_Vertexs.begin(); i != Real_Vertexs.end(); i++) {
            delete i->second;
        }
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
        // 传递边的id以减少时间
        int edge_index = 0;
        if(Appear_Edge_id.left.count(index) == 0) {
            edge_index = Appear_Edge_id.left.size();
            Appear_Edge_id.left.insert(make_pair(index, edge_index));
            Weight[edge_index] = 1;
        }
        else{
            edge_index = Appear_Edge_id.left.find(index)->second;
            Weight[edge_index]++;
        }
        if(Weight[edge_index] >= weight_threshold) {
            real_graph->insert(index, edge_index);
        }
    }

    void display_detail(set<int> results) {
        for(set<int>::iterator i = results.begin(); i != results.end(); i++) {
            cout << block_info_id.right.find(*i)->second << endl;
        }
        cout << "---------------------------" << endl;
    }

    ~Appear_Graph(){
        delete real_graph;
    }
};
#endif
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <time.h>
#include <cstdlib>
#include <queue>
#include <map>
#include <deque>
#include <set>
#include "utils.hpp"
#include "simple_ktruss_index.hpp"
using namespace std;
using namespace boost;

// 存储出现过的边以及计数、顶点集、队列（长度与时间有关）、队列首部时间
Appear_Graph graph = Appear_Graph();
set<int> vertexs_set = set<int> ();
deque<int> request_queue = deque<int> ();
deque<double> time_queue = deque<double> ();
// 设置边weight的阈值
int edge_threshold = 9;

// 设置时间间隔阈值
double time_threshold = 0.1;

void display(set<int> results) {
    for(set<int>::iterator i = results.begin(); i != results.end(); i++) {
        cout << *i << " ";
    }
    cout << endl;
}

void deal_file(string file_name) {
    string request_stream;
    ifstream in(file_name + ".txt");
    if(in.is_open() == false) {
        cout << "文件没有成功打开" << endl;
        return ;
    }
    cout << "The Edge Weight Threshold you want to set is ? " << endl;
    cin >> edge_threshold;
    cout << "The Time Threshold you want to set is ?" << endl;
    cin >> time_threshold;
    vector<string> info;
    int count = 0;
    while(getline(in, request_stream)) {
        count++;
        if(count % 100 == 0) {
            cout << "Deal with " << count << " line" << endl;
        }
        split(info, request_stream, is_any_of(" "));
        pair<int, double> temp= pre_deal(info);
        // 插入点集中
        int block_id = temp.first;
        double block_time = temp.second;
        vertexs_set.insert(block_id);
        //说明队列中没有其余的元素，也就是最开始的时候，只有一个点，也没有边产生
        if(request_queue.size() == 0){
            request_queue.push_back(block_id);
            time_queue.push_back(block_time);
        }
        else{
            // 遍历去除时间间隔不满足要求的
            while(request_queue.size() > 0 && abs(time_queue.front() - block_time) > time_threshold){
                request_queue.pop_front();
                time_queue.pop_front();
            }
            // 剩余满足要求的，其中一种情况是没有点剩下了
            request_queue.push_back(block_id);
            time_queue.push_back(block_time);
            if(request_queue.size() != 1) {
                for(int i = 0; i < request_queue.size() - 1; i++) {
                    if(request_queue[i] == block_id){
                        continue;
                    }
                    graph.insert(request_queue[i], block_id);
                }
            }
        }
    }
    in.close();
}

int main() {
    // 读取文件
    string file_name;
    cout << "which file do you want to deal with? \n";
    cin >> file_name;
    deal_file(file_name);

    graph.real_graph->truss_decomposition();
    cout << "global_k_max is : " <<  global_k_max << endl;
    //graph.real_graph->display();
    int v_q, k;
    string choice;
    v_q = 1;
    k = 1;
    while(1){
        cout << "which vertex do you want to query: " << endl;
        cin >> v_q;
        cout << "Do you want to choose the k value ? 0 is yes, else is no " <<endl;
        getchar();
        getline(cin, choice);
        if (choice == "0") {
            cout << "The k value you want to query: " << endl;
            cin >> k;
            if(v_q == 0 && k == 0) {
                break;
            }
            cout << "----------------------K_TRUSS_INDEX-----------------" << endl;
            display(graph.real_graph->query_processing(v_q, k));
            // graph.display_detail(graph.real_graph->query_processing(v_q, k));
        }
        else {
            if (v_q == 0) {
                break;
            }
            cout << "----------------------K_TRUSS_INDEX-----------------" << endl;
            display(graph.real_graph->query_processing(v_q));
            // graph.display_detail(graph.real_graph->query_processing(v_q));
        }
    }
    return 0;
}
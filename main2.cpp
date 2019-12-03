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
#include "res.hpp"
using namespace std;
using namespace boost;

// 存储出现过的边以及计数、顶点集、队列（长度与时间有关）、队列首部时间
Appear_Graph graph = Appear_Graph();
set<int> vertexs_set = set<int> ();
deque<int> request_queue = deque<int> ();
deque<double> time_queue = deque<double> ();
// // 设置边weight的阈值
// int weight_threshold = 9;

// // 设置时间间隔阈值
// double time_threshold = 0.1;

ifstream read_file;
ofstream unique_block_info_file, result_file;
double time_threshold, alpha;
int weight_threshold = 0;
bool logistic_or_physics, query_all;
int start_attr, end_attr;

void display(set<int> results) {
    for(set<int>::iterator i = results.begin(); i != results.end(); i++) {
        cout << *i << " ";
    }
    cout << endl;
}

void physics_deal_file() {
    string request_stream;
    vector<string> info;
    int count = 0;
    while(getline(read_file, request_stream)) {
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
}

void logistic_deal_file() {
    string request_stream;
    vector<string> info;
    int count = 0;
    while(getline(read_file, request_stream)) {
        count++;
        if(count % 100 == 0) {
            cout << "Deal with " << count << " line" << endl;
        }
        split(info, request_stream, is_any_of(" "));
        pair<int, double> temp= pre_deal(info);
        // 插入点集中
        int block_id = temp.first;
        vertexs_set.insert(block_id);
        //说明队列中没有其余的元素，也就是最开始的时候，只有一个点，也没有边产生
        if(request_queue.size() == 0){
            request_queue.push_back(block_id);
        }
        else{
            // 遍历去除时间间隔不满足要求的
            if (request_queue.size() > time_threshold) {
                request_queue.pop_front();
            }
            request_queue.push_back(block_id);
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
}


void write_query_file() {
    cout << "query................" << endl;
    clock_t startTime = clock();
    for(map<int, Simple_Ktruss *>::iterator i = graph.real_graph->Real_Vertexs.begin(); i != graph.real_graph->Real_Vertexs.end(); i++) {
        result_file << i->first << endl;
        result_file << i->second->k_max << endl;
        set<int> result = graph.real_graph->query_processing(i->first);
        for(set<int>::iterator j = result.begin(); j != result.end(); j++) {
            result_file << *j << " ";
        }
        result_file << endl << endl;
    }
    cout << "The query and write time is : " << (double) (clock() - startTime) / CLOCKS_PER_SEC << "s" << endl;
}

int get_id() {
    string request_stream;
    getline(cin, request_stream);
    vector<string> info;
    split(info, request_stream, is_any_of(" "));
    pair<int, double> temp= pre_deal(info);
    return temp.first;
}

int main(int argc, char **argv) {
    if (argc != 11) {
        cout << "The argc is not enough" << endl;
        return 0;
    }
    cout << argv[1] << endl;
    read_file.open("./data/" + string(argv[1]));
    unique_block_info_file.open("./info/" + string(argv[2]));
    result_file.open("./result/" + string(argv[3]));
    logistic_or_physics = string(argv[4]) == "Y";
    time_threshold = strtod(argv[5], nullptr);
    weight_threshold = strtol(argv[6], nullptr, 10);
    alpha = strtod(argv[7], nullptr);
    query_all = string (argv[8]) == "Y";
    start_attr = strtol(argv[9], nullptr, 10);
    end_attr = strtol(argv[10], nullptr, 10);
    if(read_file.is_open() == false) {
        cout << "文件没有成功打开" << endl;
        return 0;
    }
    if (logistic_or_physics) {
        logistic_deal_file();
    }
    else {
        physics_deal_file();
    }
    cout << "global_vertex num is : " << graph.real_graph->Real_Vertexs.size() << endl;
    cout << "global edge num is : " << graph.real_graph->Used_Edges.size() << endl;
    graph.real_graph->truss_decomposition();
    cout << "global_k_max is : " <<  global_k_max << endl;
    write_query_file();
    read_file.close();
    read_file.open("./data/" + string(argv[1]));
    read_file.close();
    unique_block_info_file.close();
    result_file.close();
    return 0;
}
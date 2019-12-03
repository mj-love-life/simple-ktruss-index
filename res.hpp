#pragma once
#include <fstream>
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

// 用于记录边的结构
// 全局变量
extern set<int> Appear_Vertexs;
extern map<int, int> Weight;
set<int> Appear_Vertexs = set<int> ();
map<int, int> Weight = map<int, int> ();

// 设置文件命令行的一些参数部分
extern ifstream read_file;
extern ofstream unique_block_info_file, result_file;
extern double time_threshold, alpha;
extern int weight_threshold;
extern bool logistic_or_physics, query_all;
// 开始属性、终止属性
extern int start_attr, end_attr;

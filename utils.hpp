#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <iomanip>
#include <time.h>
#include <set>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include "simple_ktruss_index.hpp"
#include "res.hpp"
using namespace std;
using namespace boost;

// 输出vector内容
template <typename P>
void display_vector(vector<P> arr){
    for(int i = 0; i < arr.size(); i++) {
        cout << arr[i] <<  " ";
    }
    cout << endl;
}

pair<int, double> pre_deal(vector<string> arrs);
//void display_vector(vector<string> arr);
double get_time(string time1);
double time_diff(string time1, string time2);
string add_zero(string str, int len);
string vec2str(vector<string> arr);

// 用于预处理每一条数据并返回vector<string> [unique_id, time]
pair<int, double> pre_deal(vector<string> arrs) {
    string requestid, sequenceid, op, deviceid, segmentindex, extentid, devoffset, segoffset, length, a, b, datafile, submitat, qos, throt, process, result;
    vector<string> unique_vec;
    requestid = arrs[0];
    sequenceid = arrs[1];
    op = arrs[2];
    deviceid = arrs[3];
    segmentindex = arrs[4];
    extentid = arrs[5];
    devoffset = arrs[6];
    segoffset = arrs[7];
    length = arrs[8];
    a = arrs[9];
    b = arrs[10];
    datafile = arrs[11];
    submitat = arrs[12].substr(8);
    qos = arrs[13];
    throt = arrs[14];
    process = arrs[15];
    result = arrs[16];
    //TODO 此处暂时认为是 设备id、段索引、扩展名、dev偏移、seg偏移作为标识
    for(int i = start_attr; i < end_attr; i++) {
        unique_vec.push_back(arrs[i]);
    }
    string info = vec2str(unique_vec);
    if(block_info_id.left.count(info) == 0) {
        unique_block_info_file << block_count << endl;
        unique_block_info_file << info << endl << endl;
        block_info_id.left.insert(make_pair(info, block_count));
        block_count++;
    }
    return make_pair(block_info_id.left.find(info)->second, get_time(submitat));
}


// 用于字符串的分割，当数据规模比较小的时候这种方法会更快，大概是因为需要创建流的原因
vector<string> split1(string str, char flag=' ') {
    istringstream request_ss(str);
    vector<string> results;
    string temp;
    while(getline(request_ss, temp, flag)) {
        results.push_back(temp);
    }
    return results;
}

// 用于字符串的分割，当数据规模比较大的时候这种方法会更快
vector<string> split2(string str, char flag=' ') {
    int start = 0, index = str.find_first_of(flag, 0);
    vector<string> results;
    while(index != str.npos) {
        if(start != index) {
            results.push_back(str.substr(start, index - start));
        }
        start = index + 1;
        index = str.find_first_of(flag, start);
    }
    if(!str.substr(start).empty()) {
        results.push_back(str.substr(start));
    }
    return results;
}

// 获取时间，协助time_diff
double get_time(string time1){
    vector<string> result;
    split(result, time1, is_any_of(":"));
    double temp = 0;
    temp = atof(result[0].c_str());
    split(result, result[1], is_any_of("."));
    temp = temp * 60 + atof(result[0].c_str());
    temp += atof(("0." + result[1]).c_str());
    return temp;
}

// 计算时间差
double time_diff(string time1, string time2) {
    return abs(get_time(time1) - get_time(time2));
}

// 协助vec2str
string add_zero(string str, int len) {
    while(str.length() < len) {
        str = "0" + str;
    }
    return str;
}

// 将vector转化为固定格式的str
string vec2str(vector<string> arr) {
    string temp = "";
    for(int i = 0; i < arr.size(); i++) {
        temp += (arr[i] + " ");
    }
    return temp;
}

#endif
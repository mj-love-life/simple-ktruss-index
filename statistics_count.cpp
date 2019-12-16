#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <string.h>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>
using namespace boost;
using namespace std;
map<string, int> block_count = map<string, int> ();
map<int, int> block_count_distribute = map<int, int> ();
ifstream read_file;
ofstream result_file;
int start_attr;
int end_attr;
string split_point;
int main(int argc, char **argv)
{
    if(argc != 6) {
        cout << "参数不对" << endl;
        return 0;
    }
    int split_num = 0;
    read_file.open("./data/" + string(argv[1]));
    start_attr = strtol(argv[2], nullptr, 10);
    end_attr = strtol(argv[3], nullptr, 10);
    split_num = strtol(argv[4], nullptr, 10);
    result_file.open("./result/" + string(argv[5]));
    split_num == 0 ? split_point = " " : split_point = ",";
    string request_stream;
    vector<string> info;
    // int count = 0;
    while(getline(read_file, request_stream)) {
        split(info, request_stream, is_any_of(split_point));
        string unique_vec;
        for(int i = start_attr; i < end_attr; i++) {
            unique_vec += info[i];
            unique_vec += " ";
        }
        block_count[unique_vec]++;
    }
    for(map<string, int>::iterator i = block_count.begin(); i != block_count.end(); i++) {
        block_count_distribute[i->second]++;
    }
    result_file << "总数" << "," << int(block_count.size()) << endl;
    for(map<int, int>::iterator i = block_count_distribute.begin(); i != block_count_distribute.end(); i++) {
        result_file << i->first << "," << i->second << endl;
    }
    read_file.close();
    result_file.close();
    return 0;
}

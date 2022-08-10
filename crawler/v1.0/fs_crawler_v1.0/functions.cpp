#include "boost/filesystem.hpp"
#include "FLog.h"
#include "json.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <limits>
#include <cstdio>
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

using namespace std;
using boost::uuids::detail::md5;
using namespace boost::filesystem;

std::fstream& GotoLine(std::fstream& file, unsigned int num);
std::string md5toString(const md5::digest_type &digest);
int csv_writer(std::string csv_name, std::string data);
std::string readContent(std::string file_path);
std::string int2str(int integer);

int row = 0, column = -1, level = 0;
vector <vector<int>> dfs_matrix(1000, vector<int>(1000));

//int dfs_matrix[1000][1000];

int crawler(std::string entry_path){

    int obj_id = 0;
    std::ofstream out("crawler.log");
    std::clog.rdbuf(out.rdbuf());
    FLog::printAll = true;
    FLog::logLevel = FLog::Level::all;
    FLoginfo("crawling has been started!");

    //csv_writer("directories.csv","id,name");
    std::fstream directory_writer;
    directory_writer.open("directory.csv", ios::out | ios::app);
    directory_writer << "id,dname";
    directory_writer << "\n";

    //csv_writer("files.csv","id,name");
    std::fstream file_writer;
    file_writer.open("files.csv", ios::out | ios::app);
    file_writer << "id,fname";
    file_writer << "\n";

    for(int i=0;i<1000;i++){

        vector<int> dfs_matrix_row;
        for(int j=0;j<1000;j++){
            dfs_matrix_row.push_back(0);
        }
        dfs_matrix.push_back(dfs_matrix_row);
    }

    path current_dir(entry_path);
    recursive_directory_iterator iter(current_dir), end;

    obj_id = obj_id+1;
    directory_writer << int2str(obj_id)+","+entry_path;
    directory_writer << "\n";
//    dfs_matrix[0][0] = obj_id;
//    level = iter.depth();
//    row = 1;

    while (iter != end){

        try{
                const path& p = iter->path();
                int current_level;
                if(boost::filesystem::is_regular_file(p) & boost::filesystem::is_regular(p)){
                    FLoginfo("File detected :"+ iter->path().filename().string());
                    obj_id++;
                    current_level = iter.depth();

                    boost::filesystem::path full_path(p);
                    file_writer << int2str(obj_id)+","+full_path.string();
                    file_writer << "\n";
                    //boost::filesystem::path full_path(p);
                    //writer << "";
                    //////cout<< readContent(full_path.string())<<endl<<"zart"<<endl;
                    //cout<<"  file_id:  "+int2str(file_id)+"  file_depth:  "+int2str(iter.depth())<<"  file_level:  "+int2str(iter.level())<<" path "<<full_path.string()<<endl;
                    //md5 hash;
                    //md5::digest_type digest;
                    //boost::filesystem::path full_path(p);
                    //hash.process_bytes(full_path.data(), full_path.size());
                    //csv_writer("files.csv",int2str(file_id)+","+iter->path().string()+","+boost::md5(std::ifstream(p)).hex_str_value());
                }
                else if(boost::filesystem::is_directory(p)){
                    //FLoginfo("Directory detected :"+ iter->path().string());
                    obj_id++;
                    current_level = iter.depth();

                    boost::filesystem::path full_path(p);
                    directory_writer << int2str(obj_id)+","+full_path.string();
                    directory_writer << "\n";
                    //cout<<"  directory_id:  "+int2str(dir_id)+"  dir_depth:  "+int2str(iter.depth())<<"  dir_level:  "+int2str(iter.level())<<" path "<<full_path.string()<<endl;
                    //csv_writer("directories.csv.csv",int2str(dir_id)+","+iter->path().filename().string());
                }

                boost::system::error_code ec;
                iter.increment(ec);
                if(ec){
                        FLogerror(ec.message());
                        iter.pop();
                        continue;
                      }
                    //id++;
                if(level == current_level){
                    dfs_matrix[row][column+1] = obj_id;
                    column = column+1;
                    //cout << "level == current_level" << endl << " level :" << int2str(level) << endl << " current_level :" << int2str(current_level) << endl << " obj_id :" << int2str(dfs_matrix[row][column+1]) << endl << " column :" << int2str(column) << endl << " row :" << int2str(row) << endl;
                }
                else if(level < current_level){
                    dfs_matrix[current_level][column] = obj_id;
                    level = current_level;
                    row = current_level;
                    //cout << "level < current_level" << endl << " level :" << int2str(level) << endl << " current_level :" << int2str(current_level) << endl << " obj_id :" << int2str(dfs_matrix[current_level][column]) << endl << " column :" << int2str(column) << endl << " row :" << int2str(row) << endl;
                }
                else if(level > current_level){
                    dfs_matrix[current_level][column+1] = obj_id;
                    level = current_level;
                    column = column+1;
                    row = current_level;
                    //cout << "level > current_level" << endl << " level :" << int2str(level) << endl << " current_level :" << int2str(current_level) << endl << " obj_id :" << int2str(dfs_matrix[current_level][column+1]) << endl << " column :" << int2str(column) << endl << " row :" << int2str(row) << endl;
                }
            }

        catch(...){
                    boost::system::error_code ec;
                    std::cout << "exceptipn catched! " << ec << std::endl;
                    FLogerror(ec.message());
                  }
    }
    std::fstream objects;
    objects.open("objects.csv", ios::out | ios::app);
    objects << "id,parent,child,relation";
    objects << "\n";

//    for(int m=0;m < dfs_matrix.size();m++){
//        for(int n=0; n < dfs_matrix[m].size();n++){
//            objects << dfs_matrix[m][n] << ",";
//        }
//      objects << "\n";
//    }
      int counter_relations = 1;
      for(int m=0;m < dfs_matrix.size();m++){
        vector<int> non_zero_indices(1000);
//        for(int r=0; r<non_zero_indices.size();r++){
//            non_zero_indices.push_back(-1);
//        }

        for(int n=0; n < dfs_matrix[m].size();n++){
            if(dfs_matrix[m][n] != 0){
                non_zero_indices.push_back(n);
            }
        }

        if(m==0){

            objects << int2str(counter_relations) << ",";
            objects << "1" << ",";
            objects << dfs_matrix[m][0] << ",";
            objects << "located";
            objects << "\n";
            counter_relations = counter_relations+1;

            for(int q=0; q < non_zero_indices.size();q++){
                if(non_zero_indices[q] != 0){
                    objects << int2str(counter_relations) << ",";
                    objects << "1" << ",";
                    int value_child = non_zero_indices[q];
                    objects << dfs_matrix[m][value_child] << ",";
                    objects << "located";
                    objects << "\n";
                    counter_relations = counter_relations+1;
                }
            }
        }
        for(int l=0; l < non_zero_indices.size();l++){
            int from = non_zero_indices[l];
            int to = non_zero_indices[l+1];
            for(int p=from; p < to;p++){
                if(m+1<dfs_matrix.size() & dfs_matrix[m+1][p] != 0){
                    objects << counter_relations << ",";
                    objects << dfs_matrix[m][from] << ",";
                    objects << dfs_matrix[m+1][p] << ",";
                    objects << "located";
                    objects << "\n";
                    counter_relations = counter_relations+1;
                }
            }
        }
      }
    objects.close();
    return 1;
}

int csv_writer(std::string csv_name, std::string data){

    //std::string last_modified
    std::fstream writer;
    writer.open(csv_name, ios::out | ios::app);
    writer << data;
    writer.close();
    return 1;
}

std::string int2str(int integer){

    stringstream str_stream;
    str_stream<<integer;
    std::string str;
    str_stream>>str;
    return str;
}

std::string md5toString(const md5::digest_type &digest){

    const auto charDigest = reinterpret_cast<const char *>(&digest);
    std::string result;
    boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(result));
    return result;
}

std::string readContent(std::string file_path){

    while(std::getline(std::cin, file_path)) {
        md5 hash;
        md5::digest_type digest;

        hash.process_bytes(file_path.data(), file_path.size());
        hash.get_digest(digest);

        std::cout << "md5(" << file_path << ") = " << digest << '\n';
    }
    return "zart";
}

std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

#include <iostream>
#include "functions.h"

using namespace std;

int main(int argc, char **argv)
{
//    if(argc>0){
//        for (int i = 0; i < argc; ++i)
//            cout << argv[i] << "\n";
//    }
    std::string entry_path = argv[1];
    //std::string entry_path = "/home/behnam/Desktop/foo";
    crawler(entry_path);

    return 0;
}

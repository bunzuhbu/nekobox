#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

int Sudo(int argc, char ** argv){
    
}

int Run(int argc, char ** argv){
    
}

int main(int argc, char ** argv) {
    if (argc < 3){
        return 1;
    }
    string job = argv[1];
    if (job == "run"){
        if (argc < 5){
            return 1;
        } 
        return Run(argc, argv);
    } else if (job != "sudo"){
        return 1;
    }
    return Sudo(argc, argv);
}

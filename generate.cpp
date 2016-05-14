
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <time.h>

const std::string COMMON_IP_ONE = "192.168.1.";
const std::string COMMON_IP_TWO = "192.168.2.";
const std::string COMMON_IP_THREE = "192.168.3.";
const std::string COMMON_IP_FOUR = "192.168.4.";

const int START_TIME = 1414713600;
const int END_TIME = 1414800000;

int main(int argc, const char * argv[]) {
    if(argc != 2){
        return -1;
    }

    std::string destination(argv[1]);
    if((argv[1][sizeof(argv[1]) - 1]) != '/'){
        destination += "/";
    }
    destination += "2014-10-31";
    std::fstream f(destination, std::fstream::out);
    
    srand(time(nullptr));
    
    std::string toPrint;
    
    int cur = START_TIME;
    while(cur < END_TIME){
        for(int i = 0; i < 250; i++){
            int cpu_0 = rand()%100;
            int cpu_1 = rand()%100;
            toPrint += std::to_string(cur) + " " + COMMON_IP_ONE + std::to_string(i) + " 0 " + std::to_string(cpu_0) + "\n" + std::to_string(cur) + " " + COMMON_IP_ONE + std::to_string(i) + " 1 " + std::to_string(cpu_1) + "\n";
        }
        for(int i = 0; i < 250; i++){
            int cpu_0 = rand()%100;
            int cpu_1 = rand()%100;
            toPrint += std::to_string(cur) + " " + COMMON_IP_TWO + std::to_string(i) + " 0 " + std::to_string(cpu_0) + "\n" + std::to_string(cur) + " " + COMMON_IP_TWO + std::to_string(i) + " 1 " + std::to_string(cpu_1) + "\n";
        }
        for(int i = 0; i < 250; i++){
            int cpu_0 = rand()%100;
            int cpu_1 = rand()%100;
            toPrint += std::to_string(cur) + " " + COMMON_IP_THREE + std::to_string(i) + " 0 " + std::to_string(cpu_0) + "\n" + std::to_string(cur) + " " + COMMON_IP_THREE + std::to_string(i) + " 1 " + std::to_string(cpu_1) + "\n";
        }
        for(int i = 0; i < 250; i++){
            int cpu_0 = rand()%100;
            int cpu_1 = rand()%100;
            toPrint += std::to_string(cur) + " " + COMMON_IP_FOUR + std::to_string(i) + " 0 " + std::to_string(cpu_0) + "\n" + std::to_string(cur) + " " + COMMON_IP_FOUR + std::to_string(i) + " 1 " + std::to_string(cpu_1) + "\n";
        }
        cur += 60;
    }
    f << toPrint;
    
    return 0;
    
}

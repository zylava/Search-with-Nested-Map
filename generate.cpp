/*! 
 @file generate.cpp
 @brief generate a file with CPU usage of two CPUs of 1000 servers every minute of 2014-10-31
 @details Change of date will require changes to START_TIME, END_TIME and DATE strings. Assuming our servers' IP addresses are 192.168.1.0 ~ 192.168.1.249, 192.168.2.0 ~ 192.168.2.249, 192.168.3.0 ~ 192.168.3.249. That's in total 4 * 250 = 1000 servers.
 @author Yilei Zhang
 @date 2016-5-15
 @param destination path and file name fo the file to be generated
 @param f an output stream to destination
 @param to_print the string to put into the destination
*/

/*!
 @mainpage
 \verbatim
    generate.sh generates a file of 2,880,000 entries of foramt:
        > 1414713600 192.168.1.0 1 30
 which corresponds to
        > timestamp  ip        cpu  usage
 \endverbatim
 */

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <time.h>

/// prefix of the first 250 IP addresses
const std::string COMMON_IP_ONE = "192.168.1.";

/// prefix of the second 250 IP addresses
const std::string COMMON_IP_TWO = "192.168.2.";

/// prefix of the third 250 IP addresses
const std::string COMMON_IP_THREE = "192.168.3.";

/// prefix of the last 250 IP addresses
const std::string COMMON_IP_FOUR = "192.168.4.";

/// timestamp of 2014-10-31 00:00:00
const int START_TIME = 1414713600;

/// timestamp of 2014-10-31 23:59:59
const int END_TIME = 1414799999;

/// name of the file to be generated
const std::string DATE = "2014-10-31";

/*!
 takes a uncompleted path, which might end with / or /., and convert it to a full path, including adding 2014-10-31 to the file name
 @param argv is a pointer to an array of char (which is what the user specifies as path)
 @return the modified path
*/
std::string toPath(const char *argv){
    std::string destination(argv);
    if((argv[sizeof(argv) - 1]) != '/'){
        destination += "/";
    }
    destination += DATE;
    return destination;
}

/*!
 Uses global constants to generate a huge string to print.
 @return string to print
*/
std::string printGen(){
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
    return toPrint;
}

int main(int argc, const char * argv[]) {
    if(argc != 2){
        std::cerr << "Wrong number of arguments." << std::endl;
        return -1;
    }
    
    std::string destination = toPath(argv[1]);
    
    std::fstream f(destination, std::fstream::out);
    
    srand(time(nullptr));
    
    std::string to_print = printGen();
    
    f << to_print;
    
    return 0;
    
}

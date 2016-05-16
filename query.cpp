/*!
 @file query.cpp
 @brief Given a start time and an end time, with server IP address and CPU number, gives back the CPU usage of each minute in this time period by doing a search on the file generated.
 @author Yilei Zhang
 @date 2016-5-15
 @param cpu_map a map with cpu as key and ip_map as value
 @param time_lower lower time limit of user input
 @param time_upper upper time limit of user input
 @param cpu cpu number of user input
 @param ip ip address of user input
 */

/*!
 @mainpage
 query.sh handles user's search of CPU usage with specific IP address,   CPU number, start and end time by optimizing the search by constructing threelayers of map.
 */

#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <vector>
#include <time.h>
#include <map>

///maps cpu to ip map
typedef std::map<unsigned long, int> InnerMap;

///maps ip to timestamp map
typedef std::map<std::string, InnerMap> MiddleMap;

///maps timestamp to cpu usage
typedef std::map<int, MiddleMap> OuterMap;

///whether or not suitable files exist in the path specified
bool file_exists = false;


/*!
 This function splits a strings into a set of strings by a string specified
 @param str the string to be separated
 @param delimiter str is separated by delimiter
 @return strings the vector to store separated strings
 */
std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;
    
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    
    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));
    
    return strings;
}

/*!
 This function checks the format of date. A example of a correct date format is 2014-10-31
 @param s the string that contains date
 @return whether or not the date is of the right format
 */
bool check_date_format(std::string s){
    if(s.length() != 10){
        return false;
    }
    if (s[4] != '-' || s[7] != '-'){
        return false;
    }
    for(int i = 0; i < 10; i++){
        if (s[4] != '-' || s[7] != '-'){
            return false;
        }
        else if (s[4] == '-' || s[7] == '-'){
            continue;
        }
        if(!(isdigit(s[i]))){
            return false;
        }
    }
    return true;
}

/*!
 This function checks the format of ip. A example of a correct date ip is 192.168.1.0.
 @param s the string that contains ip
 @return whether or not the ip is of the right format
 */
bool check_IP_format(std::string ip){
    std::vector<std::string> nums;
    nums = split_string(ip, ".");
    if(nums.size() != 4){
        std::cerr << "Wrong IP format. Currently only IPv4 is acceptable." << std::endl;
        return false;
    }
    for(int i = 0; i < 4; i++){
        std::string num = nums[i];
        for(int v = 0; v < num.length(); v++){
            if(!(isdigit(num[v]))){
                return false;
            }
        }
    }
    return true;
}

/*!
 This function checks the format of time. A example of a correct date format is 00:00.
 @param s the string that contains time
 @return whether or not the time is of the right format
 */
bool check_time_format(std::string t){
    if ((t.length()) != 5){
        return false;
    }
    for (int i = 0; i < 5; i++){
        if((i == 2) && (t[i] != ':')){
            return false;
        }
        else if ((i != 2) &&!(isdigit(t[i]))) {
            return false;
        }
    }
    return true;
}

/*!
 This function sets up a map of all cpu usage report files. The outermost map is a cpu_map with cpu number as key, and ip_map as value. The middle map is a map with ip as key, and ts_map with timestamp as key, and cpu usage as value.
 @param ifs input stream class of the current file
 @param cpu_map the map to be set up, notice all files share the same map
 */
void mapSetup(std::string full_file_name, OuterMap &cpu_map){
    std::ifstream ifs(full_file_name.c_str());
    std::string line;
    while (std::getline(ifs, line)){
        file_exists = true;
        if(ifs.eof()){
            break;
        }
        std::vector<std::string> strings;
        strings = split_string(line, " ");
        unsigned long timestamp;
        timestamp = stoul(strings[0]);
        std::string ip = strings[1];
        int cpu;
        cpu = stoi(strings[2]);
        int usage;
        usage = stoi(strings[3]);
        
        OuterMap::iterator out = cpu_map.find(cpu);
        
        if (out != cpu_map.end())
        {
            MiddleMap::iterator mid = out->second.find(ip);
            if (mid != out->second.end())
            {
                mid->second[timestamp] = usage;
            }
            else
            {
                InnerMap ts_map;
                ts_map[timestamp] = usage;
                out->second[ip] = ts_map;
            }
        }
        else
        {
            InnerMap ts_map;
            ts_map[timestamp] = usage;
            MiddleMap ip_map;
            ip_map[ip] = ts_map;
            cpu_map[cpu] = ip_map;
        }
        
        line.clear();
    }
    
}


/*!
 This function parses user's command. It converts the start time and end time to timestamps, so that we can check whether or not the keys of ts_map are within bounds. It also sets up the right cpu_map key and ip_map key.
 @param strings stores the strings separted by split_string function.
 @param time_lower start time in timestamp format
 @param time_upper end time in timestamp format
 @param cpu user input cpu, which is also the key of cpu_map
 @ip ip user input ip, which is the key of ip_map
 */
void commandParse(std::vector<std::string> strings, unsigned long &time_lower, unsigned long &time_upper, int &cpu, std::string ip){
    
    //assistant structure to record user input, and to store output info
    struct tm time_info;
    std::string year = strings[3].substr(0,4);
    std::string mon = strings[3].substr(5,2);
    std::string mday = strings[3].substr(8,2);
    std::string hour = strings[4].substr(0,2);
    std::string min = strings[4].substr(3,2);
    
    time_info.tm_year = stoi(year);
    time_info.tm_year = time_info.tm_year - 1900;
    time_info.tm_mon = stoi(mon);
    time_info.tm_mon = time_info.tm_mon - 1;
    time_info.tm_mday = stoi(mday);
    time_info.tm_hour = stoi(hour);
    time_info.tm_min = stoi(min);
    time_info.tm_sec = 0;
    
    time_lower = static_cast<unsigned long>(timegm(&time_info));
    
    year = strings[5].substr(0,4);
    mon = strings[5].substr(5,2);
    mday = strings[5].substr(8,2);
    hour = strings[6].substr(0,2);
    min = strings[6].substr(3,2);
    
    time_info.tm_year = stoi(year);
    time_info.tm_year = time_info.tm_year - 1900;
    time_info.tm_mon = stoi(mon);
    time_info.tm_mon = time_info.tm_mon - 1;
    time_info.tm_mday = stoi(mday);
    time_info.tm_hour = stoi(hour);
    time_info.tm_min = stoi(min);
    
    time_upper = static_cast<unsigned long>(timegm(&time_info));
    
    cpu = stoi(strings[2]);
    ip = strings[1];
}

/*!
 This function puts the search result in a string.
 @param result stores the search result
 @param found a boolean variable to indicate whether or not a result is found
 @param time_lower start time in timestamp format
 @param time_upper end time in timestamp format
 @param cpu_map nested map constructed to map cpu, ip and timestamp to CPU usage.
 @param cpu user input cpu, which is the key of cpu_map
 @ip ip user input ip, which is the key of ip_map
 */
void putResult(std::string &result, bool &found, const unsigned long time_lower, const unsigned long time_upper, OuterMap cpu_map, const int cpu, const std::string ip){
    struct tm time_info;
    OuterMap::iterator out = cpu_map.find(cpu);
    //traversing the outer map, with cpu being the key, and ip map being the value
    if(out != cpu_map.end()){
        MiddleMap::iterator mid = out->second.find(ip);
        //traversing the middle map, with ip being the key, and timestamp map being the value
        if(mid != out->second.end()){
            for(unsigned long i = time_lower; i <= time_upper; i+=60){
                //traversing the inner map, with timestamp being the key, and usage being the value
                InnerMap::iterator in = mid->second.find(i);
                if(in != mid->second.end()){
                    //converting the format of user's input time to timestamp so that it can be used as a key
                    time_t t = i;
                    int usage = mid->second[i];
                    time_info = *gmtime(&t);
                    if(found == true){
                        result += ", ";
                    }
                    found = true;
                    result = result + "(" + std::to_string(time_info.tm_year + 1900) + "-" + std::to_string(time_info.tm_mon + 1) + "-" + std::to_string(time_info.tm_mday) + " ";
                    std::string hr = std::to_string(time_info.tm_hour);
                    std::string m = std::to_string(time_info.tm_min);
                    
                    //makes sure min and hour always have 2 digits
                    if(hr.length() == 1){
                        hr = "0" + hr;
                    }
                    if(m.length() == 1){
                        m = "0" + m;
                    }
                    result = result + hr + ":" + m + ", ";
                    result = result + std::to_string(usage) + "%)";
                }
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    
    if(argc != 2){
        std::cerr << "Wrong number of argument!" << std::endl;
        return -1;
    }
    
    
    std::string content;
    unsigned long time_lower;
    unsigned long time_upper;
    int cpu;
    std::string ip;
    
    std::string destination(argv[1]);
    
    OuterMap cpu_map;
    
    const char *directory = destination.c_str();
    
    DIR *dir = NULL;
    
    struct dirent *dir_struct;
    
    dir = opendir(directory);
    if (dir == NULL){
        std::cerr << "Cannot open directory!" << std::endl;
        return -1;
    }

    while((dir_struct = readdir(dir))){
        
        std::string file_name(dir_struct->d_name);
        std::string full_file_name = destination + "/" + file_name;
        
        
        if (!(check_date_format(file_name)))
        {
            continue;
        }
        
        mapSetup(full_file_name, cpu_map);
        
        
        std::string user_input;
        
        while(true){
            std::string user_input;
            std::cout << "Program ready for QUERY." << std::endl;
            std::cout << "Examples of correct commands are: " << std::endl;
            std::cout << " > QUERY 192.168.1.10 1 2014-10-31 00:00 2014-10-31 00:05 " << std::endl;
            std::cout << " > EXIT" << std::endl;
            std::getline(std::cin, user_input);
            
            std::string result = "CPU";
            
            std::vector<std::string> strings;
            strings = split_string(user_input, " ");
            
            //basic error checking
            
            if(strings.size() == 1){
                if(strings[0] != "EXIT"){
                    std::cerr << "Command not recognized. Please enter EXIT to exit, and QUERY to query." << std::endl;
                    continue;
                }
                else{
                    std::cout << "Program exiting" << std::endl;
                    break;
                }
            }
            
            else if (strings.size() == 7){
                if(strings[0] != "QUERY"){
                    std::cerr << "Command not recognized. Please enter EXIT to exit, and QUERY to query." << std::endl;
                    continue;
                }
                
                if(!(check_IP_format(strings[1]))){
                    std::cerr << "Bad IP address. Please try again." << std::endl;
                    continue;
                }
                
                if(!(check_date_format(strings[3])) || !(check_date_format(strings[5]))){
                    std::cerr << "Bad date format. Please try entering a date with the format YYYY-MM-DD." << std::endl;
                    continue;
                }
                
                if(!(check_time_format(strings[4])) || !(check_time_format(strings[6]))){
                    std::cerr << "Bad time format. Please try entering a time with the format HH:MM." << std::endl;
                    continue;
                }
                
                
                if((stoi(strings[2])!= 0) && (stoi(strings[2]) != 1)){
                    std::cout << "Please input 0 for the first CPU, and 1 for the second CPU." << std::endl;
                    continue;
                }
                
                commandParse(strings, time_lower, time_upper, cpu, ip);
                
                
                int cpu;
                cpu = stoi(strings[2]);
                std::string ip = strings[1];
                
                OuterMap::iterator out = cpu_map.find(cpu);
                result = result + std::to_string(cpu) + " usage on " + ip + ": \n";
                
                bool found = false;
                
                putResult(result, found, time_lower, time_upper, cpu_map, cpu, ip);
                
                if(found == true){
                    std::cout << result << std::endl;
                }
                else if(found == false){
                    std::cout << "Data in query not found." << std::endl;
                }
            }
            
            else{
                std::cout << "Wrong number of arguments. An Example format should look like the following: " << std::endl;
                std::cout << " > EXIT" << std::endl;
                std::cout << " > QUERY 192.168.1.10 1 2014-10-31 00:00 2014-10-31 00:05 " << std::endl;
                continue;
            }
            result.clear();
            
        }
        
    }
    if (!file_exists){
        std::cerr << "No suitable input file was found." << std::endl;
    }
    return 0;
}

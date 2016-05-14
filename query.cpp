//
//  main.cpp
//  Query
//
//  Created by Yilei Zhang on 5/12/16.
//  Copyright © 2016 yilei. All rights reserved.
//

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

/*Three layers of map, each with a key to optimize search.
 The keys are cpu number, ip address, and the timestamp with timestamp being the inner key, and cpu being the outer key. */
typedef std::map<unsigned long, int> InnerMap;
typedef std::map<std::string, InnerMap> MiddleMap;
typedef std::map<int, MiddleMap> OuterMap;

/* This function split a strings into a set of strings by a string specified */
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

int main(int argc, const char * argv[]) {
    
    if(argc != 2){
        std::cerr << "Wrong number of argument!" << std::endl;
        return -1;
    }
    
    
    std::string content;
    
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
        
        std::ifstream ifs(full_file_name.c_str());
        std::string line;
        
        while (std::getline(ifs, line)){
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
        
        std::string user_input;
        
        while(true){
            std::string user_input;
            std::cout << "Program ready for QUERY." << std::endl;
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
                
                
                //converting from time format of user input to timestamp
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
                
                //start time of request
                time_t time_lower = timegm(&time_info);
                
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
                
                //end time of request
                time_t time_upper = timegm(&time_info);
                
                
                int cpu;
                cpu = stoi(strings[2]);
                std::string ip = strings[1];
                
                OuterMap::iterator out = cpu_map.find(cpu);
                result = result + std::to_string(cpu) + " usage on " + ip + ": \n";
                
                /*only if we have any result will we want to print it. found tells if there' any result available. Notice it also helps decide whether or not to put the comma in between results */
                bool found = false;
                
                /*traversing the outer map, with cpu being the key, and ip map being the value*/
                if(out != cpu_map.end()){
                    MiddleMap::iterator mid = out->second.find(ip);
                    
                    /*traversing the middle map, with ip being the key, and timestamp map being the value*/
                    if(mid != out->second.end()){
                        for(unsigned long i = static_cast<unsigned long>(time_lower); i <= static_cast<unsigned long>(time_upper); i+=60){
                            
                            /*traversing the inner map, with timestamp being the key, and usage being the value*/
                            InnerMap::iterator in = mid->second.find(i);
                            if(in != mid->second.end()){
                                
                                /*converting the format of user's input time to timestamp so that it can be used as a key*/
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
                                
                                /*makes sure min and hour always have 2 digits*/
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
        
        
        return 0;
    }
}

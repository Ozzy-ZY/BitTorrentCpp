#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include<fstream>
#include <sstream>
#include <map>

#include"lib/sha1.hpp"
#include "lib/nlohmann/json.hpp"
using json = nlohmann::json;
typedef long long i64;
json decode_string(const std::string& content_value, i64& index);
json decode_int(const std::string& content_value, i64& index);
json decode_list(const std::string& content_value, i64& index);
json decode_bencoded(const std::string& content_value,i64& index);
json decode_bencoded(const std::string& content_value);
json decode_dict(const std::string& content_value, i64& index);


json decode_string(const std::string& content_value, i64& index){ // 5:hello -> hello
    i64 colon_index = content_value.find(':',index);
    if(colon_index != std::string::npos){
        std::string size_string = content_value.substr(index,colon_index - index);
        i64 size_int = std::stoll(size_string);
        index = colon_index + 1 + size_int; // Update position
        std::string str = content_value.substr(colon_index + 1, size_int);
        return json(str);
    }else {
        std::cout<<"error with string\n";
        throw std::runtime_error("invalid string");
    } 
}
json decode_int(const std::string& content_value, i64& index) {
    index++; // Skip 'i'
    size_t end = content_value.find('e', index);
    if (end == std::string::npos) {
        throw std::invalid_argument("Invalid bencoded integer");
    }
    std::string integer_str = content_value.substr(index, end - index);
    index = end + 1; // Move past 'e'
    return std::stoll(integer_str);
}

json decode_list(const std::string& content_value, i64& index){
    index++; // skip l
    json list = json::array();
    while(content_value[index]!='e'){
        list.push_back(decode_bencoded(content_value,index));
    }
    index++;// ignore e
    return list;
}
json decode_dict(const std::string& content_value, i64& index){
    index++;
    auto map = nlohmann::ordered_map<json, json>();
    while(content_value[index] != 'e'){
        json key = decode_bencoded(content_value,index);
        json val = decode_bencoded(content_value,index);
        map.push_back({key,val});
    }
    index++;
    return json(map);
}
json decode_bencoded(const std::string& content_value,i64& index){
    if(std::isdigit(content_value[index]))
        return decode_string(content_value,index);
    else if(content_value[index] == 'i')
        return decode_int(content_value, index);
    else if(content_value[index] == 'l')
        return decode_list(content_value, index);
    else if(content_value[index] == 'd')
        return decode_dict(content_value,index);
    else{
        std::cout<<"invalid";
        throw std::runtime_error("invalid arguments");
    }
}
json decode_bencoded(const std::string& content_value){
    i64 index = 0;
    return decode_bencoded(content_value,index);
}
json parse_torrent_file(const std::string& filename){
    std::fstream file(filename);
    if(file){
        std::stringstream str_stream;
        str_stream << file.rdbuf();
        file.close();
        return decode_bencoded(str_stream.str());
    }
    file.close();
    printf("error with file");
    throw std::runtime_error("file err");
}
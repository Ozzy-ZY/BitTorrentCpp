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
std::string encode_content(const json& val);
std::string encode_int(const json& val);
std::string encode_string(const json& val);
std::string encode_list(const json& val);
std::string encode_dict(const json& val);

std::string encode_int(const json& val){
    std::stringstream str_strm;
    str_strm << 'i' << val.get<i64>() << 'e';
    return str_strm.str();
}
std::string encode_string(const json& val){
    std::stringstream str_strm;
    std::string str = val.get<std::string>();
    str_strm << str.length() <<':'<<str;
    return str_strm.str();

}
std::string encode_list(const json& val){
    std::stringstream str_strm;
    str_strm <<'l';
    for(const auto item:val){
        str_strm << encode_content(item);
    }
    str_strm << 'e';
    return str_strm.str();
}
std::string encode_dict(const json& val){
    std::stringstream str_strm;
    str_strm <<'d';
    for(const auto& item:val.items()){
        str_strm << encode_string(item.key()) << encode_content(item.value());
    }
    return str_strm.str();
}
std::string encode_content(const json& val){
    if(val.is_string()){
        return encode_string(val);
    }
    else if(val.is_number_integer()){
        return encode_int(val);
    }
    else if(val.is_array()){
        return encode_list(val);
    }
    else if(val.is_object()){
        return encode_dict(val);
    }
    else{
        std::cout<<"invalidsssss";
        throw std::runtime_error("invalid data");
    }
}
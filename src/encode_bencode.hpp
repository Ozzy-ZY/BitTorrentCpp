#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>
#include "lib/sha1.hpp"
#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;
using i64 = long long;

std::string encode_int(const json& val);
std::string encode_string(const json& val);
std::string encode_list(const json& val);
std::string encode_dict(const json& val);
std::string encode_content(const json& val);

std::string encode_int(const json& val) {
    if (!val.is_number_integer()) {
        throw std::invalid_argument("Expected integer value");
    }
    std::stringstream str_strm;
    str_strm << 'i' << val.get<i64>() << 'e';
    return str_strm.str();
}

std::string encode_string(const json& val) {
    if (!val.is_string()) {
        throw std::invalid_argument("Expected string value");
    }
    std::string str = val.get<std::string>();
    std::stringstream str_strm;
    str_strm << str.length() << ':' << str;
    return str_strm.str();
}

std::string encode_list(const json& val) {
    if (!val.is_array()) {
        throw std::invalid_argument("Expected array value");
    }
    std::stringstream str_strm;
    str_strm << 'l';
    for (const auto& item : val) {
        str_strm << encode_content(item);
    }
    str_strm << 'e';
    return str_strm.str();
}

std::string encode_dict(const json& val) {
    if (!val.is_object()) {
        throw std::invalid_argument("Expected object value");
    }
    std::stringstream str_strm;
    str_strm << 'd';
    for (auto it = val.begin(); it != val.end(); ++it) {
        str_strm << encode_string(it.key()) << encode_content(it.value());
    }
    return str_strm.str();
}

std::string encode_content(const json& val) {
    if (val.is_string()) {
        return encode_string(val);
    } else if (val.is_number_integer()) {
        return encode_int(val);
    } else if (val.is_array()) {
        return encode_list(val);
    } else if (val.is_object()) {
        return encode_dict(val);
    } else {
        throw std::invalid_argument("Unsupported data type for encoding");
    }
}

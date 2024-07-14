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

json decode_string(const std::string& content_value, i64& index);
json decode_int(const std::string& content_value, i64& index);
json decode_list(const std::string& content_value, i64& index);
json decode_dict(const std::string& content_value, i64& index);
json decode_bencoded(const std::string& content_value, i64& index);
json decode_bencoded(const std::string& content_value);
json parse_torrent_file(const std::string& filename);

json decode_string(const std::string& content_value, i64& index) {
    size_t colon_index = content_value.find(':', index);
    if (colon_index == std::string::npos) {
        throw std::runtime_error("Invalid string format");
    }

    std::string size_string = content_value.substr(index, colon_index - index);
    i64 size_int = std::stoll(size_string);
    index = colon_index + 1 + size_int; // Update position

    if (index > content_value.size()) {
        throw std::runtime_error("Invalid string format");
    }

    std::string str = content_value.substr(colon_index + 1, size_int);
    return json(str);
}

json decode_int(const std::string& content_value, i64& index) {
    index++; // Skip 'i'
    size_t end = content_value.find('e', index);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid bencoded integer format");
    }

    std::string integer_str = content_value.substr(index, end - index);
    index = end + 1; // Move past 'e'

    try {
        return std::stoll(integer_str);
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid bencoded integer format");
    }
}

json decode_list(const std::string& content_value, i64& index) {
    index++; // Skip 'l'
    json list = json::array();
    while (content_value[index] != 'e') {
        list.push_back(decode_bencoded(content_value, index));
    }
    index++; // Skip 'e'
    return list;
}

json decode_dict(const std::string& content_value, i64& index) {
    index++; // Skip 'd'
    json map = json::object();
    while (content_value[index] != 'e') {
        json key = decode_bencoded(content_value, index);
        json val = decode_bencoded(content_value, index);
        map[key.get<std::string>()] = val;
    }
    index++; // Skip 'e'
    return map;
}

json decode_bencoded(const std::string& content_value, i64& index) {
    if (index >= content_value.size()) {
        throw std::runtime_error("Unexpected end of input");
    }

    if (std::isdigit(content_value[index])) {
        return decode_string(content_value, index);
    } else if (content_value[index] == 'i') {
        return decode_int(content_value, index);
    } else if (content_value[index] == 'l') {
        return decode_list(content_value, index);
    } else if (content_value[index] == 'd') {
        return decode_dict(content_value, index);
    } else {
        throw std::runtime_error("Invalid Bencoded format");
    }
}

json decode_bencoded(const std::string& content_value) {
    i64 index = 0;
    return decode_bencoded(content_value, index);
}

json parse_torrent_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::stringstream str_stream;
    str_stream << file.rdbuf();
    file.close();

    return decode_bencoded(str_stream.str());
}

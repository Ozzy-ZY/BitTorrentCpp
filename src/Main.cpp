#include"lib/sha1.hpp"
#include "lib/nlohmann/json.hpp"
#include"decode_bencode.hpp"

std::string calculate_hash(const std::string& text);

std::string calculate_hash(const std::string& text){
    SHA1 hasher;
    hasher.update(text);
    const std::string finalhash = hasher.final();
    return finalhash;
}

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }
        std::string encoded_value = argv[2];
        json decoded_value = decode_bencoded(encoded_value);
        std::cout << decoded_value.dump() << std::endl;
    }else if(command == "info"){
        std::string filename = argv[2];
        json decoded_data = parse_torrent_file(filename);
        std::string tracker_url;
        decoded_data["announce"].get_to(tracker_url);
        i64 len = decoded_data["info"]["length"];
        std::cout << "Tracker URL: " << tracker_url << std::endl<<
        "Length: "<<len<<std::endl;
    }     
    else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    
    return 0;
}

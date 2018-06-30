#include "Parser.h"

std::string Parser::get_json_string(){
    Json::FastWriter fastWriter;
    return fastWriter.write(root);
}

// gets result as a char* and writes it to a string
std::size_t Parser::write_callback(char *in, size_t size, size_t nmemb, std::string *out){
    std::size_t total_size = size * nmemb;
    if(total_size){
        out->append(in, total_size);
        return total_size;
    }
    return 0;
}

Json::Value Parser::str_to_json(std::string str){
    Json::Reader reader;
    Json::Value rv;
    bool parse_status = reader.parse(str.c_str(), rv);
    if (!parse_status)
        std::cerr  << "parse() failed: " << reader.getFormattedErrorMessages() << std::endl;
    return rv;
}

void Parser::request(){
    // Parse raw Json string
    std::string str_buffer;
    CURL *curl = curl_easy_init();
    if(curl){
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_buffer);
        res = curl_easy_perform(curl);
        while(res!=CURLE_OK){
            std::cerr << "curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
            std::cerr << "Re attempting curl_easy_perform. If the error keeps popping, exit the program." << std::endl;
            res = curl_easy_perform(curl);
        }
        curl_easy_cleanup(curl);
    }else{
        std::cout << "Could not initialize curl" << std::endl;
    }
    // Convert string to Json
    root = str_to_json(str_buffer);
}

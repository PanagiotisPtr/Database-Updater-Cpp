#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>

/*
    The (Json) Parser is used to parse a json file from the web. It uses libcurl to request the json file from the
    server, given the link to it, and then uses jsoncpp to process the Json file.
*/

class Parser{
public:
    Parser(const std::string &s): url(s){}
    Parser(){}
    void request();
    std::string get_json_string();
    Json::Value get_json(){ return root; }
private:
    std::string url;
    Json::Value root;
    // The write_callback function is necessary to process the result of curl_easy_perform. Without this function
    // libcurl just outputs to stdout. See https://curl.haxx.se/libcurl/c/libcurl-tutorial.html
    static std::size_t write_callback(char* in, std::size_t size, std::size_t nmemb, std::string* out);
    Json::Value str_to_json(std::string str);
};

#endif

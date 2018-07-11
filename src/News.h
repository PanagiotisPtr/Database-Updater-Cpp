#ifndef NEWS_H
#define NEWS_H

#include "Data.h"

/*
    Specialized News class. All it does is parse news data from the json that gets to collect the informtion necessary.
    Note: This classes is designed to only be used with the newsAPI
*/

class News : public Data {
public:
    News(const Json::Value &j): Data(j){
        props = {
            {"name", "varchar(500)"},
            {"title", "TEXT"},
            {"urlToImage", "TEXT"},
            {"description", "TEXT"}
        };
    }
private:
    std::string create_data_query(){
        std::string rv = "";
        for(unsigned i = 0; i < data["articles"].size(); i++){
            rv += "(";
            std::string name, title, urlToImage, description;
            name = data["articles"][i]["source"]["name"].asString();
            title = data["articles"][i]["title"].asString();
            urlToImage = data["articles"][i]["urlToImage"].asString();
            description = data["articles"][i]["description"].asString();
            std::replace(description.begin(), description.end(), '\'', '`');
            std::replace(title.begin(), title.end(), '\'', '`');
            std::replace(name.begin(), name.end(), '\'', '`');
            rv += "'" + name + "', ";
            rv += "'" + title + "', ";
            rv += "'" + urlToImage + "', ";
            rv += "'" + description + "'),";
        }
        return rv;
    }
};

#endif

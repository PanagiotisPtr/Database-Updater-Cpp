#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "Parser.h"
#include "Stock.h"
#include "News.h"

using namespace std;

/*
    Notes: the `rv` and `res` variable names stand for `return value` and `result` respectively.
    This naming convention is used over all of the project files
*/

string getLastWeekDate(){
    time_t now = time(nullptr);
    // go back 7 days
    tm *t = gmtime(&now);
    t->tm_mday = t->tm_mday - 7;
    // Normalize date ( ex. avoid something like the day being a negative number )
    time_t lastWeek = mktime(t);
    tm *norm = gmtime(&lastWeek);
    // get as string
    ostringstream oss;
    oss << put_time(norm, "%Y-%m-%d");
    return oss.str();
}

// replaces spaces with %20 to search URLs
string replaceSpaces(string str){
    // This is a bad way to do this*
    // check the end of the file to learn why
    while(str.find(" ") != string::npos)
        str.replace(str.find(" "), 1, "%20");
    return str;
}

int main(){
    ifstream inFile("companies.csv");
    string ticker;
    string name;
    string sector;
    while(getline(inFile, ticker, ',')){
        // get comapny info from csv
        cout << "Now parsing: " << ticker << '\t';
        getline(inFile, name, ',');
        cout << name << '\t';
        getline(inFile, sector, '\n');
        cout << sector << endl;

        cout << "Updating prices..." << endl;
        // Parse stock prices
        Parser parser("https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=" + ticker + "&outputsize=compact&apikey=YOUR_API_KEY");
        parser.request();
        Json::Value j = parser.get_json();
        ticker.erase(std::remove(ticker.begin(), ticker.end(), '.'), ticker.end());
        if(j.isMember("Error Message")){
            cout << "STOCK" << ticker << "SKIPPED!" << endl;
            continue;
        }
        while(j.isMember("Information")){
            cout << "Exceeded API Calls." << endl;
            std::this_thread::sleep_for(std::chrono::seconds(20));
            parser.request();
            j = parser.get_json();
        }

        // Create Stock and push the data on the database
        Stock stock(j);
        stock.set_opt("localhost", "username", "password", "database", 0, NULL, 0);
        stock.post(ticker);

        cout << "Updating news..." << endl;
        // Parse news about company
        Parser newsParser("https://newsapi.org/v2/everything?q=" + replaceSpaces(name) + "&from=" + getLastWeekDate() + "&sortBy=popularity&apiKey=YOUR_API_KEY");
        newsParser.request();
        Json::Value n = newsParser.get_json();
        if(n.get("status", "error") != "ok" || n.get("totalResults", 0) == 0){
            // try searching for company ticker instead of name
            Parser newsParser("https://newsapi.org/v2/everything?q=" + ticker + "&from=" + getLastWeekDate() + "&sortBy=popularity&apiKey=YOUR_API_KEY");
            newsParser.request();
            n = newsParser.get_json();
            if(n.get("status", "error") != "ok" || n.get("totalResults", 0) == 0){
                cout << "No news found about" << name << endl;
                continue;
            }
        }

        // Create News object and push data on database
        News news(n);
        news.set_opt("localhost", "username", "password", "database", 0, NULL, 0);
        news.post(ticker + "_news");
        cout << "Done!" << endl;
        // Do not overwhelm the APIs
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    cout << "Success\n";
}


// *The function that replaces spaces is really badly written. It takes exponentially more time to replace spaces as the string gets longer
// the reason for this is that it starts from the start of the string and searches to the end each time while we KNOW for a fact that there
// are no spaces before the one we just erased so there is no point of going back. Thus this task can also be done in linear time but needs
// attention because you can't replace a character ' ' with a string "%20" easily.

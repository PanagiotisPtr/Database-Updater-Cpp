#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>

#include "Parser.h"
#include "Stock.h"

using namespace std;

/*
    Notes: the `rv` and `res` variable names stand for `return value` and `result` respectively.
    This naming convention is used over all of the project files
*/

int main(){
    ifstream inFile("companies.csv");
    string ticker;
    string name;
    string sector;
    while(getline(inFile, ticker, ',')){
        // get comapny info from css
        cout << "Now parsing: " << ticker << '\t';
        getline(inFile, name, ',');
        cout << name << '\t';
        getline(inFile, sector, '\n');
        cout << sector << endl;

        // Create Parser and get json
        Parser parser("https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=" + ticker + "&outputsize=full&apikey=YOUR_API_KEY");
        parser.request();
        Json::Value j = parser.get_json();
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
		// Note that SERVER_IP can also be "localhost"
		// Reference: string _ip, string _user, string _pass, string _db, unsigned _port, const char *_socket, unsigned long _flag) // Check Stock.h and Options.h
		// For more options visit: https://dev.mysql.com/doc/mysql/en/mysql-real-connect.html 
        stock.set_opt("SERVER_IP", "USERNAME", "PASSWORD", "DATABASE_NAME", 0, NULL, 0);
        stock.post("cpp_example");

        // Do not overwhelm the API
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    cout << "Success\n";
}

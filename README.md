# Database Updater Cpp
A C++ program that updates a database with Stock Data from the alphavantage API

### What is this project about
This is a C++ program that using the Alphavantage API requests a json file with the prices of a certain stock (using libcurl). Then it parses the file (using Jsoncpp) and finally uploads the data on a database (using mysql). it also uses the newsAPI to parse news data about stocks. This project is used to updates the info on a database that I plan to use for other puproses.

### The code
All the code is written in C++. Some classes are general purpose and others are specific. For example the Parser and the Connection classes could be used independently while the Stock and Options classes are made for a specific purpose. Quick note; this project doesn't use the mysql connector but rather the mysql C++ API (mysqlclient).

### How to use
To run the code you will need to have mysql-server(make sure that the service is running), the jsoncpp library and the libcurl C++. Before you compile the code make sure to edit the main.cpp file and replace SERVER_IP, API_KEY etc with the corresponding values (for example replace SERVER_IP with localhost, USER with root, API_KEY with your api key etc.) as I have removed them for obvious security reasons.

##### For Ubuntu
To get those libraries on ubuntu use:

```
sudo apt-get install libmysqlclient-dev
sudo apt-get install libcurl4-openssl-dev
sudo apt-get install libjsoncpp-dev
```

##### MacOS/Windows
If you are not using linux please setup the libraries accordingly

##### Compiling sources
To compile the code `cd /src` and then, using cmake, type `cmake .` and then `make`. Keep in mind that CMake can also be used to convert the project to a Visual Studio Solution and other project types. 

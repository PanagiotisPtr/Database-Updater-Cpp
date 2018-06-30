#include "Stock.h"

const std::string Stock::PBSTR = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
const unsigned Stock::PBWIDTH = 60;

void Stock::set_opt(std::string ip, std::string user, std::string pass, std::string db, unsigned port=0, const char *socket=NULL, unsigned long flag = CLIENT_MULTI_STATEMENTS){
    conn_opt = Options(ip, user, pass, db, port, socket, flag);
}

// There might be a better way to convert a date to unix in C++ but I am not aware of it
// Although the following code works I am pretty sure there is a better way
std::string Stock::get_unix(std::string date){
    // Assumes YYYY/MM/DD
    std::string y_s = date.substr(0, 4);
    std::string m_s = date.substr(5, 2);
    std::string d_s = date.substr(8, 2);

    std::tm tmp;

    tmp.tm_year = std::stoi(y_s) - 1900;
    tmp.tm_mon = std::stoi(m_s) - 1;
    tmp.tm_mday = std::stoi(d_s);
    tmp.tm_hour = 5;
    tmp.tm_min = 1;
    tmp.tm_sec = 0;

	time_t tmp_stamp;
	tmp_stamp = timegm(&tmp);

    auto secs = static_cast<std::chrono::seconds>(tmp_stamp).count();
    long long res = static_cast<long long>(secs);

    return std::to_string(res);
}

// Only works on Linux/MacOS ( and terminal emulators on windows )
void Stock::print_progress(double percentage){
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR.c_str(), rpad, "");
    fflush (stdout);
}

// Creates table on database
bool Stock::create_table(std::string db_name, std::string table_name){
    Connection conn_1(conn_opt);
    // check if the table already exists
    // if there is an error creating a table we need to see it. Other errors such as whether a table exists or not can be skipped
    // remember that the last parameter of run_query(...) tells Connection whether we want to see the error messages or not
    if(!conn_1.run_query("select 1 from " + db_name + "." + table_name + " limit 1;", false))
        conn_1.run_query("create table " + db_name + "." + table_name + "(date int unsigned primary key, open float, high float, low float, close float, volume int unsigned);", true);

    return true;
}

// Dummy function to parse parameters of Json::Value iterator to a StockProps struct
Stock::StockProps Stock::parse_props(Json::Value::const_iterator i){
    Stock::StockProps rv;
    rv.date = i.key().asString();
    rv.unix_date = get_unix(rv.date);
    rv.open = (*i).get("1. open", "missing_argument").asString();
    rv.high = (*i).get("2. high", "missing_argument").asString();
    rv.low = (*i).get("3. low", "missing_argument").asString();
    rv.close = (*i).get("4. close", "missing_argument").asString();
    rv.volume = (*i).get("5. volume", "missing_argument").asString();
    return rv;
}

// Note: MySQL is much better at hundling one large queries rather than a lot of small ones.
// For example: insert db.table (f1, f2) values (1,2);insert db.table (f1, f2) values (1,2);insert db.table (f1, f2) values (1,2);insert db.table (f1, f2) values (1,2);
// is A LOT slower than insert db.table (f1, f2) values (1,2),(1,2),(1,2),(1,2);
void Stock::post(std::string database){
    // Create a table ( if it doesn't exist ) with the stock ticker as a name to which we will push
    // all the data. We also need a tmporary table to which the data will be pushed first.
    // Basically what we will do is merge the 2 tables ( ticker and temporary ) to update the latter.
    std::string ticker = data["Meta Data"].get("2. Symbol", "missing_argument").asString();
    ticker.erase(std::remove(ticker.begin(), ticker.end(), '.'), ticker.end());
    if(!create_table(database, ticker))return;
    if(!create_table(database, "TEMPORARY_CPP_TABLE"))return;

    // Start adding values to the query
    Connection conn_2(conn_opt);
    std::string query = "insert into " + database + "." + "TEMPORARY_CPP_TABLE (date, open, high, low, close, volume) VALUES ";
    double progress = 1;
    for(Json::Value::const_iterator i = data["Time Series (Daily)"].end() ; i-- != data["Time Series (Daily)"].begin();){
        StockProps sp = parse_props(i);
        print_progress((progress++)/(double)data["Time Series (Daily)"].size());
        query += "(" + sp.get_vals_string() + "),";
    }
    query.replace(query.size()-1, 1, ";");
    // Add data to temporary table
    conn_2.run_query(query, true);
    // Update the ticker table with the values of the temporary
    conn_2.run_query("replace into " + database + "." + ticker + " select * from " + database + ".TEMPORARY_CPP_TABLE;", true);
    // Delete the temporary table
    conn_2.run_query("drop table " + database + ".TEMPORARY_CPP_TABLE;", true);
    std::cout << std::endl;
}

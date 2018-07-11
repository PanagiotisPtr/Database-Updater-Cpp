#include "Data.h"

void Data::set_opt(std::string ip, std::string user, std::string pass, std::string db, unsigned port=0, const char *socket=NULL, unsigned long flag = CLIENT_MULTI_STATEMENTS){
    conn_opt = Options(ip, user, pass, db, port, socket, flag);
}

std::string Data::get_props_string(bool with_type){
    std::string rv = "(";
    for(unsigned i = 0; i < props.size(); i++){
        std::string name, type;
        name = props[i].first;
        type = props[i].second;
        rv += name;
        if(with_type)rv += " " + type;
        rv += ",";
    }
    rv.replace(rv.size()-1, 1, ")");
    return rv;
}

// There might be a better way to convert a date to unix in C++ but I am not aware of it
// Although the following code works, I am pretty sure there is a better way
std::string Data::get_unix(std::string date){
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

// Creates table on database
bool Data::create_table(std::string table_name){
    Connection conn_1(conn_opt);
    // check if the table already exists
    // if there is an error creating a table we need to see it. Other errors such as whether a table exists or not can be skipped
    // remember that the last parameter of run_query(...) tells Connection whether we want to see the error messages or not
    if(!conn_1.run_query("select 1 from " + conn_opt.db + "." + table_name + " limit 1;", false))
        conn_1.run_query("create table " + conn_opt.db + "." + table_name + get_props_string(true) + ";", true);
    return true;
}

// Note: MySQL is much better at hundling one large queries rather than a lot of small ones.
// For example: insert db.table (f1, f2) values (1,2);insert db.table (f1, f2) values (1,2);insert db.table (f1, f2) values (1,2);insert db.table (f1, f2) values (1,2);
// is A LOT slower than insert db.table (f1, f2) values (1,2),(1,2),(1,2),(1,2);
void Data::post(std::string table_name){
    // Create a table ( if it doesn't exist ) to which we will push all the data.
    // We also need a tmporary table to which the data will be pushed first.
    // Basically what we will do is merge the 2 tables ( table_name and temporary ) to update the latter.
    if(!create_table(table_name))return;
    if(!create_table("TEMPORARY_CPP_TABLE"))return;

    // Start adding values to the query
    Connection conn_2(conn_opt);
    std::string query = "insert into " + conn_opt.db + "." + "TEMPORARY_CPP_TABLE "+ get_props_string(false) +" values ";
    query += create_data_query();
    if(query[query.size()-1]==',')query.replace(query.size()-1, 1, ";");
    // Add data to temporary table
    conn_2.run_query(query, true);
    // Update the table_name table with the values of the temporary
    conn_2.run_query("replace into " + conn_opt.db + "." + table_name + " select * from " + conn_opt.db + ".TEMPORARY_CPP_TABLE;", true);
    // Delete the temporary table
    conn_2.run_query("drop table " + conn_opt.db + ".TEMPORARY_CPP_TABLE;", true);
}

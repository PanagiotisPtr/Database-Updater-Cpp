#include "Connection.h"

Connection::Connection(Options opt): m_opt(opt){
    mysql_init(&handle);
    mysql_real_connect(&handle, m_opt.ip.c_str(), m_opt.user.c_str(), m_opt.pass.c_str(), m_opt.db.c_str(), m_opt.port, m_opt.socket, m_opt.flag);
    if(*mysql_error(&handle)) // See https://dev.mysql.com/doc/refman/5.7/en/mysql-error.html
        std::cerr << "Couldn't open connection: " << mysql_error(&handle) << std::endl;
}

bool Connection::run_query(std::string q, bool verbose){
    mysql_query(&handle, q.c_str());
    if(*mysql_error(&handle)){
        if(verbose)std::cerr << "Error: " << mysql_error(&handle) << std::endl;
        return false;
    }
    // Note that you must use or store the result of your mysql query. Otherwhise there may be errors when calling multiple queries
    MYSQL_RES *res = mysql_store_result(&handle);
    // You must also free said result
    mysql_free_result(res);
    return true;
}

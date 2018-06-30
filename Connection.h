#ifndef CONNECTION_H
#define CONNECTION_H

#include <mysql/mysql.h>
#include <iostream>

#include "Options.h"

/*
    The connection class handles MYSQL connections. It can handle query_request through the run_query function and
    using it's destructor it makes sure that all connections are closed when the connection object is destroyed.
*/

class Connection{
public:
    Connection(Options opt);
    ~Connection(){ if(&handle!=NULL)mysql_close(&handle); }
    MYSQL* get_handle(){ return &handle; }
    bool run_query(std::string q, bool verbose);
private:
    Options m_opt;
    MYSQL handle;
};

#endif

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

/*
    The Options struct is mostly use to avoid typing a lot of reduntant code. It really doesn't do much.
    It is just a struct that holds a few values.
*/

struct Options{
    Options(){}
    Options(std::string _ip, std::string _user, std::string _pass, std::string _db, unsigned _port, const char *_socket, unsigned long _flag)
    : ip(_ip), user(_user), pass(_pass), db(_db), port(_port), socket(_socket), flag(_flag) {}
    std::string ip;
    std::string user;
    std::string pass;
    std::string db;
    unsigned port;
    const char* socket;
    unsigned long flag;
};

#endif

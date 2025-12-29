#ifndef DEF__H
#define DEF__H
#define DB_OLD_VERSION 1 
#define DB_NEW_VERSION 1//数据库表结构有改动时，此版本号需做+1操作 

#include <strings.h>
#include <stdint.h>
#include <stddef.h>

#define CLRFLAG \
    char __CLRFLAG;

#define INITZERO(name) \
    static long offset = (long)offsetof(class name, __CLRFLAG);   \
    bzero(this, offset);

#define CLR_POD_VALUE(name ) \
    CLRFLAG     \
    name() {    \
        INITZERO(name)  \
    }

#endif

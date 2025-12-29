#pragma once

#include <odb/core.hxx>
#include "def.h"

#pragma db model version (DB_OLD_VERSION, DB_NEW_VERSION)

#pragma db object table("demo")
class Demo
{
public:
	friend class odb::access;
#pragma db id
    int32_t id;

#pragma db transient 
    CLR_POD_VALUE(Demo)
};

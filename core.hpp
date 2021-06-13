#ifndef MAL_CORE_HPP_INCLUED
#define MAL_CORE_HPP_INCLUED

#include "types.hpp"
#include "env.hpp"



namespace op {

MalType add(MalType);
MalType sub(MalType);
MalType div(MalType);
MalType mul(MalType);

MalType list(MalType);
MalType listp(MalType);
MalType emptyp(MalType);
MalType count(MalType);

MalType eq(MalType);
MalType ne(MalType);
MalType lt(MalType);
MalType le(MalType);
MalType gt(MalType);
MalType ge(MalType);

MalType prn(MalType);
MalType pr_str(MalType);
MalType str(MalType);
MalType println(MalType);

};

extern EnvPtr & ns;
EnvPtr & get_core_env(); 

#endif//MAL_CORE_HPP_INCLUED
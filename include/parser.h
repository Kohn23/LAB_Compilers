#ifndef PARSER_H
#define PARSER_H

#include "windll.h"
#include "tok.h"
#include "sym.h"
#include "error.h"

CORE_API void recursive_descent_parse(TokenStream* , VarTable* , ProcTable* , ErrorLogger* );

#endif // PARSER_H

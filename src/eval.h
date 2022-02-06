#ifndef __EVAL_H
#define __EVAL_H


#include "env.h"
#include "cell.h"

CellPtr read_value(EnvironmentPtr env, const char* buffer, const char** next, int quoted);

#endif
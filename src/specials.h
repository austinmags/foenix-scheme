#ifndef __SPECIALS_H
#define __SPECIALS_H

#include "env.h"
#include "cell.h"

CellPtr scheme_env(EnvironmentPtr env, CellPtr operands);
CellPtr scheme_print(EnvironmentPtr env, CellPtr operands);
#endif

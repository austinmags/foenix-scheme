#ifndef __ENV_H
#define __ENV_H

#include "map.h"
#include "cell.h"

typedef struct ENV {
    struct ENV* parent;
    MapPtr symbols;
} Environment, *EnvironmentPtr;

EnvironmentPtr MakeEnvironment(EnvironmentPtr parent);
void FreeEnvironment(EnvironmentPtr env);
CellPtr EnvGet(EnvironmentPtr env, const char* name);
int EnvInstallSymbol(EnvironmentPtr env, const char* name, CellPtr cell);
int EnvInstallUnboundSymbols(EnvironmentPtr env, CellPtr symbols);
#endif
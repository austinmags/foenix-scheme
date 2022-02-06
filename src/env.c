#include "env.h"

#include <stdio.h>
#include <stdlib.h>

#include "cell.h"

EnvironmentPtr MakeEnvironment(EnvironmentPtr parent) {
    EnvironmentPtr env = (EnvironmentPtr)malloc(sizeof(Environment));
    if(!env) return NULL;
    env->parent = parent;
    env->symbols = MakeMap();
    return env;
}

void FreeEnvironment(EnvironmentPtr env) {
    if(!env) return;
    FreeMap(env->symbols);
    free(env);
}

CellPtr EnvGet(EnvironmentPtr env, const char* name) {
    while(env) {
        CellPtr cell = (CellPtr)MapGet(env->symbols, name, NULL);
        if(cell) return cell;
        env = env->parent;
    }
    return NULL;
}

int EnvInstallSymbol(EnvironmentPtr env, const char* name, CellPtr cell) {
    if(!env) return 1;
    MapSet(env->symbols, name, cell);
    return 0;
}

int EnvInstallUnboundSymbols(EnvironmentPtr env, CellPtr symbols) {
    CellPtr ptr = CAR(symbols);
    CellPtr tail = CDR(symbols);
    while(ptr) {
        if(!ISSYMBOL(ptr)) return 1;
        EnvInstallSymbol(env, SYMNAME(ptr), CopyCell(ptr));
        ptr = CAR(tail);
        tail = CDR(tail);
    }
    return 0;
}

#include "primitives.h"

#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "env.h"
#include "proc.h"
#include "eval.h"
#include "cell.h"
#include "num.h"

#include "specials.h"

CellPtr scheme_add(EnvironmentPtr env, CellPtr operands) {
    CellPtr result = CAR(operands);
    CellPtr tail = CDR(operands);
    if(!ISNUM(result) || tail == NULL) return ErrorCell("*: bad syntax");
    while(tail && !ISNAN(result->num)) {
        CellPtr next = CAR(tail);
        if(!ISNUM(next)) return ErrorCell("*: bad syntax");
        result->num = AddNumA(result->num, next->num);
        tail = CDR(tail);
    }
    return result;
}

CellPtr scheme_sub(EnvironmentPtr env, CellPtr operands) {
    CellPtr result = CAR(operands);
    CellPtr tail = CDR(operands);
    if(!ISNUM(result) || tail == NULL) return ErrorCell("*: bad syntax");
    while(tail && !ISNAN(result->num)) {
        CellPtr next = CAR(tail);
        if(!ISNUM(next)) return ErrorCell("*: bad syntax");
        result->num = SubNumA(result->num, next->num);
        tail = CDR(tail);
    }
    return result;
}

CellPtr scheme_mult(EnvironmentPtr env, CellPtr operands) {
    CellPtr result = CAR(operands);
    CellPtr tail = CDR(operands);
    if(!ISNUM(result) || tail == NULL) return ErrorCell("*: bad syntax");
    while(tail && !ISNAN(result)) {
        CellPtr next = CAR(tail);
        if(!ISNUM(next)) return ErrorCell("*: bad syntax");
        result->num = MultNumA(result->num, next->num);
        tail = CDR(tail);
    }
    return result;
}

CellPtr scheme_div(EnvironmentPtr env, CellPtr operands) {
    CellPtr result = CAR(operands);
    CellPtr tail = CDR(operands);
    if(!ISNUM(result) || tail == NULL) return ErrorCell("*: bad syntax");
    while(tail && !ISNAN(result)) {
        CellPtr next = CAR(tail);
        if(!ISNUM(next)) return ErrorCell("*: bad syntax");
        result->num = DivNumA(result->num, next->num);
        tail = CDR(tail);
    }
    return result;
}

CellPtr scheme_car(EnvironmentPtr env, CellPtr operands) {
    CellPtr head = CAAR(operands);
    if(!head || CDR(operands) != NULL) return ErrorCell("car: bad syntax");
    return CopyCell(head);
}

CellPtr scheme_cdr(EnvironmentPtr env, CellPtr operands) {
    CellPtr tail = CDR(CAR(operands));
    if(!tail || CDR(operands) != NULL) return ErrorCell("cdr: bad syntax");
    return CopyCell(tail);
}

CellPtr scheme_cons(EnvironmentPtr env, CellPtr operands) {
    CellPtr item = CAR(operands);
    CellPtr list = CADR(operands);
    if(!item || !ISPAIR(list) || CDDR(operands) != NULL) return ErrorCell("cons: bad syntax");
    return PairCell(CopyCell(item), CopyCell(list));
}

CellPtr scheme_define(EnvironmentPtr env, const char* buffer, const char** next) {
    CellPtr variable_or_formals = read_value(env, buffer, next, 1);
    if(ISSYMBOL(variable_or_formals)) {
        CellPtr opt_expr = read_value(env, *next, next, 0);
        if(opt_expr) {
            CellPtr end = read_value(env, *next, next, 0);
            if(end != NULL) {
                FreeCell(end);
                return ErrorCell("define: bad syntax");
            }
            EnvInstallSymbol(env, SYMNAME(variable_or_formals), opt_expr);
        } else {
            EnvInstallSymbol(env, SYMNAME(variable_or_formals), EmptyCell());
        }
        FreeCell(variable_or_formals);
        return EmptyCell();
    } else if(ISPAIR(variable_or_formals)) {

    }
    return ErrorCell("define: bad syntax");
}

CellPtr scheme_lambda(EnvironmentPtr env, const char* buffer, const char**next) {
    CellPtr formals = read_value(env, buffer, next, 1);
    if(!ISPAIR(formals)) {
        if (formals) FreeCell(formals);
        return ErrorCell("lambda: bad syntax");
    }
    EnvironmentPtr closingEnv = MakeEnvironment(env);
    EnvInstallUnboundSymbols(closingEnv, formals);
    FreeCell(formals);
    while(1) {
        CellPtr expr = read_value(closingEnv, *next, next, 0);

    }
    return ErrorCell("lambda: bad syntax");
}


struct {
    const char* name;
    int specialForm;
    void *impl;
    void *extra;
} PRIMITIVES[] = {
    {"!print", 0, &scheme_print, NULL},
    {"!env", 0, &scheme_env, NULL},
    {"car", 0, &scheme_car, NULL},
    {"cdr", 0, &scheme_cdr, NULL},
    {"cons", 0, &scheme_cons, NULL},
    {"+", 0, &scheme_add, NULL},
    {"-", 0, &scheme_sub, NULL},
    {"*", 0, &scheme_mult, NULL},
    {"/", 0, &scheme_div, NULL},
    /* special forms */
    {"define", 1, &scheme_define, NULL},
    {"lambda", 1, &scheme_lambda, NULL},
    {NULL, 0, NULL, NULL}
};

int install_primitives(EnvironmentPtr env) {
    for(unsigned i=0;; i++) {
        if(!PRIMITIVES[i].name) break;
        #ifdef DEBUG
            printf("Installing primitive '%s'\n", PRIMITIVES[i].name);
        #endif
        ProcedurePtr proc = MakeProcedure(
            PRIMITIVES[i].name,
            PRIMITIVES[i].specialForm,
            PRIMITIVES[i].impl,
            PRIMITIVES[i].extra);
        EnvInstallSymbol(env, proc->name, ProcCell(proc->name, proc));
    }
    return 0;
}
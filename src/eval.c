#include "eval.h"

#include <stdio.h>
#include <stdlib.h>

#include "conv.h"
#include "lex.h"
#include "proc.h"
#include "cell.h"
#include "num.h"

CellPtr execute_procedure(EnvironmentPtr env, CellPtr proc, CellPtr operands) {
    if(!ISPROC(proc)) return ErrorCell("bad procedure");
    ProcedurePtr procInfo = (ProcedurePtr)proc->second;
    ProcedureImpl impl = (ProcedureImpl)procInfo->impl;
    CellPtr result = impl(env, operands);
    return result;
}

CellPtr evaluate(EnvironmentPtr env, CellPtr expr) {
    if(!expr) return ErrorCell("null expr");
    return execute_procedure(env, CAR(expr), CDR(expr));
}

CellPtr read_list(EnvironmentPtr env, const char* buffer, const char** next, int quoted) {
    CellPtr elem = read_value(env, buffer, next, quoted);
    if(!elem) return NULL;
    CellPtr tail = read_list(env, *next, next, quoted);
    if(!tail) return PairCell(elem, NULL);
    return PairCell(elem, tail);
}

CellPtr read_value(EnvironmentPtr env, const char* buffer, const char** next, int quoted) {
    while(1) {
        const char* start;
        int tokid = next_token(buffer, &start, next);
        switch(tokid) {            
            // TODO: differentiate end of list from no more tokens
            case T_NONE: return NULL;
            case T_RPAREN: return NULL;
            // ^ ^ ^ HACK ALERT ^ ^ ^
            
            case T_INT: return NumCell(MakeInteger(cstr_to_signed(start, *next)));
            case T_REAL: return NumCell(MakeFloat(cstr_to_float(start, *next)));
            case T_QUOTE: {quoted = 1; buffer = *next; continue; }
            
            case T_STRING: {
                char* token = extract_token(start, *next);
                if(token) {
                    CellPtr cell = StringCell(token);
                    free(token);
                    return cell;
                }
                return NULL;
            }
            
            case T_LPAREN: {
                CellPtr cell = read_list(env, *next, next, quoted);
                if(!quoted) return evaluate(env, cell);
                return cell;
            }

            case T_IDENT: {
                CellPtr cell = NULL;
                char* token = extract_token(start, *next);                
                if(quoted) {
                    cell = SymbolCell(token);
                } else {
                    cell = CopyCell(EnvGet(env, token));
                }
                free(token);
            
                if(ISPROC(cell)) {
                    ProcedurePtr procInfo = (ProcedurePtr)cell->second;
                    if(procInfo && procInfo->specialForm) {
                        SpecialFormImpl impl = procInfo->impl;
                        FreeCell(cell);
                        CellPtr result = impl(env, *next, next);
                        PrintCell(result, stdout); printf("\n");
                        return impl(env, *next, next);
                    }
                }
                return cell;                
            }
            case T_ERROR:
            default: return ErrorCell("lexical error");
        }
    }
}
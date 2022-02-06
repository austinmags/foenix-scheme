#include "cell.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

CellPtr EmptyCell() {
    CellPtr cell = (CellPtr)malloc(sizeof(Cell));
    if(cell) cell->flags = (C_EMPTY);
    return cell;
}

CellPtr NumCell(NumPtr num) {
    CellPtr cell = EmptyCell();
    if(cell) {
        cell->flags = C_NUM;
        cell->num = CopyNum(num);
    }
    return cell;
}

CellPtr StringCell(const char* value) {
    CellPtr cell = EmptyCell();
    if(cell) {
        cell->flags = (C_STRING);
        size_t len = strlen(value);
        cell->sval = (char *)malloc(len+1);
        strcpy(cell->sval, value);
    }
    return cell;
}

CellPtr SymbolCell(const char* value) {
    CellPtr cell = StringCell(value);
    if(cell) cell->flags = (C_SYMBOL);
    return cell;
}

CellPtr ProcCell(const char* name, void* func) {
    CellPtr cell = EmptyCell();
    if (cell) {
        cell->flags = C_PROC;
        cell->first = StringCell(name);
        cell->second = func;
    }
    return cell;
}

CellPtr PairCell(CellPtr first, CellPtr second) {
    CellPtr cell = EmptyCell();
    if(cell) {
        cell->flags = C_PAIR;
        cell->first = CopyCell(first);
        cell->second = CopyCell(second);
    }
    return cell;
}

CellPtr ErrorCell(const char*fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    CellPtr cell = StringCell(buffer);
    if(cell) cell->flags = C_ERROR;
    return cell;
}

CellPtr CopyCell(CellPtr cell) {
    if(!cell) return NULL;
    switch(cell->flags) {
        case C_EMPTY: return EmptyCell();
        case C_NUM: return NumCell(cell->num);
        case C_STRING: return StringCell(cell->sval);
        case C_SYMBOL: return SymbolCell(cell->sval);
        case C_PROC: return ProcCell(cell->first->sval, cell->second);
        case C_PAIR: return PairCell(cell->first, cell->second);
        case C_ERROR: return ErrorCell(cell->sval);
        default: return NULL;
    }
}

void FreeCell(CellPtr cell) {
    if(!cell) return;
    if(cell->flags == C_STRING || cell->flags == C_SYMBOL || cell->flags == C_ERROR) free(cell->sval);
    else if(cell->flags == C_PROC) {
        FreeCell(cell->first);
    }
    else if(cell->flags == C_PAIR) {
        FreeCell(cell->first);
        FreeCell((CellPtr)cell->second);
    }
    free(cell);
}

void PrintCell(CellPtr cell, FILE* f) {
    if(!cell) {
        fprintf(f, "!NULL!");
        return;
    } else if(cell->flags == C_EMPTY) {
        fprintf(f, "'nil");
    } else if(cell->flags == C_NUM) {
        NumPtr num = cell->num;
        if(num->flags == N_NAN) fprintf(f, "'nan");
        else if(num->flags == N_INT) fprintf(f, "%ld", num->lval);
        else if(num->flags == N_FLOAT) fprintf(f, "%g", num->fval);
        else fprintf(f, "'bad");
    } else if(cell->flags == C_STRING) {
        fprintf(f, "\"%s\"", cell->sval);
        return;
    } else if(cell->flags == C_SYMBOL) {
        fprintf(f, "'%s", cell->sval);
        return;
    } else if(cell->flags == C_PAIR) {
        CellPtr head = CAR(cell);
        CellPtr next = head;
        CellPtr tail = CDR(cell);
        fprintf(f, "(");
        while(next) {
            if(next != head) fprintf(f, " ");
            PrintCell(next, f);
            next = CAR(tail);
            tail = CDR(tail);
        }
        fprintf(f, ")");
    } else if(cell->flags == C_PROC) {
        CellPtr name = cell->first;
        void *impl = cell->second;
        fprintf(f, "[proc:%s@%p]", name->sval, impl);
    } else if(cell->flags == C_ERROR) {
        fprintf(f, "error: %s", cell->sval);
    } else {
        fprintf(f, "[?:%u]", cell->flags);
    }
    

}
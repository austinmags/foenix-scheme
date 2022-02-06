#ifndef __CELL_H
#define __CELL_H

#include <stdio.h>

#include "num.h"

#define C_UNDEFINED                     0
#define C_EMPTY                         1
#define C_NUM                           2
#define C_STRING                        4
#define C_SYMBOL                        5
#define C_PROC                          6
#define C_PAIR                          7
#define C_ERROR                         8

typedef struct CELL {
    unsigned short flags;
    union {
        char* sval;
        void* proc;
        NumPtr num;
        struct {
            struct CELL* first;
            void* second;
        };
    };
} Cell, *CellPtr;

CellPtr EmptyCell();                            // Makes an empty cell
CellPtr NumCell(NumPtr num);                    // Make a number cell
CellPtr StringCell(const char*);                // String cell
CellPtr SymbolCell(const char*);                // Symbol cell
CellPtr ProcCell(const char*, void*);           // Proc cell
CellPtr PairCell(CellPtr first, CellPtr second);// Pair
CellPtr ErrorCell(const char*, ...);            // Error

CellPtr CopyCell(CellPtr);
void FreeCell(CellPtr);

void PrintCell(CellPtr, FILE*);

#define TYPE(p)             (p != NULL ? p->flags : C_UNDEFINED)

#define ISPAIR(p)           (p && p->flags == C_PAIR)
#define ISPROC(p)           (p && p->flags == C_PROC)
#define ISSYMBOL(p)         (p && p->flags == C_SYMBOL)
#define ISNUM(p)            (p && p->flags == C_NUM)
#define CAR(p)              (ISPAIR(p) ? p->first : NULL)
#define CDR(p)              ((CellPtr)(ISPAIR(p) ? p->second : NULL))
#define CAAR(p)             (CAR(CAR(p)))
#define CADR(p)             (CAR(CDR(p)))
#define CDDR(p)             (CDR(CDR(p)))
#define SYMNAME(p)          ((const char *)(ISSYMBOL(p) ? p->sval : NULL))

#endif
#ifndef __PROC_H
#define __PROC_H

#include "env.h"
#include "cell.h"

typedef struct PROC {
    char* name;
    int specialForm;
    void* impl;
    void* extra;
} Procedure, *ProcedurePtr;

typedef CellPtr(*SpecialFormImpl)(EnvironmentPtr, const char*, const char**);
typedef CellPtr(*ProcedureImpl)(EnvironmentPtr, CellPtr);

ProcedurePtr MakeProcedure(const char *name, int specialForm, void* impl, void *extra);
void FreeProcedure(ProcedurePtr proc);

#endif
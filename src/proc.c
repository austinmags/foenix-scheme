#include "proc.h"

#include <stdlib.h>
#include <string.h>

ProcedurePtr MakeProcedure(const char *name, int specialForm, void* impl, void *extra) {
    ProcedurePtr proc = (ProcedurePtr)malloc(sizeof(Procedure));
    if(!proc) return NULL;
    size_t len = strlen(name);
    proc->name = (char *)malloc(len+1);
    memcpy(proc->name, name, len+1);
    proc->specialForm = specialForm;
    proc->impl = impl;
    proc->extra = extra;
    return proc;
}

void FreeProcedure(ProcedurePtr proc) {
    if(!proc) return;
    if(proc->name) free(proc->name);
    free(proc);
}

#include "num.h"

#include <stdlib.h>

NumPtr MakeInteger(long value) {
    NumPtr num = (NumPtr)malloc(sizeof(Num));
    if(num) {
        num->flags = N_INT;
        num->lval = value;
    }
    return num;
}

NumPtr MakeFloat(float value) {
    NumPtr num = (NumPtr)malloc(sizeof(Num));
    if(num) {
        num->flags = N_FLOAT;
        num->fval = value;
    }
    return num;
}

NumPtr MakeNAN() {
    NumPtr num = (NumPtr)malloc(sizeof(Num));
    if(num) {
        num->flags = N_NAN;
    }
    return num;
}

NumPtr CopyNum(NumPtr num) {
    switch(num->flags) {
        case N_INT: return MakeInteger(num->lval);
        case N_FLOAT: return MakeFloat(num->fval);
        default: break;
    }
    return MakeNAN();
}

NumPtr NaNNumber(NumPtr num) {
    if(num) {
        num->flags = N_NAN;
    }
    return num;
}

void TryCoerce(NumPtr num, unsigned totype) {
    if(num->flags != totype) {
        if(num->flags == N_INT && totype == N_FLOAT) {
            num->flags = N_FLOAT;
            num->fval = (float)num->lval;
        }
    }
}

NumPtr NegateNum(NumPtr dst) {
    if(dst->flags == N_INT) {
        dst->lval = -(dst->lval);
    } else if(dst->flags == N_FLOAT) {
        dst->fval = -(dst->fval);
    }
    return dst;
}

// Dst = Dst + N1
NumPtr AddNumA(NumPtr dst, NumPtr n1) {
    TryCoerce(dst, n1->flags);
    switch(dst->flags | n1->flags) {
        case N_INT: { dst->lval += n1->lval; break; }
        case N_FLOAT: { dst->fval += n1->fval; break; }
        case N_INT | N_FLOAT: { dst->fval += (float)n1->fval; break;}
        default: dst = NaNNumber(dst);
    }
    return dst;
}

// Dst = Dst - N1
NumPtr SubNumA(NumPtr dst, NumPtr n1) {
    TryCoerce(dst, n1->flags);
    switch(dst->flags | n1->flags) {
        case N_INT: { dst->lval -= n1->lval; break; }
        case N_FLOAT: { dst->fval -= n1->fval; break; }
        case N_INT | N_FLOAT: { dst->fval -= (float)n1->fval; break;}
        default: dst = NaNNumber(dst);
    }
    return dst;
}

// Dst = Dst * N1
NumPtr MultNumA(NumPtr dst, NumPtr n1) {
    TryCoerce(dst, n1->flags);
    switch(dst->flags | n1->flags) {
        case N_INT: { dst->lval *= n1->lval; break; }
        case N_FLOAT: { dst->fval *= n1->fval; break; }
        case N_INT | N_FLOAT: { dst->fval *= (float)n1->fval; break;}
        default: dst = NaNNumber(dst);
    }
    return dst;
}

// Dst = Dst / N1
NumPtr DivNumA(NumPtr dst, NumPtr n1) {
    if(ISZERO(n1)) return NaNNumber(dst);
    TryCoerce(dst, n1->flags);
    switch(dst->flags | n1->flags) {
        case N_INT: { dst->lval /= n1->lval; break; }
        case N_FLOAT: { dst->fval /= n1->fval; break; }
        case N_INT | N_FLOAT: { dst->fval /= (float)n1->fval; break;}
        default: NaNNumber(dst);
    }
    return dst;
}

void FreeNum(NumPtr num) {
    if(num) free(num);
}

#ifndef __NUM_H
#define __NUM_H

#define N_NAN               1
#define N_INT               2
#define N_FLOAT             4


typedef struct NUM {
    unsigned short flags;
    union {
        long lval;
        float fval;
    };
} Num, *NumPtr;

#define ISNAN(n)            (n && n->flags == N_NAN)
#define ISZERO(n)           (n && n->flags == N_INT ? n->lval == 0 : n && n->flags == N_FLOAT ? n->fval == 0 : 0==1)

NumPtr MakeInteger(long);
NumPtr MakeFloat(float);
NumPtr MakeNAN();

NumPtr CopyNum(NumPtr);
void FreeNum(NumPtr num);

NumPtr NegateNum(NumPtr dst);
// Dst = Dst + N1
NumPtr AddNumA(NumPtr dst, NumPtr n1);
// Dst = Dst - N1
NumPtr SubNumA(NumPtr dst, NumPtr n1);
// Dst = Dst * N1
NumPtr MultNumA(NumPtr dst, NumPtr n1);
// Dst = Dst / N1
NumPtr DivNumA(NumPtr dst, NumPtr n1);

#endif
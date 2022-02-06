#ifndef __LEX_H
#define __LEX_H

#define T_NONE                  0
#define T_ERROR                 1
#define T_COMMENT               2
#define T_IDENT                 10
#define T_INT                   20
#define T_REAL                  21
#define T_STRING                29
#define T_LPAREN                30
#define T_RPAREN                31
#define T_QUOTE                 32

#include <stdlib.h>
#include <stdio.h>

typedef struct TOKEN {
    char *token;
    int id;
    union {
        long lval;
        float fval;
        char *sval;
    };
    struct TOKEN* next;
} Token, *TokenPtr;

typedef struct SOURCE {
    unsigned short flags;
    void* source;
    void* getchfn;
    size_t pos;
    struct SOURCE *prev;
    struct SOURCE *next;
} Source, *SourcePtr;

typedef int(*GetCharImpl)(SourcePtr, int, int);

typedef struct LEXER {
    SourcePtr srcFirst;
    SourcePtr srcLast;
    TokenPtr tokFirst;
    TokenPtr tokLast;
} Lexer, *LexerPtr;

LexerPtr MakeLexer();
void PushBufferSource(LexerPtr, const char*);
void PushFileSource(LexerPtr, FILE*);
TokenPtr NextToken(LexerPtr);
void PushTokenBack(LexerPtr, TokenPtr);

// TODO: replace "end" with "next"
// returns a T_ value identifying the token type
// *start will be the start of the token
// *end will be the end of the token + 1 (i.e., start of next token)
// T_NONE means no more tokens are available from the buffer
// T_ERROR is a lexical error *end will identify the unexpected character
int next_token(const char*buffer, const char** start, const char** end);
int expect_next_token(int tokid, const char*buffer, const char** next);
char *extract_token(const char* start, const char* end);

#endif
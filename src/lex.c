#include "lex.h"

#include <stdlib.h>
#include <string.h>

#define ISWS(x) (x == '\t' || x == '\n' || x == '\r' || x == 32)
#define ISDIG(x) (x >= '0' && x <= '9')
#define ISIDENT(x) (!ISWS(x) && x != '.' && x != '(' && x != ')' && x != '#' && x != '\"' && x != '\'')

#define SOURCE_FILE 0
#define SOURCE_BUFF 1

LexerPtr MakeLexer() {
    LexerPtr lexer = (LexerPtr)malloc(sizeof(Lexer));
    if(lexer) {
        lexer->srcFirst = NULL;
        lexer->srcLast = NULL;
        lexer->tokFirst = NULL;
        lexer->tokLast = NULL;
    }
    return lexer;
}

SourcePtr MakeSource(void* source, int type) {
    SourcePtr s = (SourcePtr)malloc(sizeof(Source));
    if(s) {
        s->flags = type;
        s->source = source;
        s->pos = 0;
        s->next = NULL;
        s->prev = NULL;
    }
    return s;
}

void PushSource(LexerPtr lexer, SourcePtr source) {
    if(!source || !lexer) return;
    if(!lexer->srcFirst) {
        lexer->srcFirst = source;
        lexer->srcLast = source;
    } else {
        source->prev = NULL;
        source->next = lexer->srcFirst;
        lexer->srcFirst = source;
    }
}

void PushBufferSource(LexerPtr lexer, const char* buffer) {
    SourcePtr source = MakeSource(buffer, SOURCE_BUFF);
    if(source) PushSource(lexer, source);
}

void PushFileSource(LexerPtr lexer, FILE* file) {
    SourcePtr source = MakeSource(file, SOURCE_FILE);
    if(source) PushSource(lexer, source);
}

TokenPtr NextToken(LexerPtr lexer) {
    if(!lexer) return NULL;
    // pull off the queue if there's a token enqueued there
    if(lexer->tokFirst) {
        TokenPtr token = lexer->tokFirst;
        lexer->tokFirst = lexer->tokFirst->next;
        return token;
    }
    return FetchToken(lexer->srcFirst);
}

void PushTokenBack(LexerPtr lexer, TokenPtr token) {
    if(!lexer || !token) return;
    token->next = NULL;
    if(!lexer->tokFirst) {
        lexer->tokFirst = token;
        lexer->tokLast = token;
    } else {
        lexer->tokLast->next = token;
        lexer->tokLast = token;
    }
}

TokenPtr FetchToken(SourcePtr source) {
    if(!source) return NULL;
    GetCharImpl getchar = source->getchfn;

    while(ISWS(getchar(source, 0, 1)))
        getchar(source, 0, 0);

    int ch = getchar(source, 0, 0);
    if(ch == -1) return MakeToken(T_NONE, NULL);



}


// TODO: replace name "end" with "next" which is more accurate
int next_token(const char*buffer, const char** start, const char** end) {
    *start = buffer;
    // skip whitespace
    while(ISWS(**start)) ++*start;
    *end = *start;
    // end of buffer
    if(!**start) return T_NONE;
    // line comment
    else if(**end == ';') {
        while(*++*end && **end != '\n' && (**end == '\r' && *end[1] == '\n'));
        return T_COMMENT;
    }
    else if(**end == '(') {
        ++*end;
        return T_LPAREN;
    } else if (**end == ')') {
        ++*end;
        return T_RPAREN;
    } else if (**end == '\'') {
        ++*end;
        return T_QUOTE;
    }
    else if(**end == '\"') {
        while(*++*end && **end != '\"');
        if(!*end) return T_ERROR;
        ++*end;
        return T_STRING;
    }
    // Integer and real
    else if(ISDIG(**end)) {
        while(*++*end && ISDIG(**end));
        if(**end == '.') {
            while(*++*end && ISDIG(**end));
            return T_REAL;
        }
        return T_INT;
    } else {
        while(*++*end && ISIDENT(**end));
        return T_IDENT;
    }
    //
    return T_ERROR;
}

int expect_next_token(int tokid, const char*buffer, const char** next) {
    const char* start;
    return next_token(buffer, &start, next) == tokid;
}

char *extract_token(const char* start, const char* end) {
    size_t len = end-start;
    char *token = (char *)malloc(len+1);
    memcpy(token, start, len);
    token[len] = 0;
    return token;
}
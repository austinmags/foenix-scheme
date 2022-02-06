#include "conv.h"

#include <stdio.h>

signed cstr_to_signed(const char* start, const char* end) {
    signed val = 0;
    signed sign = 1;
    const char* p = start;

    if(*p == '-') {
        sign = -1;
        ++p;
    } else if(*p == '+') {
        ++p;
    }
    
    while(*p && p != end) {
        if(*p >= '0' && *p <= '9') {
            val = val * 10 + (*p - '0');
            ++p;
        } else {
            break;
        }
    }
    return val * sign;
}

float cstr_to_float(const char* start, const char* end) {
    float val = 0;
    float sign = 1;
    const char* p = start;

    if(*p == '-') { 
        sign = -1.0;
        ++p;
    } else if (*p == '+') {
        ++p;
    }

    while(*p && p != end) {
        if(*p >= '0' && *p <= '9') {
            val = val * 10.0 + (*p - '0');
            ++p;
        }
        else if(*p == '.' || *p == 'e' || *p == 'E') {
            break;
        } else {
            return val;
        }
    }
    if(*p == '.') {
        ++p;
        float factor = .1;
        while(*p && p != end) {
            if(*p >= '0' && *p <= '9') {
                val = val + ((*p - '0') * factor);
                factor *= 0.1;
                ++p;
            } else if(*p == 'e' || *p == 'E') {
                break;
            } else {
                return val;
            }
        }        
    }
    if(*p == 'e' || *p == 'E') {
        return val;
    }
    return val;
}
#include "specials.h"

#include <stdio.h>

#include "map.h"
#include "cell.h"

CellPtr scheme_env(EnvironmentPtr env, CellPtr operands) {
    while(env) {
        printf("Environment: %p (parent=%p)\n", env, env->parent);
        print_map(env->symbols);
        printf("\n");        
        env = env->parent;
    }
    return EmptyCell();
}

CellPtr scheme_print(EnvironmentPtr env, CellPtr operands) {
    PrintCell(operands, stdout);
    return EmptyCell();
}
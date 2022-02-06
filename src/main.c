#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "eval.h"
#include "env.h"
#include "map.h"
#include "primitives.h"

const char* GREETING = "Foenix Scheme 0.1 - 2021-01-31\n"
                        "https://github/somewhere\n"
                        "\n"
                        "!help for help\n"
                        "!quit to exit\n";

void help() {
    printf("!help or !h - shows command information\n");
    printf("!quit or !q - exits the environment\n");
}

int main() {
    printf("%s\n", GREETING);

    // set up the default environments
    EnvironmentPtr top = MakeEnvironment(NULL);
    install_primitives(top);
    EnvironmentPtr user = MakeEnvironment(top);

    while(1) {
        char line[256];
        printf("sch>");
        fgets(line, sizeof(line), stdin);

        const char* next;
        CellPtr cell = read_value(user, line, &next, 0);
        if(cell) {
            PrintCell(cell, stdout);
            printf("\n");
            FreeCell(cell);
        }
    }
}
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"
#include "config.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))
#define STATUS_LENGTH (LENGTH(blocks) * BLOCK_OUTPUT_LENGTH + 1)
#define INTERVAL 25

char block_outputs[LENGTH(blocks)][BLOCK_OUTPUT_LENGTH + DELIMITER_LENGTH];
char status[STATUS_LENGTH];
char old_status[STATUS_LENGTH];
bool done = false;

void print_status()
{
    // Concatenate the command outputs
    strcpy(old_status, status);
    status[0] = '\0';
    for (unsigned int i = 0; i < LENGTH(blocks); i++)
    {
        strcat(status, block_outputs[i]);
        if (!blocks[i].argument.no_delimiter)
            strcat(status, delimiter);
    }
    status[strlen(status) - DELIMITER_LENGTH] = '\0';

    // Print the status if it changed
    if (strcmp(status, old_status) != 0)
    {
        puts(status);
        fflush(stdout);
    }
}

void signal_handler(int signal)
{
    for (unsigned int i = 0; i < LENGTH(blocks); i++)
        if (blocks[i].signal == signal - SIGRTMIN)
            blocks[i].render(&blocks[i].argument, block_outputs[i]);
    print_status();
}

void terminate_handler()
{
    done = true;
}

int main()
{
    if (DELIMITER_LENGTH != strlen(delimiter))
        fputs("warning: Delimiter length mismatch\n", stderr);

    signal(SIGTERM, terminate_handler);
    signal(SIGINT, terminate_handler);

    for (unsigned int i = 0; i < LENGTH(blocks); i++)
    {
        if (blocks[i].signal > 0)
            signal(SIGRTMIN + blocks[i].signal, signal_handler);
        if (blocks[i].init != NULL)
            blocks[i].init();
    }

    unsigned int time = 0;

    while (!done)
    {
        // Execute due blocks
        for (unsigned int i = 0; i < LENGTH(blocks); i++)
        {
            if (blocks[i].poll != NULL)
                blocks[i].poll(&blocks[i], &blocks[i].argument, block_outputs[i]);

            unsigned int interval = blocks[i].interval;
            if (blocks[i].render != NULL && (time == 0 || (interval != 0 && time % interval == 0)))
                blocks[i].render(&blocks[i].argument, block_outputs[i]);
        }
        time += INTERVAL;

        // Print the status
        print_status();

        usleep(INTERVAL * 1000);
    }

    for (unsigned int i = 0; i < LENGTH(blocks); i++)
        if (blocks[i].deinit != NULL)
            blocks[i].deinit();

    return 0;
}


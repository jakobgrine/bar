#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"
#include "config.h"

#ifndef NO_X
#include <X11/Xlib.h>
#endif // NO_X

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))
#define STATUS_LENGTH (LENGTH(blocks) * BLOCK_OUTPUT_LENGTH + 1)
#define INTERVAL 25

char block_outputs[LENGTH(blocks)][BLOCK_OUTPUT_LENGTH + DELIMITER_LENGTH];
char status[STATUS_LENGTH];
char old_status[STATUS_LENGTH];
bool done = false;

#ifndef NO_X
Display *dpy;
int screen;
Window root;

int setupX()
{
    if (!(dpy = XOpenDisplay(NULL)))
    {
        fputs("setupX: Failed to open X display\n", stderr);
        return 0;
    }
    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);
    return 1;
}
#endif // NO_X

void write_status()
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

    // Write the status if it changed
    if (strcmp(status, old_status) != 0)
    {
#ifndef NO_X
        if (mode == MODE_X)
        {
            XStoreName(dpy, root, status);
            XFlush(dpy);
        }
        else
#endif // NO_X
        if (mode == MODE_STDOUT)
        {
            puts(status);
            fflush(stdout);
        }
    }
}

void signal_handler(int signal)
{
    for (unsigned int i = 0; i < LENGTH(blocks); i++)
        if (blocks[i].signal == signal - SIGRTMIN)
            blocks[i].render(&blocks[i].argument, block_outputs[i]);
    write_status();
}

void terminate_handler()
{
    done = true;
}

int main(int argc, char *argv[])
{
    char opt;
    while ((opt = getopt(argc, argv, "xo")) != -1 )
    {
        switch (opt)
        {
#ifndef NO_X
            case 'x':
                mode = MODE_X;
                break;
#endif // NO_X
            case 'o':
                mode = MODE_STDOUT;
                break;
            default:
                fprintf(stderr, "Usage: %s [-xo]\n", argv[0]);
                return 1;
        }
    }

#ifndef NO_X
    if (mode == MODE_X && !setupX())
        return 1;
#endif // NO_X

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
                blocks[i].poll(&blocks[i].argument, block_outputs[i]);

            unsigned int interval = blocks[i].interval;
            if (blocks[i].render != NULL && (time == 0 || (interval != 0 && time % interval == 0)))
                blocks[i].render(&blocks[i].argument, block_outputs[i]);
        }
        time += INTERVAL;

        // Write the status
        write_status();

        usleep(INTERVAL * 1000);
    }

#ifndef NO_X
    if (mode == MODE_X)
        XCloseDisplay(dpy);
#endif // NO_X

    for (unsigned int i = 0; i < LENGTH(blocks); i++)
        if (blocks[i].deinit != NULL)
            blocks[i].deinit();

    return 0;
}


#include "bspwm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <xcb/xcb.h>

int bspwm_sock_fd;
bool bspwm_module_active = false;

unsigned int get_num_monitors()
{
    int sock_fd;
    struct sockaddr_un sock_address;
    sock_address.sun_family = AF_UNIX;

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        fputs("init_bspwm: Failed to create the socket\n", stderr);

    char *sp = getenv("BSPWM_SOCKET");
    if (sp != NULL)
        snprintf(sock_address.sun_path, sizeof(sock_address.sun_path), "%s", sp);
    else
    {
        char *host = NULL;
        int dsp = 0, srn = 0;
        if (xcb_parse_display(NULL, &host, &dsp, &srn) != 0)
            snprintf(sock_address.sun_path, sizeof(sock_address.sun_path), "/tmp/bspwm%s_%i_%i-socket", host, dsp, srn);
        free(host);
    }

    if (connect(sock_fd, (struct sockaddr *) &sock_address, sizeof(sock_address)) == -1)
        fputs("init_bspwm: Failed to connect to the socket\n", stderr);

    char *msg = "query\0-M\0\0";
    if (send(sock_fd, msg, 9, 0) == -1)
        fputs("init_bspwm: Failed to send the data\n", stderr);

    struct pollfd fds[] = {
        {sock_fd, POLLIN, 0},
    };
    int nb, num = 0;
    char rsp[BUFSIZ];

    while (poll(fds, 1, -1) > 0) {
        if (fds[0].revents & POLLIN) {
            if ((nb = recv(sock_fd, rsp, sizeof(rsp) - 1, 0)) > 0)
            {
                rsp[nb] = '\0';
                if (rsp[0] == '\x07')
                    fprintf(stderr, "%s", rsp + 1);
                else
                    for (unsigned int i = 0; i < strlen(rsp); i++)
                        if (rsp[i] == '\n')
                            num++;
            }
            else
                break;
        }
     }

     close(sock_fd);
     return num;
}

void init_bspwm()
{
    if (bspwm_module_active)
    {
        fputs("init_bspwm: Bspwm module is already initialized\n", stderr);
        return;
    }
    bspwm_module_active = true;

    struct sockaddr_un sock_address;
    sock_address.sun_family = AF_UNIX;

    if ((bspwm_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        fputs("init_bspwm: Failed to create the socket\n", stderr);

    char *sp = getenv("BSPWM_SOCKET");
    if (sp != NULL)
        snprintf(sock_address.sun_path, sizeof(sock_address.sun_path), "%s", sp);
    else
    {
        char *host = NULL;
        int dsp = 0, srn = 0;
        if (xcb_parse_display(NULL, &host, &dsp, &srn) != 0)
            snprintf(sock_address.sun_path, sizeof(sock_address.sun_path), "/tmp/bspwm%s_%i_%i-socket", host, dsp, srn);
        free(host);
    }

    if (connect(bspwm_sock_fd, (struct sockaddr *) &sock_address, sizeof(sock_address)) == -1)
        fputs("init_bspwm: Failed to connect to the socket\n", stderr);

    char *msg = "subscribe\0report\0\0";
    if (send(bspwm_sock_fd, msg, 17, 0) == -1)
        fputs("init_bspwm: Failed to send the data\n", stderr);
}

void render_bspwm(const Argument *argument, char *output, char *input)
{
    char buf[BLOCK_OUTPUT_LENGTH];
    char *bufp = buf;
    char *inputp = input;

    bool monitor_focused;

    while (1)
    {
        char *endp = inputp;
        while (*endp != ':' && *endp != '\0')
            endp++;
        bool already_null = *endp == '\0';
        if (!already_null)
            *endp = '\0';

        // Get the color for the current block
        char col_fg[8], col_bg[8], col_ul[8];
        switch (*inputp)
        {
            case 'm':
                strcpy(col_fg, "#8dbcdf");
                strcpy(col_bg, "#000000");
                monitor_focused = false;
                break;
            case 'M':
                strcpy(col_fg, "#b1d0e8");
                strcpy(col_bg, "#144b6c");
                monitor_focused = true;
                break;
            case 'f':
                strcpy(col_fg, "#737171");
                strcpy(col_bg, "#000000");
                strcpy(col_ul, "#000000");
                break;
            case 'F':
                strcpy(col_fg, monitor_focused ? "#000000" : "#737171");
                strcpy(col_bg, monitor_focused ? "#504e4e" : "#000000");
                strcpy(col_ul, "#504e4e");
                break;
            case 'o':
                strcpy(col_fg, "#a7a5a5");
                strcpy(col_bg, "#000000");
                strcpy(col_ul, "#000000");
                break;
            case 'O':
                strcpy(col_fg, monitor_focused ? "#d6d3d2" : "#a7a5a5");
                strcpy(col_bg, monitor_focused ? "#504e4e" : "#000000");
                strcpy(col_ul, "#504e4e");
                break;
            case 'u':
                strcpy(col_fg, "#f15d66");
                strcpy(col_bg, "#000000");
                strcpy(col_ul, "#000000");
                break;
            case 'U':
                strcpy(col_fg, monitor_focused ? "#501d1f" : "#f15d66");
                strcpy(col_bg, monitor_focused ? "#d5443e" : "#000000");
                strcpy(col_ul, "#d5443e");
                break;
            case 'L':
            case 'T':
            case 'G':
                strcpy(col_fg, "#89b09c");
                break;
        }

        // Write the current block to the output buffer
        unsigned int n = 0;
        switch (*inputp)
        {
            case 'm':
            case 'M':
                n = snprintf(bufp, BLOCK_OUTPUT_LENGTH - strlen(buf), "%%{F%s}%%{B%s}%%{A:bspc monitor -f %s:} %s %%{A}%%{B-}%%{F-}", col_fg, col_bg, inputp + 1, inputp + 1);
                break;
            case 'f':
            case 'F':
            case 'o':
            case 'O':
            case 'u':
            case 'U':
                n = snprintf(bufp, BLOCK_OUTPUT_LENGTH - strlen(buf), "%%{F%s}%%{B%s}%%{U%s}%%{A:bspc desktop -f %s:}%%{+u} %s %%{-u}%%{A}%%{U-}%%{B-}%%{F-}", col_fg, col_bg, col_ul, inputp + 1, inputp + 1);
                break;
            case 'L':
            case 'T':
            case 'G':
                n = snprintf(bufp, BLOCK_OUTPUT_LENGTH - strlen(buf), "%%{F%s} %s %%{F-}", col_fg, inputp + 1);
                break;
        }

        if (!already_null)
            *endp = ':';
        else
            break;

        bufp += n;
        inputp = endp + 1;
    }

    if (argument->bspwm.all_monitors)
    {
        char next_mon[] = "%{S+}";
        unsigned int next_mon_len = strlen(next_mon);
        unsigned int len = strlen(buf);
        unsigned int num = get_num_monitors();
        for (unsigned int i = 1; i < num; i++)
        {
            strcpy(buf + i * (len + next_mon_len) - next_mon_len, next_mon);
            memmove(buf + i * (len + next_mon_len), buf, len);
            *(buf + i * (len + next_mon_len) + len) = '\0';
        }
    }

    snprintf(output, BLOCK_OUTPUT_LENGTH, "%s", buf);
}

void poll_bspwm(const Argument *argument, char *output)
{
    struct pollfd bspwm_fds[] = {
        {bspwm_sock_fd, POLLIN, 0},
    };

    if (bspwm_module_active)
    {
        if (poll(bspwm_fds, 1, 0) < 0)
            fputs("poll_bspwm: Failed to poll bspwm socket\n", stderr);
        else if (bspwm_fds[0].revents & POLLIN)
        {
            char buf[BUFSIZ];
            int n = recv(bspwm_sock_fd, buf, sizeof(buf) - 1, 0);
            if (n > 0)
            {
                buf[n] = '\0';
                int len = strlen(buf);
                buf[len - 1] *= buf[len - 1] != '\n';
                if (buf[0] == '\7')
                    fprintf(stderr, "poll_bspwm: Socket error: %s", buf + 1);
                else
                    render_bspwm(argument, output, buf + 1);
            }
            else
                fputs("poll_bspwm: Nothing received\n", stderr);
        }
    }
}

void deinit_bspwm()
{
    close(bspwm_sock_fd);
}


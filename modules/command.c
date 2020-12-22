#include "command.h"
#include <stdio.h>
#include <string.h>

void render_command(const Argument *argument, char *output)
{
    FILE *cmd = popen(argument->command.command, "r");
    if (!cmd)
    {
        fprintf(stderr, "render_command: Failed to execute the command '%s'", argument->command.command);
        return;
    }

    char buffer[BLOCK_OUTPUT_LENGTH];
    // Read command output
    if (fgets(buffer, BLOCK_OUTPUT_LENGTH, cmd) != NULL)
    {
        // Delete trailing newline
        unsigned int output_length = strlen(buffer);
        buffer[output_length - 1] *= buffer[output_length - 1] != '\n';
    }
    else
        buffer[0] = '\0';

    // Apply command output to format string
    snprintf(output, BLOCK_OUTPUT_LENGTH, argument->command.format, buffer);

    pclose(cmd);
}


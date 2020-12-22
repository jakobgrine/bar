#ifndef __MODULES_COMMAND_H
#define __MODULES_COMMAND_H

#include "../common.h"

#define MODULE_COMMAND .init = NULL, .render = render_command, .poll = NULL, .deinit = NULL

void render_command(const Argument *argument, char *output);

#endif // __MODULES_COMMAND_H


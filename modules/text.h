#ifndef __MODULES_TEXT_H
#define __MODULES_TEXT_H

#include "../common.h"

#define MODULE_TEXT .init = NULL, .render = render_text, .poll = NULL, .deinit = NULL

void render_text(const Argument *argument, char *output);

#endif // __MODULES_TEXT_H


#ifndef __MODULES_BACKLIGHT_H
#define __MODULES_BACKLIGHT_H

#include "../common.h"

#define MODULE_BACKLIGHT .init = NULL, .render = render_backlight, .poll = NULL, .deinit = NULL

void render_backlight(const Argument *argument, char *output);

#endif // __MODULES_BACKLIGHT_H


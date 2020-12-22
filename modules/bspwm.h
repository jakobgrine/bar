#ifndef __MODULES_BSPWM_H
#define __MODULES_BSPWM_H

#include "../common.h"

#define MODULE_BSPWM .init = init_bspwm, .render = NULL, .poll = poll_bspwm, .deinit = deinit_bspwm

void init_bspwm();
void render_bspwm(const Argument *argument, char *output, char *input);
void poll_bspwm(const Argument *argument, char *output);
void deinit_bspwm();

#endif // __MODULES_BSPWM_H


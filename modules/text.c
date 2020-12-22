#include "text.h"
#include <string.h>

void render_text(const Argument *argument, char *output)
{
    strcpy(output, argument->text.text);
}


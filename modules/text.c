#include "text.h"
#include <string.h>

void render_text(const Argument *argument, char *output)
{
    strncpy(output, argument->text.text, BLOCK_OUTPUT_LENGTH - 1);
}


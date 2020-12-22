#include "common.h"
#include "modules/command.h"

static const Block blocks[] = {
/*   interval, signal, module,         argument */
    {1000,     0,      MODULE_COMMAND, { .command = { "%s", "date +'%F %T'" } } },
};

#define DELIMITER_LENGTH 3
static char delimiter[] = " | ";


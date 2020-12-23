#include "backlight.h"
#include <stdio.h>

int read_backlight_sysfs(char *driver, char *value, int *output)
{
    char filename[128];
    int n = snprintf(filename, sizeof(filename), "/sys/class/backlight/%s/%s", driver, value);
    if (n < 0 || n >= sizeof(filename))
    {
        fprintf(stderr, "render_backlight: Failed to get the filename for: %s\n", value);
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "render_backlight: Failed to open the file: %s\n", filename);
        return -1;
    }

    if (fscanf(file, "%d", output) < 1)
    {
        fprintf(stderr, "render_backlight: Failed to read the file: %s\n", filename);
        fclose(file);
        return -1;
    }
    fclose(file);

    return 0;
}

void render_backlight(const Argument *argument, char *output)
{
    int brightness;
    if (read_backlight_sysfs(argument->backlight.driver, "brightness", &brightness) < 0)
        return;

    int max_brightness;
    if (read_backlight_sysfs(argument->backlight.driver, "max_brightness", &max_brightness) < 0)
        return;

    double percentage = (double)brightness * 100.0 / (double)max_brightness;

    snprintf(output, BLOCK_OUTPUT_LENGTH, argument->backlight.format, percentage);
}


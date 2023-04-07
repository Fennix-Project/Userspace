#include <libdraw.h>

#include "../../../Kernel/syscalls.h"

void DrawRect(int x, int y, int w, int h, int color)
{
    int i, j;
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
            // TODO: DrawPixel(x + i, y + j, color);
        }
    }
}

#include <assert.h>
#include <string.h>

#include "bresenham.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    struct point origin = {0, 0};
    struct point dst = {0, 2};
    struct point output[4];
    struct point answer[] = {{0, 0}, {0, 1}, {0, 2}};
    size_t len;

    len = bresenham(&origin, &dst, output, lengthof(output));

    assert(len == lengthof(answer));
    assert(memcmp(output, answer, sizeof(answer)) == 0);

    return 0;
}

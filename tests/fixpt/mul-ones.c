#include <assert.h>

#include "fixpt.h"

int main(int argc, char *argv[])
{
    assert(fixpt_to_int(fixpt_mul(fixpt_from_int(1), fixpt_from_int(1))) == 1);

    return 0;
}

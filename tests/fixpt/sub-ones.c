#include <assert.h>

#include "fixpt.h"

int main(int argc, char *argv[])
{
    assert(fixpt_to_int(fixpt_sub(fixpt_from_int(1), fixpt_from_int(1))) == 0);

    return 0;
}

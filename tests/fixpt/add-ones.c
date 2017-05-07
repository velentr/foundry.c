#include <assert.h>

#include "fixpt.h"

int main(int argc, char *argv[])
{
    assert(fixpt_to_int(fixpt_add(fixpt_from_int(1), fixpt_from_int(1))) == 2);

    return 0;
}

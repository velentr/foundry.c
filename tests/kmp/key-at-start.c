#include <assert.h>
#include <string.h>

#include "kmp.h"

int main(int argc, char *argv[])
{
    size_t rc;
    char *key = "this";
    char *str = "this is a test";

    rc = kmp(key, strlen(key), str, strlen(str));
    assert(rc == 0);

    return 0;
}


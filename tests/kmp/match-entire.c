#include <assert.h>
#include <string.h>

#include "kmp.h"

int main(int argc, char *argv[])
{
    char table[32];
    size_t rc;
    char *key = "this is a test";
    char *str = "this is a test";

    rc = kmp(key, table, strlen(key), str, strlen(str));
    assert(rc == 0);

    return 0;
}


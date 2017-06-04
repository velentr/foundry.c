#include <assert.h>
#include <string.h>

#include "kmp.h"

int main(int argc, char *argv[])
{
    size_t table[32];
    size_t rc;
    char *key = "test1";
    char *str = "this is a test";

    rc = kmp(key, table, strlen(key), str, strlen(str));
    assert(rc == strlen(str));

    return 0;
}


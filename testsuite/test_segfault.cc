// -*- mode: c++; fill-column: 79 -*-

/* This program intensionally segfaults as a test for test_execpipe. */

int main()
{
    char* nullptr = 0;

    *nullptr = 1;

    return 0;
}

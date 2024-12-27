#include <stdio.h>
#include "dynamic-lib/dynamiclib.h"
#include "static-lib/staticlib.h"

int main()
{
    printf("Main Application: Starting...\n");

    callStaticLibFun();
    callDynamicLibFun();

    printf("Main Application: Finished.\n");
    return 0;
}

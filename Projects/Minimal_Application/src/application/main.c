#include <stdio.h>
#include "module1/module1.h"
#include "module2/module2.h"

int main()
{
    printf("Calling module1 function:\n");
    module1_function();

    printf("Calling module2 function:\n");
    module2_function();
    return 0;
}

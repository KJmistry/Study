#include <stdio.h>
#include "module1.h"
#include "typedef.h"

int main()
{
    printf("%d\n", getArrValue(9));
    if (arr[0] == 10)
    {
        printf("Inside If\n");
        return 1;
    }

    printf("Return 0\n");
    /* code */
    return 0;
}
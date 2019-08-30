#include <stdio.h>

int main(){
    int x = 1;
    int y = 2;

    int* a= &x;
    int* b= a;

    printf("%d,%d\n",*a,*b);
    a=&y;
    printf("%d,%d\n",*a,*b);
    return 0;
}
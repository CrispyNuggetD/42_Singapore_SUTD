#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (void)
{
    char s[5];
    strcpy(s,"hey!");

    char *t = "telegram";
    printf("Printing string t... %s",t);


    // Learning using Malloc properly (Rmb to free memory!!)
    // Only explicitly cast Malloc with (int *) in C++.
    // As if #not_included <stdlib.h>, compiler doesn't warn.

    int *c = malloc(sizeof(int));
    if (c == NULL) // Check if Malloc is successful.
    {
        printf("failed memory alloc!");
        return 10;
    }

    *c = 27;
    printf("\n%%p expects void *. %%p is %p\n",(void*)c); // %p expects void *
    printf("*c dereferences 'c' which is a pointer to heap-alloc memory. *c is %d\n",*c); //*c to dereference

    int x = 3;
    int *ptr = &x;

    printf("\nImpt!! Printing *ptr is actually %d, as it dereferences &x \n",*ptr);
    printf("%p\n", ptr);
    printf("* is the dereferencing operator\n");

    free(c); // ALWAYS free! Type free first when Malloc-ed.
}
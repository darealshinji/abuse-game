#include <ctype.h>
#include <stdio.h>

main()
{
    int i;
    for (i=0;i<256;i++) {
        if (isprint(i))
            printf("%c",i);
        else printf(" ");
        if (i%32==31)
            printf("\n");
    }
}

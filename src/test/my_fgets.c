#include <stdio.h>

int main() {
    while (1) { //Infinite loop
        char s[4096];
        printf("Input : ");
        fgets(s,sizeof(s),stdin); //
        printf("Output : %s\n",s);
    }
    return 0;
}
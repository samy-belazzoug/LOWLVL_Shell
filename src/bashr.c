#include "bashr.h"

int main() {   
    while (1) {
        char s[N_TTY_BUF_SIZE];
        printf("Input : ");
        fgets(s,sizeof(s),stdin); //
        printf("Output : %s\n",s);
    }
}
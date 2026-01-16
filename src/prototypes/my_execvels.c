#include <libsync.h>

int main() {   
    char *args[] = {"/bin/man","execve",NULL};
    execve(args[0],args,__environ);
    return 0;
}
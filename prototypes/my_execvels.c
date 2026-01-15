#include <libsync.h>

int main() {   
    char *args[] = {"/bin/ls","-a",NULL};
    execve(args[0],args,__environ);
    return 0;
}
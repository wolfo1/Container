#ifndef _CONTAINER_H_
#define _CONTAINER_H_
#define MOUNT_ERR "system error: could not mount proc.\n"
#include <iostream>
#include <sys/unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <sys/mount.h>

typedef struct Container {
    char* hostName;
    char* rootDir;
    int numOfProcesses;
    int pid;
    char* processFileSystem;
} Container;



#endif //_CONTAINER_H_

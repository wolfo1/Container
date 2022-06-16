#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <iostream>
#include <sys/unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <sys/mount.h>
#include <sys/wait.h>
#include <list>
#include <experimental/filesystem>

#define STACK_SIZE 8192

#define MOUNT_ERR "system error: could not mount proc.\n"
#define CLONE_ERR "system error: clone system call failed.\n"
#define WAIT_ERR "system error: wait system call failed.\n"
#define ALLOC_ERR "system error: malloc failed to allocate memory.\n"
#define SETHOST_ERR "system error: sethostname system call failed.\n"
#define CHROOT_ERR "system error: chroot system call failed.\n"
#define MKDIR_ERR "system error: mkdir system call failed.\n"
#define CHDIR_ERR "system error: chdir system call failed.\n"
#define OPEN_PROCS_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.procs\n"
#define OPEN_MAX_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.max\n"
#define OPEN_NOTIFY_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.notify_on_release\n"
#define REMOVE_DIR_ERR "system error: could not remove directory.\n"

typedef struct Container {
    char* hostName;
    char* rootDir;
    char* numOfProcesses;
    std::string processFileSystem;
    char** programArgs;

    Container(char** argv, int argc)
    {
        this->hostName = argv[1];
        this->rootDir = argv[2];
        this->numOfProcesses = argv[3];
        this->processFileSystem = std::string(argv[4]);
        // create a new array for program arguments
        programArgs = new char*[argc - 3];
        for (int i = 0; i < argc - 3; i++)
        {
            programArgs[i] = *(argv + i + 4);
        }
        programArgs[argc - 4] = (char *) 0;
    }

    ~Container()
    {
        delete[] programArgs;
    }
} Container;

void errorHandler(const char* errorMessage);
int newContainer(void* args);
int newProcess(int argc, char** argv);
int get_connection(int s);

#endif //_CONTAINER_H_

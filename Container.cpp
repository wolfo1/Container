#include "Container.h"


void errorHandler(const char* errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

int newContainer(void* args)
{
    auto* container = (Container*) args;
    // change root directory
    if (chroot(container->rootDir) == -1)
        errorHandler(CHROOT_ERR);
    // go into root directory
    if (chdir("/") == -1)
        errorHandler(CHDIR_ERR);
    // create hostname
    if (sethostname(container->hostName, strlen(container->hostName)) == -1)
        errorHandler(SETHOST_ERR);
    // create directories
    const char* const dirs[] = {"/sys/fs", "/sys/fs/cgroup", "/sys/fs/cgroup/pids"};
    for (const char* dir : dirs)
    {
        if (mkdir(dir, 0755) == -1)
        {
            errorHandler(MKDIR_ERR);
        }
    }
    // write into procs, max and notify_on_release files
    std::ofstream procs;
    procs.open("/sys/fs/cgroup/pids/cgroup.procs");
    if(!procs)
        errorHandler(OPEN_PROCS_ERR);
    procs << getpid();
    procs.close();
    std::ofstream pidMax;
    pidMax.open("/sys/fs/cgroup/pids/pids.max");
    if(!pidMax)
        errorHandler(OPEN_MAX_ERR);
    pidMax << container->numOfProcesses;
    pidMax.close();
    // write 1 into notify_on_release file
    std::ofstream notifyOnRelease;
    notifyOnRelease.open("/sys/fs/cgroup/pids/notify_on_release");
    if(!notifyOnRelease)
        errorHandler(OPEN_NOTIFY_ERR);
    notifyOnRelease << "1";
    notifyOnRelease.close();
    // mount procs
    if (mount("proc", "/proc", "proc", 0, 0) != 0)
    	errorHandler(MOUNT_ERR);
    if (execv(container->processFileSystem.c_str(), container->programArgs) == -1)
        errorHandler(CLONE_ERR);
    return 0;
}

int newProcess(int argc, char** argv)
{
    // allocate a new stack
    char* containerStack = new (std::nothrow) char[STACK_SIZE];
    if (containerStack == nullptr)
        errorHandler(ALLOC_ERR);
    // create a new container
    auto* container = new Container {argv, argc};
    int pid = clone(newContainer, containerStack + STACK_SIZE, CLONE_NEWPID|CLONE_NEWUTS|CLONE_NEWNS|SIGCHLD, container);
    if (pid == -1)
    {
        free(containerStack);
        delete container;
        errorHandler(CLONE_ERR);
    }
    if(wait(nullptr) < 0)
    {
        delete containerStack;
        delete container;
        errorHandler(WAIT_ERR);
    }
    // delete directories
    std::string rootDirStr = container->rootDir;
    std::error_code ec;
    std::experimental::filesystem::remove_all(rootDirStr + "/sys/fs", ec);
    if (ec)
    {
        delete containerStack;
        delete container;
        errorHandler(REMOVE_DIR_ERR);
    }
    // unmount
    std::string procFilepath = rootDirStr + "/proc";
    if (umount(procFilepath.c_str()) != 0)
    {
        delete containerStack;
        delete container;
        errorHandler(MOUNT_ERR);
    }
    // free memory
    delete container;
    delete containerStack;
    return 0;
}


int main(int argc, char* argv[])
{
    // number of arguments should be at least 4 (5 with program name)
    if (argc < 5)
        exit(EXIT_FAILURE);
    // create process
    newProcess(argc, argv);
    exit(EXIT_SUCCESS);
}

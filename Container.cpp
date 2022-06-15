#include "Container.h"

#define ALLOC_ERR "system error: malloc failed to allocate memory.\n"
#define SETHOST_ERR "system error: sethostname system call failed.\n"
#define CHROOT_ERR "system error: chroot system call failed.\n"
#define MKDIR_ERR "system error: mkdir system call failed.\n"
#define CHDIR_ERR "system error: chdir system call failed.\n"
#define OPEN_PROCS_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.procs\n"
#define OPEN_MAX_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.max\n"
#define OPEN_NOTIFY_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.notify_on_release\n"
#define STACK_SIZE 8192

Container* newContainer(char*  hostName, char* rootDir, int numOfProcesses,
                        char* processFileSystem, int pid, void* args)
{
  auto* container = new Container {hostName, rootDir, numOfProcesses, pid, processFileSystem};
  if (container == nullptr)
  {
    std::cerr << ALLOC_ERR;
    return nullptr;
  }
  // create hostname
  if (sethostname(container->hostName, strlen(container->hostName)) == -1)
    {
      std::cerr << SETHOST_ERR;
      delete container;
      return nullptr;
    }
  // change root directory
  if (chroot(container->rootDir) == -1)
    {
      std::cerr << CHROOT_ERR;
      delete container;
      return nullptr;
    }
  // go into root directory
  if (chdir(container->rootDir) == -1)
    {
      std::cerr << CHDIR_ERR;
      delete container;
      return nullptr;
    }
  // create cgroup
  if (mkdir("sys/fs/cgroup/pids", 0755) == -1)
    {
      std::cerr << MKDIR_ERR;
      delete container;
      return nullptr;
    }
  // write into procs, max and notify_on_release files
  std::ofstream procs;
  procs.open("/sys/fs/cgroup/pids/cgroup.procs");
  if(!procs)
    {
      std::cerr << OPEN_PROCS_ERR;
      delete container;
      return nullptr;
    }
  procs << container->pid;
  procs.close();

  std::ofstream pidMax;
  pidMax.open("/sys/fs/cgroup/pids/pids.max");
  if(!pidMax)
    {
      std::cerr << OPEN_MAX_ERR;
      delete container;
      return nullptr;
    }
  pidMax << container->numOfProcesses;
  pidMax.close();
  // write 1 into notify_on_release file
  std::ofstream notifyOnRelease;
  notifyOnRelease.open("/sys/fs/cgroup/pids/notify_on_release");
  if(!notifyOnRelease)
    {
      std::cerr << OPEN_NOTIFY_ERR;
      delete container;
      return nullptr;
    }
  notifyOnRelease << 1;
  notifyOnRelease.close();

  // mount procs
  if (mount("proc", "/proc", "proc", 0, 0) == -1)
    {
      std::cerr << MOUNT_ERR;
      delete container;
      return nullptr;
    }
    return container;
}

int newProcess(char** argv)
{
  void* containerStack = malloc(STACK_SIZE);
  if (containerStack == nullptr)
  {
    std::cerr << ALLOC_ERR;
    return -1;
  }
  Container* container = newContainer(argv[1], argv[2],
                                      atoi(argv[3]), argv[4],
                                      1, argv[5]);
  if (container == nullptr)
  {
    free(containerStack);
    return -1;
  }
  int pid = clone(newContainer, (char*)containerStack + STACK_SIZE,
                  CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWNET,
                 argv[5]);
  if (pid == -1)
  {
    std::cerr << CLONE_ERR;
    free(containerStack);
    delete container;
    return -1;
  }
  if(wait(NULL) != 0)
  {
    std::cerr << WAIT_ERR;
    free(containerStack);
    delete container;
    return -1;
  }
  // delete directories

  // umount

  // free memory
  delete container;
  free(containerStack);
  return 0;
}

void deleteDirectories(Container* container)
{

}
// main
int main(int argc, char** argv)
{
  // number of arguments should be at least 4 (5 with program name)
  if (argc < 5)
    exit(1);
  // create process
  if (newProcess(argv) != 0)
    exit(1);

}
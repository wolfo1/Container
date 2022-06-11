#include "Container.h"

#define SETHOST_ERR "system error: sethostname system call failed.\n"
#define CHROOT_ERR "system error: chroot system call failed.\n"
#define MKDIR_ERR "system error: mkdir system call failed.\n"
#define CHDIR_ERR "system error: chdir system call failed.\n"
#define OPEN_PROCS_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.procs\n"
#define OPEN_MAX_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.max\n"
#define OPEN_NOTIFY_ERR "system error: could not open /sys/fs/cgroup/pids/cgroup.notify_on_release\n"

// main
int main(int argc, char* argv[])
{
  // number of arguments should be at least 4 (5 with program name)
  if (argc < 5)
    return -1;
  Container container;
  container.hostName = argv[1];
  container.rootDir = argv[2];
  container.numOfProcesses = atoi(argv[3]);
  container.processFileSystem = argv[4];
  container.pid = 1;
  // create hostname
  if (sethostname(container.hostName, strlen(container.hostName)) == -1)
  {
    std::cerr << SETHOST_ERR << std::endl;
    exit(1);
  }
  // change root directory
  if (chroot(container.rootDir) == -1)
  {
    std::cerr << CHROOT_ERR << std::endl;
    exit(1);
  }
  // go into root directory
  if (chdir(container.rootDir) == -1)
  {
    std::cerr << CHDIR_ERR << std::endl;
    exit(1);
  }
  // create cgroup
  if (mkdir("sys/fs/cgroup/pids", 0755) == -1)
  {
    std::cerr << MKDIR_ERR << std::endl;
    exit (1);
  }
  // write into procs, max and notify_on_release files
  std::ofstream procs;
  procs.open("/sys/fs/cgroup/pids/cgroup.procs");
  if(!procs)
  {
    std::cerr << OPEN_PROCS_ERR << std::endl;
    exit(1);
  }
  procs << container.pid;
  procs.close();

  std::ofstream pidMax;
  pidMax.open("/sys/fs/cgroup/pids/pids.max");
  if(!pidMax)
  {
    std::cerr << OPEN_MAX_ERR << std::endl;
    exit(1);
  }
  pidMax << container.numOfProcesses;
  pidMax.close();
  // write 1 into notify_on_release file
  std::ofstream notifyOnRelease;
  notifyOnRelease.open("/sys/fs/cgroup/pids/notify_on_release");
  if(!notifyOnRelease)
  {
    std::cerr << OPEN_NOTIFY_ERR << std::endl;
    exit(1);
  }
  notifyOnRelease << 1;
  notifyOnRelease.close();

  // mount procs
  if (mount("proc", "/proc", "proc", 0, 0) == -1)
  {
    std::cerr << MOUNT_ERR << std::endl;
    exit(1);
  }

}
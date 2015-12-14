#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME  "prettykey"

int init_mpty(void)
{
  int masterfd;

  masterfd = posix_openpt(O_RDWR);
  if (masterfd == -1
    || grantpt(masterfd) == -1
    || unlockpt(masterfd) == -1)
  {
    perror(PROGRAM_NAME);
    return -1;
  }
  return masterfd;
}

int main(void)
{
  int masterfd, slavefd;

  if ((masterfd = init_mpty()) == -1)
    return EXIT_FAILURE;

  slavefd = open(ptsname(masterfd), O_RDWR);
  if (slavefd == -1)
  {
    perror(PROGRAM_NAME);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

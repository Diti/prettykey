#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROGRAM_NAME  "prettykey"
#define GNUPG_BINARY  "gpg"

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

  char  *gnupg_args[] =
  {
    GNUPG_BINARY,
    "--gen-key",
    "--expert",
    NULL
  };
  if (execvp(gnupg_args[0], gnupg_args) == -1)
  {
    if (errno == ENOENT)
      fprintf(stderr, "%s: %s was not found in your PATH.\n", PROGRAM_NAME, GNUPG_BINARY);
    else
      perror(PROGRAM_NAME);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include <util.h>

#define PROGRAM_NAME  "prettykey"
#define GNUPG_BINARY  "gpg"

void
usage(char *program_name)
{
  (void)program_name;
  printf("%s <passphrase>\n", PROGRAM_NAME);
  exit(EX_USAGE);
}

int
pty_setup(int *fd)
{
  pid_t pid = forkpty(fd, NULL, NULL, NULL);
  char *gnupg_args[] = { GNUPG_BINARY, "--gen-key", "--expert", NULL };

  if (pid == -1) {
    perror("forkpty");
    exit(EX_OSERR);
  } else if (pid == 0) {
    if (execvp(gnupg_args[0], gnupg_args) == -1) {
      if (errno == ENOENT) {
        fprintf(stderr, "%s: %s was not found in your PATH.\n", PROGRAM_NAME, GNUPG_BINARY);
        return 0;
      } else {
        perror("execvp");
        return 0;
      }
    }
    return 1;
  } else {
    close(0);
  }

  // Set non-blocking
  int flags;
  if ((flags = fcntl(*fd, F_GETFL, 0)) == -1)
    flags = 0;
  if (fcntl(*fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl");
    return 0;
  }
  return 1;
}

static void
process_input(int fd)
{
  char buf[255];
  while (1) {
    int nread = read(fd, buf, 254);
    if (nread == -1) {
      if (errno == EAGAIN) {
        usleep(1000);
        continue;
      }
      perror("read");
      break;
    }
    int i;
    for (i = 0; i < nread; i++) {
      putchar(buf[i]);
    }
  }
}

int main(int argc, char *argv[])
{
  int fd;

  if (argc < 2) {
    usage(argv[0]);
  }

  if (!pty_setup(&fd)) {
    perror("pty_setup()");
    exit(EX_OSERR);
  }

  write(fd, "8\n", 2);

  process_input(fd);
  return EX_OK;
}

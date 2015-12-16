#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#if defined(__linux__)
# include <pty.h>
#elif defined(__APPLE__) || defined(__FreeBSD__)
# include <util.h>
#endif

#define PROGRAM_NAME  "prettykey"
#define GNUPG_BINARY  "gpg"

#define GPG_UID_NAME    "Dimitri Torterat"
#define GPG_UID_EMAIL   ""
#define GPG_UID_COMMENT "born 1990-05-26 in Melun, France"

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
  char *gnupg_args[] = {
    GNUPG_BINARY,
    "--gen-key",
    "--expert",
    "--no-default-keyring",
    "--keyring=" PROGRAM_NAME "_pubring.gpg",
    "--secret-keyring=" PROGRAM_NAME "secring.gpg",
    NULL
    //"--no-use-agent",
  };

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

inline static void
write_endl(int fd, const char *str)
{
  write(fd, str, strlen(str));
  write(fd, "\n", 1);
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

  write_endl(fd, "8");
  write_endl(fd, "S");
  write_endl(fd, "E");
  write_endl(fd, "Q");
  write_endl(fd, "4096");
  write_endl(fd, "0");
  write_endl(fd, "y");
  write_endl(fd, GPG_UID_NAME);
  write_endl(fd, GPG_UID_EMAIL);
  write_endl(fd, GPG_UID_COMMENT);
  write_endl(fd, "O");
  write_endl(fd, argv[1]);
  write_endl(fd, argv[1]);

  process_input(fd);
  return EX_OK;
}

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

#define PROGRAM_NAME    "prettykey"
#define GNUPG_BINARY    "gpg2"
#define GNUPG_SUBDIR    "GnuPG"

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

static char *
gnupghome_dir(void)
{
  char  *cwd;
  int   ret;
  char  *str;

  cwd = realpath(".", NULL);
  if (!cwd)
  {
    perror("realpath()");
    exit(EX_OSERR);
  }

  ret = asprintf(&str, "%s/%s", cwd, GNUPG_SUBDIR);
  if (ret == -1)
  {
    perror("asprintf");
    exit(EX_OSERR);
  }
  return str;
}

static char *
gnupghome_arg()
{
  int   ret;
  char  *str;
  char  *tmp;

  tmp = gnupghome_dir();

  ret = asprintf(&str, "--home=%s", tmp);
  if (ret == -1 || !tmp)
  {
    perror("asprintf");
    exit(EX_OSERR);
  }
  free(tmp);
  return str;
}

void
call_gnupg_gen(void)
{
  char  *home_arg;

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(EX_OSERR);
  } else if (pid == 0) {

    char *gnupg_args[] = {
      GNUPG_BINARY,
      home_arg = gnupghome_arg(),
      "--full-gen-key",
      "--expert",
      "--command-file=commands.txt",
      NULL
    };

    if (execvp(gnupg_args[0], gnupg_args) == -1) {
      if (errno == ENOENT) {
        fprintf(stderr, "%s: %s was not found in your PATH.\n", PROGRAM_NAME, GNUPG_BINARY);
        exit(EX_UNAVAILABLE);
      } else {
        perror("execvp");
        exit(EX_OSERR);
      }
    }
    free(home_arg);
    home_arg = NULL;
    exit(EX_OK);
  } else {
    int ret;
    waitpid(pid, &ret, 0);
    if (ret == 0) {
      // GnuPG exited normally
    } else {
      // GnuPG terminated with an error
    }
  }
  puts("Done.");
}

/*
 * This simple program works with GnuPG 2.1 and these settings:
 * # https://wiki.archlinux.org/index.php/GnuPG#Unattended_passphrase
 * - [~/.gnupg/gpg-agent.conf] allow-loopback-pinentry
 * - [~/.gnupg/gpg.conf] pinentry-mode loopback
 */
int main(int argc, char *argv[])
{
  if (argc < 2) {
    usage(argv[0]);
  }

  char *gpghome = gnupghome_dir();
  if (mkdir(gpghome, 0700) == -1) {
    if (errno != EEXIST) {
      perror("mkdir");
    }
  }

  call_gnupg_gen();
  return EX_OK;
}

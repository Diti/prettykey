#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

#define PROGRAM_NAME  "prettykey"
#define GNUPG_BINARY  "gpg2"

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

void
call_gnupg_gen(void)
{
  pid_t pid = fork();
  char *gnupg_args[] = {
    GNUPG_BINARY,
    "--full-gen-key",
    "--expert",
    "--no-default-keyring",
    "--keyring=" PROGRAM_NAME "_pubring.gpg",
    "--secret-keyring=" PROGRAM_NAME "_secring.gpg",
	"--command-file=commands.txt",
    NULL
  };

  if (pid == -1) {
    perror("fork");
    exit(EX_OSERR);
  } else if (pid == 0) {
    if (execvp(gnupg_args[0], gnupg_args) == -1) {
      if (errno == ENOENT) {
        fprintf(stderr, "%s: %s was not found in your PATH.\n", PROGRAM_NAME, GNUPG_BINARY);
        exit(EX_UNAVAILABLE);
      } else {
        perror("execvp");
        exit(EX_OSERR);
      }
    }
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

  call_gnupg_gen();
  return EX_OK;
}

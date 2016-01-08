#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#define PROGRAM_NAME "prettykey"
#define GNUPG_BINARY "gpg2"
#define GNUPG_SUBDIR "GnuPG"

void
usage(void)
{
    puts(PROGRAM_NAME);
    puts("  This program has no defined options yet.");
    exit(EX_USAGE);
}

static char *gnupghome_dir(void) {
    char *cwd;
    int ret;
    char *str;

    cwd = realpath(".", NULL);
    if (!cwd) {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "realpath",
                strerror(errno));
        exit(EX_OSERR);
    }

    ret = asprintf(&str, "%s/%s", cwd, GNUPG_SUBDIR);
    if (ret == -1 || !str) {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "asprintf",
                strerror(errno));
        exit(EX_OSERR);
    }

    return str;
}

static char *gnupghome_arg() {
    int ret;
    char *str;
    char *tmp;

    tmp = gnupghome_dir();

    ret = asprintf(&str, "--homedir=%s", tmp);
    if (ret == -1 || !tmp) {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "asprintf",
                strerror(errno));
        exit(EX_OSERR);
    }

    free(tmp);
    return str;
}

void call_gnupg_gen(void) {
    char *home_arg;
    int p[2];

    if (pipe(p) == -1) {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "pipe", strerror(errno));
        exit(EX_OSERR);
    }

    pid_t pid = fork();

    if (pid == -1) {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "fork", strerror(errno));
        exit(EX_OSERR);
    } else if (pid == 0) {
        close(p[0]);

        while ((dup2(p[1], STDOUT_FILENO) == -1)) {
            if (errno != EINTR) {
                fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "dup2",
                        strerror(errno));
                exit(EX_OSERR);
            }
        }
#if defined(DEBUG)
        while ((dup2(p[1], STDERR_FILENO) == -1)) {
            if (errno != EINTR) {
                fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "dup2",
                        strerror(errno));
                exit(EX_OSERR);
            }
        }
#endif

        close(p[1]);

        char *gnupg_args[] = {
            GNUPG_BINARY,
#if defined(DEBUG)
            "--verbose",
#endif
            home_arg = gnupghome_arg(),
            "--full-gen-key",
            "--expert",
            "--command-file=commands.txt",
            "--passphrase-file=passphrase.txt",
            "--pinentry-mode=loopback",
            NULL
        };

        if (execvp(gnupg_args[0], gnupg_args) == -1) {
            if (errno == ENOENT) {
                fprintf(stderr, "%s: %s was not found in your PATH.\n",
                        PROGRAM_NAME, GNUPG_BINARY);
                exit(EX_UNAVAILABLE);
            } else {
                fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, GNUPG_BINARY,
                        strerror(errno));
                exit(EX_OSERR);
            }
        }
        free(home_arg);
        home_arg = NULL;
        _Exit(EX_OK);
    } else {
        int ret;

        close(p[1]);
        waitpid(pid, &ret, 0);

#if defined(DEBUG)
        char buffer[4096] = {0};
        while (read(p[0], buffer, sizeof(buffer)) != 0) {
            printf("\n%s\n", buffer);
        }
#endif

        if (ret == 0) {
            // GnuPG exited normally
            puts("Key generation succeeded.");
        } else {
            fprintf(stderr, "Key generation failed: %s returned an error.\n",
                    GNUPG_BINARY);
            _Exit(EX_OSERR);
        }

        _Exit(EX_OK);
    }
}

/*
 * This simple program works with GnuPG 2.1 and these settings:
 * # https://wiki.archlinux.org/index.php/GnuPG#Unattended_passphrase
 * - [~/.gnupg/gpg-agent.conf] allow-loopback-pinentry
 * - [~/.gnupg/gpg.conf] pinentry-mode loopback
 */
int main(int argc, char *argv[]) {
    (void)argv;
    if (argc != 1)
        usage();

    char *gpghome = gnupghome_dir();

    if (mkdir(gpghome, 0700) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "%s: mkdir %s: %s\n", PROGRAM_NAME, GNUPG_SUBDIR,
                    strerror(errno));
            exit(EX_OSERR);
        }
    }

    if (chmod(gpghome, 0700) == -1) // Enforce secure permissions
    {
        fprintf(stderr, "%s: chmod %s: %s\n", PROGRAM_NAME, GNUPG_SUBDIR,
                strerror(errno));
        exit(EX_OSERR);
    }

    free(gpghome);
    gpghome = NULL;

    call_gnupg_gen();

    exit(EX_OK);
}

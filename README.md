Please do not use this.

Usage
=====

Build the binary with `make`.

(Optional) Try running `gpg2 --full-gen-key --expert` and see what commands fit your uses.
Report said commands in file `commands.txt`, using your own information.
Change the passphrase in file `passphrase.txt` with your own.

Then:

```sh
# `prettykey` needs to use its own `gpg-agent`; other instances must be killed
killall gpg-agent

# Run `prettykey` without any parameter (yet).
./prettykey
```

FAQ
===

> I get `fatal error: errno.h: No such file or directory` when I try to compile!

It is a [known bug in Ubuntu](https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/825574). Consider installing the `gcc-multilib` package.

> Why did you bother writing this tool?

I wanted a PGP key with particular requirements like my old one:

    sec#  4096R/00FF24DC 1990-01-01  usage: C
    uid                  John Doe <john.doe@example.com>
    uid                  John Doe (born 1970-01-01)
    ssb   4096R/EA86A7C0 1990-01-01 usage: S
    ssb   4096R/7FA7A91F 1990-01-01 usage: E
    ssb   4096R/FEE2CE5B 1990-01-01 usage: A

Automated, unattended PGP key generation is possible with GnuPG’s `--batch` option, but it is impossible to create a cert-only secret key with it. Also, I enjoy the concept of an easy-to-remember PGP key shortid.

Also, I like writing C code and control every part of the workflow.

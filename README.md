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

    sec#  4096R/CD42FF00 2013-05-14  usage: C
    uid                  Dimitri Torterat <kra@diti.me>
    uid                  Dimitri Torterat (born 1990-05-26 in Melun, France)
    ssb   4096R/0C7A68AE 2013-05-14  usage: S
    ssb   4096R/F19A7AF7 2013-05-14  usage: E
    ssb   4096R/B5EC2EEF 2013-05-14  usage: A

Automated, unattended PGP key generation is possible with GnuPG’s `--batch` option, but it is impossible to create a cert-only secret key with it. Also, I enjoy the concept of an easy-to-remember PGP key shortid.

Also, I like writing C code and control every part of the workflow.

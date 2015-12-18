Usage
=====

```
# `prettykey` needs to use its own `gpg-agent`; other instances must be killed
killall gpg-agent
# Run `prettykey` without any parameter (yet).
./prettykey
```

Building
========

```
make
```

Ubuntu
------

In case you encounter the following error during compilation:

```
> fatal error: errno.h: No such file or directory
>  #include <errno.h>
>                    ^
```

This is a [known bug](https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/825574).
Consider installing the `gcc-multilib` package.

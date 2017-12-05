## Introduction

`edln` is a simple readline-based utility for interactively editing the target
of a symbolic link. This is most useful if you only need to make a relatively
minor change to the symlink.


## Using `edln`

Typically, `edln` is called with a single argument -- the path to the symlink
to be edited. For example, if you have a symlink `foo` that points to the
file `/usr/local/bin/foo-1.0`, and you want to update it to point to the file
`/usr/local/bin/foo-1.1` instead, you would invoke `edln` as follows (where the
`$` denotes the shell prompt):

```
  $ edln foo
  New target: /usr/local/bin/foo-1.0
```

As you can see, `edln` displays the current symlink target, and allows you to
edit it. Now press the `Backspace` key once to delete the `0` character, then
press the `1` key to insert the `1` character, and finally, press the `Enter`
key to complete the update.

As `edln` relies on GNU Readline for editing, it supports much more
sophisticated editing than demonstrated in the simple example above.
Refer to the GNU Readline User Manual for further details:

  https://tiswww.case.edu/php/chet/readline/rluserman.html

As a final note, `edln` also supports a two-argument invocation that is
basically equivalent to `ln -sfT`. That is, `edln foo /usr/local/bin/foo-1.1`
has the same effect as `ln -sfT /usr/local/bin/foo-1.1 foo`.

Note that the `-T` option causes `ln` to behave differently when the symlink
`foo` points to a directory. For example, suppose `foo` points to `dir1`, and
you want to change it to point to `dir2` instead. Running `ln -sf dir2 foo`
will not accomplish this -- instead, it will create inside `dir1` a new symlink
`dir2` (that points to `dir2`). In contrast, running `edln foo dir2` or
`ln -sfT dir2 foo` will have the desired effect.


## Pre-Built Binaries

Binaries for select platforms are available at:

  https://github.com/jjlin/edln/downloads

For other platforms, `edln` should not be difficult to build from source.


## Building From Source

A simple makefile is provided with the `edln` source code, available at:

  https://github.com/jjlin/edln/

You need to ensure that the GNU Readline development packages are installed
(e.g., `libreadline-dev` on Debian/Ubuntu, or `readline-devel` on RHEL/CentOS).

You may also need to ensure that the GNU Ncurses package is installed,
particularly on RHEL/CentOS, where linking with the `-lncurses` flag is
required. On RHEL/CentOS, the package name is `ncurses`.

Exact details for building from source on other Unix-like systems may vary,
but should be quite similar.

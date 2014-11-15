# SDCC 3.4.0 support for NASCOM 2 running NASSYS-3 (and 1?)

Includes a crt0_nascom.s which provides the essential support for NASCOM 2.

## Features

* `putchar()` and `getchar()` (and thus all derived IO like `printf()` etc.) use the NASSYS IO.

Note, `putchar()` translates `0x23` to `0x9E` so that `'#'` prints as expected rather than the British pound symbol `'£'`.

* `exit()` returns control to NASSYS 3 as recommended.

## Demos

* nasmacs - a small, simple, but powerful editor (EMACS inspired)

## Issues

* SDCC 3.4.0 doesn't add float support for `printf()` by default (otherwise floats are mostly supported).

* Why can't the linker figure out to place the data at the end of the text rather than having to give it explicitly?

For a kludgy workaround: http://sourceforge.net/p/sdcc/feature-requests/399/

* Similarily, but less important: my hack to put init at `0x1000` and `code-loc 0x1004` isn't very elegant.  Surely one could do better?

* Stack pointer is hardwire to start at `0xE000` (the end of ROM Basic); might be better if it could automatically find the end of RAM or otherwise be configurable.

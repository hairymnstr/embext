Embext
======

An open source EXT2 filesystem driver for small, bare-metal systems especially microcontrollers.
This library is licensed under a BSD license, you may use it under the terms found in COPYING.

Structure
---------

The filesystem driver itself is contained within the a single source file ``embext.c``.  This
relies upon a generic block driver which can provide a 512 byte block from the volume containing
the filesystem.  This driver may for example request a block from an SD card.  See ``block.h`` for 
the common function definitions that must be provided by the block driver.

There are two examples of block drivers in the ``src/block_driver`` folder, ``block_sd.c`` is an 
implementation of an SD card block driver designed to run an STM32F103 microcontroller using the 
[libopencm3](http://libopencm3.org) hardware library. ``block_pc.c`` is an implementation mainly
used for testing on a Linux host, it is designed to allow reading/writing from a filesystem
image in a file on the host.  The PC driver also contains some tools to snapshot and generate MD5
hashes for testing.

The library is designed to be called from a UNIX style C library for example 
[newlib](http://www.sourceware.org/newlib/) where there are POSIX compliant ``_open()`` and 
``_write()`` calls etc.  Since the implementation uses a structure pointer to represent an open
file it can be used in systems with multiple partitions, however it requires additional code to
wrap the open/read/write/close etc. calls and provide the appropriate filesystem context and
file object.

There is also a handler for MBR type primary partition tables in ``partition.c`` which can be used
in an embedded system to identify partitions within a volume.

History
-------

This is a fork of the EXT2 routines which were being developed in the Gristle FAT32/16 driver
structure.  However, changes to the block layer driver to improve maintainability mean that the
block drivers are no longer compatible.  In future I plan to re-integrate so that the drivers
are interchangeable.

Author
------

Gristle is written by Nathan Dumont <nathan@nathandumont.com>.

=====================
 OpenSlides Portable
=====================

Overview
========

Scripts to build a portable version of OpenSlides for Windows. Other
plattforms (Mac OS X, Linux) are planned for the future.


Requirements
============

- `OpenSlides <https://github.com/OpenSlides/OpenSlides/>`_ 2.x
- `OpenSlides-GUI <https://github.com/OpenSlides/openslides-gui/>`_


Build a Windows portable Version
================================

This is an instruction to create a portable Windows distribution of OpenSlides.

1. Install Python 3.7

   Follow the instructions in the README of OpenSlides.

2. Install OpenSlides 2.x package with all requirements::

    $ pip install openslides-2.x.tar.gz

   Unfortunately, twisted does not provide a binary wheel package for
   python 3.7. So it could be easier to download and install a prebuild
   whl package before you install OpenSlides, see
   https://www.lfd.uci.edu/~gohlke/pythonlibs/#twisted.

3. Navigate to OpenSlides 2.x git checkout and install the python requirements
   of OpenSlides for big mode::

    pip install -r requirements_big_mode.txt

4. Install OpenSlides-GUI package with all requirements::

    $ pip install openslides-gui

5. Install requirements for openslides-portable::

    $ pip install pywin32 attr

   Pywin32 is used to update the version resource of the prebuild openslides.exe.

6. Run::

    $ python make_portable.py


The portable OpenSlides distribution is created as a zip archive in the 'dist' directory.

NOTE: Creating the portable Windows distribution of OpenSlides is not possible if Python
is installed in 64-bit(!) version.


License
=======

This software is Free/Libre Open Source Software and distributed under the
MIT License, see LICENSE file.


Authors
=======

* Andy Kittner <andkit@gmx.net>
* Emanuel Schütze <emanuel@intevation.de>


Changelog
=========

Version 1.0 (2015-12-07)
------------------------
* First release of openslides-portable for OpenSlides 2.x.
  It's moved from old OpenSlides 1.x repository into an own openslides-portable package.

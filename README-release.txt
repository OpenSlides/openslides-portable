=====================
 OpenSlides Portable
=====================

This is a portable version of OpenSlides for Windows which does not
required any install steps. To start OpenSlides just run openslides.exe.

If you get an error message that a DLL file is missing you have to
install Microsoft Visual C++ 2015 Redistributable Package (x86) from:
https://www.microsoft.com/de-de/download/details.aspx?id=48145
Note that you have to install the 32bit version (vc_redist.x86.exe)
even if you use a 64bit Windows.

The OpenSlides data directory 'openslides' contains the sqlite
database file, the settings.py and the plugin directory.

If there is a reason that you can not use the portable version you can
install OpenSlides manually in a Python environment, see:
https://github.com/OpenSlides/OpenSlides


What is OpenSlides?
===================

OpenSlides is a free web-based presentation and assembly system for
displaying and controlling of agenda, motions and elections of an assembly.
See https://openslides.org for more information.


Used software
=============

See https://github.com/OpenSlides/OpenSlides for all packages used by
OpenSlides.


License and authors
===================

OpenSlides is Free/Libre Open Source Software (FLOSS), and distributed under
the MIT License, see LICENSE file. The authors of OpenSlides are mentioned
in the AUTHORS file, see https://github.com/OpenSlides/OpenSlides.

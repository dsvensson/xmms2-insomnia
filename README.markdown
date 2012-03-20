XMMS2 Insomnia
==============

A simple tool that tickles the Mac OS X activity sensor during playback
to prevent the system from going to sleep.

Building
--------

As simple as:

	$ make

This will default to search for XMMS2 libraries and headers in /opt/local.
If you've installed elsewhere you may override with XMMS2_PREFIX. If you want
to use another compiler than the system specific just set the CC-flag.

Example:

	$ make CC=clang XMMS2_PREFIX=/usr

Installing
----------

To install `xmms2-insomnia` in the current users XMMS2 autostart directory type:

	$ make install

To override use the XMMS2_HOME variable:

	$ make install XMMS2_HOME=/opt/local/share/xmms2/scripts/startup.d

If XMMS2 is already running you will need to restart XMMS2 for `xmms2-insomnia` to
start for the first time.

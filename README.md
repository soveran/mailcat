# mailcat

Fake SMTP server that prints emails to stdout.

Description
-----------

This program binds to port 25 by default (you may need to use sudo)
and streams to stdout all the incoming emails. If you want to run
it in a different port, pass the port number as the first argument.

Examples
--------

Print emails to stdout:

  	$ sudo mailcat

Tell `mailcat` to bind to port 2525 instead:

  	$ mailcat 2525

Run it in the background:

  	$ mailcat &

Installation
------------

Download the [latest release][releases] or check the available
[packages][packages]. You can also compile the source code in the
master branch.

[releases]: https://github.com/soveran/mailcat/releases
[packages]: https://github.com/soveran/mailcat/wiki/Distribution-Packages

Install mailcat into `/usr/local/bin` with the following command:

    $ make install

You can use `make PREFIX=/some/other/directory install` if you wish
to use a different destination. If you want to remove mailcat from
your system, use `make uninstall`.

Contributing
------------

If you find a bug, please create an issue detailing the ways to
reproduce it. If you have a suggestion, create an issue detailing
the use case.

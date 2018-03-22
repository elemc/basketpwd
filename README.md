[![Build Status](https://travis-ci.org/elemc/basketpwd.svg?branch=master)](https://travis-ci.org/elemc/basketpwd)

General information
===================
Basket of Passwords
The program for storage and information management about passwords.

You can download the source code from
	http://github.com/elemc/basketpwd/tarball/master
or
	http://github.com/elemc/basketpwd/zipball/master
or alternatively you can use GIT to get latest development version:
	git clone git://github.com/elemc/basketpwd.git

Dependencies
============
* Qt libraries version 4.2.1 or later
* libgcrypt version 1.4.0 or later (WARNING!!! It is required only for basketpwd version < 0.4.2)
* openssl version 0.9.7a or later (This required for basketpwd version >= 0.4.2)
* cmake

Installation
============

On GNU/Linux or FreeBSD
Run bellow commands in source folder
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=/usr #(if you have install application in /usr or simple 'cmake ..' if you have install into /usr/local)

On Mac OS X
$ mkdir build
# cd build
$ cmake .. 
$ make
$ make mac-bundle # for create only bundle
or
$ make mac-dmg    # for create bundle and dmg file

Installation for Fedora Linux
=============================
for Fedora 13 and later basketpwd packages in RFRemix repository:
    # http://koji.russianfedora.ru/koji/packageinfo?packageID=101

Installation for Slackware Linux
================================
$ git clone git://github.com/elemc/basketpwd.git		# clone repository from git
$ cd basketpwd							# enter to application directory
$ tools/make-source-archive					# make a tarboll for building
$ cp ../basketpwd*.tar.bz2 tools/SlackBuild/			# copy tarboll to SlackBuild directory
$ cd tools/SlackBuild
if version of tarboll not equal version in basketpwd.SlackBuild, then edit to equal.
$ su -								# enter in root account
# ./basketpwd.SlackBuild					# build a package
SlackBuild script tell where him create a package file, type
# installpkg /path/to/package/basketpwd*.tgz

Installation for OpenSUSE Linux
===============================
First, add repository:
    * for 11.2		http://download.opensuse.org/repositories/home:/elemc/openSUSE_11.2
    * for 11.3		http://download.opensuse.org/repositories/home:/elemc/openSUSE_11.3
    * for 11.4		http://download.opensuse.org/repositories/home:/elemc/openSUSE_11.4
    * for Facctory	http://download.opensuse.org/repositories/home:/elemc/openSUSE_Factory
 and install package named basketpwd, as example
    zypper in basketpwd
 
Installation for Windows
========================
Go to section Files on github.com/elemc/basketpwd and select one:
* basketpwd-static-X.X.X.zip - is a statically linked one exe-file for mobility
* basketpwd-setup-X.X.X.exe  - is a windows installer

Reporting bugs
==============

Please send bug reports to the <me AT elemc DOT name>
or post to issue in http://github.com/elemc/basketpwd/issues

Thanks

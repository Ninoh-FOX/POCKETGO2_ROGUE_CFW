#!/bin/sh

DIR=`dirname "$0"`
cd "$DIR"

rm -rf /tmp/modules_fs
mkdir /tmp/modules_fs
INSTALL_MOD_PATH=/tmp/modules_fs ARCH=mips make modules_install

NAME=`ls -d /tmp/modules_fs/lib/modules/*`
NAME=`basename "$NAME" +`

if [ -d  "/tmp/modules_fs/lib/modules/${NAME}" ] ; then
	ln -s "${NAME}" "/tmp/modules_fs/lib/modules/${NAME}+"
else
	ln -s "${NAME}+" "/tmp/modules_fs/lib/modules/${NAME}"
fi

mksquashfs /tmp/modules_fs/lib/modules modules.squashfs -all-root -noappend -no-exports -no-xattrs

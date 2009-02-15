#!/bin/sh

LRELEASE=""

if [ `which lrelease-qt4` ] ; then
	LRELEASE="`which lrelease-qt4`"
elif [ `which lrelease` ] ; then
	LRELEASE="`which lrelease`"
fi

if [ "$LRELEASE" = "" ] ; then
	echo "Error: lrelease / lrelease-qt4 not found."
	exit 1
fi

cd `dirname $0`
rm share/keepassx/i18n/*.qm
cd src
lrelease-qt4 src.pro
mv translations/*.qm ../share/keepassx/i18n
rm ../share/keepassx/i18n/keepassx-xx_XX.qm

#!/bin/sh

# This script updates ts files of qpiano, but does not touch ts files of music
# License: same with QPiano
# Author: Alexey Loginov <alexl@mageia.org>

if [ ! -z "`lupdate -version 2>/dev/null`" ]
then
  lupdate="lupdate"
fi

if [ ! -z "`lupdate-qt5 -version 2>/dev/null`" ]
then
  lupdate="lupdate-qt5"
fi

if [ -z "$lupdate" ]
then
  echo "lupdate was not found!"
  exit 1
fi

pushd ../
  $lupdate qpiano.pro -no-obsolete -ts translations/qpiano_blank.ts
  #$lupdate qpiano.pro -no-obsolete -ts translations/qpiano_*.ts
popd

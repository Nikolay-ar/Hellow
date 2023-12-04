#!/bin/bash

#
# Оrganization: GosNIIAS
# Year        : 2023
# Author      : Vorobev Alexey Vitalievich
# EMail       : vorobev_av@gosniias.ru
#

################################################################################

if false; then
    set -x
fi

set -e
set -u

################################################################################

BIN_DIRNAME="/usr/bin/dirname"
BIN_READLINK="/bin/readlink"

SCRIPT_PATH=$($BIN_DIRNAME $($BIN_READLINK -f ${0}))
cd $SCRIPT_PATH

################################################################################

BIN_FIND="/usr/bin/find"
BIN_SORT="/usr/bin/sort"

################################################################################

$BIN_FIND . -type f \
-not -path "./.git/*" \
-not -path "./[Bb]uild/*" \
-not -name "*.sublime-*" \
-not -name "*.creator.user" \
-not -name "*.cflags" \
-not -name "*.config" \
-not -name "*.cxxflags" \
-not -name "*.files" \
-not -name "*.includes" \
-not -name "*.raw" \
-not -name "*.bmp" \
-not -wholename "./Sources/3rdParty/qdbmp/license" \
-not -wholename "./Sources/3rdParty/qdbmp/readme" \
-not -wholename "./Sources/3rdParty/qdbmp/samples/*.c" | $BIN_SORT

################################################################################

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

BIN_ECHO="/bin/echo"
BIN_DATE="/bin/date"
BIN_MKDIR="/bin/mkdir -v"
BIN_CP="/bin/cp -v"
BIN_RM="/bin/rm -v"
BIN_TAR="/bin/tar -v"
BIN_BASENAME="/usr/bin/basename"
BIN_SED="/usr/bin/sed"

################################################################################

function Now () {
    $BIN_DATE +%Y-%m-%d_%H-%M-%S-%N
}

function PrintLine () {
    $BIN_ECHO "$(Now) --------------------------------------------------------------------------------"
}

################################################################################

PROJECT_NAME=$($BIN_BASENAME ${0} | $BIN_SED 's/\.pack.sh//')

################################################################################

SESSION_ID="$(Now)"
ARCHIVE_NAME_DIRECTORY="../"${PROJECT_NAME}"_"${SESSION_ID}
ARCHIVE_NAME_FILE=${ARCHIVE_NAME_DIRECTORY}".tar.bz2"

################################################################################

PrintLine
$BIN_ECHO "$(Now) [CreateTargetDirectory] [Begin]"
$BIN_MKDIR -p $ARCHIVE_NAME_DIRECTORY
$BIN_ECHO "$(Now) [CreateTargetDirectory] [End]"
PrintLine

################################################################################

PrintLine
$BIN_ECHO "$(Now) [Copy] [Begin]"

set +e

$BIN_CP -pr "./Includes" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -pr "./Sources" $ARCHIVE_NAME_DIRECTORY"/"

$BIN_CP -p "./.gitignore" $ARCHIVE_NAME_DIRECTORY"/"

$BIN_CP -p "./libGigEVision.build.conf.example" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.build.sh" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.cflags" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.config.example" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.creator" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.cxxflags" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.files" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.files.sh" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.includes" $ARCHIVE_NAME_DIRECTORY"/"
$BIN_CP -p "./libGigEVision.pack.sh" $ARCHIVE_NAME_DIRECTORY"/"

set -e

$BIN_ECHO "$(Now) [Copy] [End]"
PrintLine

################################################################################

PrintLine
$BIN_ECHO "$(Now) [Archive] [Begin]"
$BIN_TAR -p -C "$SCRIPT_PATH" -cf "$ARCHIVE_NAME_FILE" -j "$ARCHIVE_NAME_DIRECTORY"
$BIN_ECHO "$(Now) [Archive] [End]"
PrintLine

################################################################################

PrintLine
$BIN_ECHO "$(Now) $0 [Clean] [Begin]"
$BIN_RM -rf "$ARCHIVE_NAME_DIRECTORY"
$BIN_ECHO "$(Now) $0 [Clean] [End]"
PrintLine

################################################################################

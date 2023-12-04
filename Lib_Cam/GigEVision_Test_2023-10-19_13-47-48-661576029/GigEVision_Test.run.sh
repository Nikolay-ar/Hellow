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

BIN_ECHO="/usr/bin/echo"
BIN_DATE="/bin/date"
BIN_BASENAME="/usr/bin/basename"
BIN_SED="/usr/bin/sed"
BIN_LDD="/usr/bin/ldd"
BIN_RM="/usr/bin/rm -fv"

################################################################################

function Now () {
    $BIN_DATE +%Y-%m-%d_%H-%M-%S-%N
}

function PrintLine () {
    $BIN_ECHO "$(Now) --------------------------------------------------------------------------------"
}

################################################################################

PROJECT_NAME=$($BIN_BASENAME ${0} | $BIN_SED 's/\.run.sh//')

################################################################################

set +u

if [ -n "$1" ]; then
    CONFIGURATION_FILE_BUILD="$1"
else
    CONFIGURATION_FILE_BUILD="./${PROJECT_NAME}.build.conf"
fi

set -u

source "$CONFIGURATION_FILE_BUILD"

################################################################################

set +u

if [ -n "$2" ]; then
    CONFIGURATION_FILE_RUN="$2"
else
    CONFIGURATION_FILE_RUN="./${PROJECT_NAME}.run.conf"
fi

set -u

source "$CONFIGURATION_FILE_RUN"

################################################################################

if [ ! -z "${LIBRARIES_PATHS}" ]; then
    LIBRARIES_PATHS=$($BIN_ECHO ${LIBRARIES_PATHS} | $BIN_SED 's/ *$//' | $BIN_SED 's/ /:/g')
fi

################################################################################

set +u

if [ ! -z "${LD_LIBRARY_PATH}" ]; then
    LD_LIBRARY_PATH=${LIBRARIES_PATHS}${LD_LIBRARY_PATH}
else
    LD_LIBRARY_PATH=${LIBRARIES_PATHS}
fi

set -u

################################################################################

BUILD_DIRECTORY=${DIRECTORY_BUILD}

################################################################################

case "$TARGET_PLATFORM" in
    Windows)
        EXECUTABLE="${BUILD_DIRECTORY}/${TARGET_PLATFORM}/${TARGET_ARCHITECTURE}/${BUILD_TYPE}/${PROJECT_NAME}.exe"
        PATH=$PATH":"$LD_LIBRARY_PATH
        ;;
    *)
        EXECUTABLE="${BUILD_DIRECTORY}/${TARGET_PLATFORM}/${TARGET_ARCHITECTURE}/${BUILD_TYPE}/${PROJECT_NAME}"
        PATH=$PATH
        ;;
esac

################################################################################

PrintLine
$BIN_ECHO "$(Now) [ConfigurationData] "'$PROJECT_NAME'"            : $PROJECT_NAME"
$BIN_ECHO "$(Now) [ConfigurationData] "'$CONFIGURATION_FILE_BUILD'": $CONFIGURATION_FILE_BUILD"
$BIN_ECHO "$(Now) [ConfigurationData] "'$CONFIGURATION_FILE_RUN'"  : $CONFIGURATION_FILE_RUN"
$BIN_ECHO "$(Now) [ConfigurationData] "'$TARGET_PLATFORM'"         : $TARGET_PLATFORM"
$BIN_ECHO "$(Now) [ConfigurationData] "'$TARGET_ARCHITECTURE'"     : $TARGET_ARCHITECTURE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BUILD_TYPE'"              : $BUILD_TYPE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BUILD_DIRECTORY'"         : $BUILD_DIRECTORY"
$BIN_ECHO "$(Now) [ConfigurationData] "'$LD_LIBRARY_PATH'"         : $LD_LIBRARY_PATH"
$BIN_ECHO "$(Now) [ConfigurationData] "'$EXECUTABLE'"              : $EXECUTABLE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$PATH'"                    : $PATH"
$BIN_ECHO "$(Now) [ConfigurationData] "'$PYTHON_CONVERTER'"        : $PYTHON_CONVERTER"
PrintLine
$BIN_ECHO "$(Now) [RunData] "'$OPTION_DEVICE'"              : $OPTION_DEVICE"
$BIN_ECHO "$(Now) [RunData] "'$OPTION_SOURCE'"              : $OPTION_SOURCE"
$BIN_ECHO "$(Now) [RunData] "'$OPTION_DESTINATION'"         : $OPTION_DESTINATION"
$BIN_ECHO "$(Now) [RunData] "'$OPTION_USE_PYTHON_CONVERTER'": $OPTION_USE_PYTHON_CONVERTER"
PrintLine

################################################################################

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
export PATH=${PATH}

################################################################################

$BIN_LDD $EXECUTABLE
PrintLine

################################################################################

if [ "${OPTION_DEVICE}" == "Crevis MG-A320K-35" ]; then

    $BIN_RM "./Data/GigEVision_Test.interpolated-c.bmp"
    $BIN_RM "./Data/GigEVision_Test.interpolated-py.bmp"
    $BIN_RM "./Data/GigEVision_Test.rg8"
    $BIN_RM "./Data/GigEVision_Test.rg8.bmp"

    PrintLine

fi

################################################################################

#
# Socket receive buffer: cat /proc/sys/net/core/rmem_max
# Socket send buffer   : cat /proc/sys/net/core/wmem_max
#
# sudo sysctl -w net.core.rmem_max=67108864
# sudo sysctl -w net.core.wmem_max=67108864
#

################################################################################

$EXECUTABLE \
--device "${OPTION_DEVICE}" \
--source ${OPTION_SOURCE} \
--destination ${OPTION_DESTINATION}

################################################################################

if [ "${OPTION_DEVICE}" == "Crevis MG-A320K-35" ]; then
    if [ "${OPTION_USE_PYTHON_CONVERTER}" == true ]; then
        $PYTHON_CONVERTER
    fi
fi

################################################################################

PrintLine

################################################################################

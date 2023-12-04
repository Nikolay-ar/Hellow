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
BIN_CAT="/usr/bin/cat"
BIN_RM="/bin/rm -v"
BIN_MKDIR="/bin/mkdir -v"
BIN_FIND="/usr/bin/find"
BIN_BASENAME="/usr/bin/basename"
BIN_SED="/usr/bin/sed"
BIN_TR="/usr/bin/tr"
BIN_GREP="/usr/bin/grep"

################################################################################

function Now () {
    $BIN_DATE +%Y-%m-%d_%H-%M-%S-%N
}

function PrintLine () {
    $BIN_ECHO "$(Now) --------------------------------------------------------------------------------"
}

################################################################################

PROJECT_NAME=$($BIN_BASENAME ${0} | $BIN_SED 's/\.build.sh//')

################################################################################

set +u

if [ -n "$1" ]; then
    CONFIGURATION_FILE="$1"
else
    CONFIGURATION_FILE="./${PROJECT_NAME}.build.conf"
fi

set -u

source "$CONFIGURATION_FILE"

################################################################################

CFLAGS=$($BIN_CAT "./${PROJECT_NAME}.cflags")

################################################################################

case "$BUILD_TYPE" in
    Release)
        CFLAGS="${CFLAGS} -O${OPTIMIZATION_LEVEL}"
        ;;
    Debug)
        CFLAGS="${CFLAGS} -O${OPTIMIZATION_LEVEL} -g"
        ;;
    *)
        ;;
esac

################################################################################

DEFINES=$($BIN_CAT "./${PROJECT_NAME}.config" | $BIN_GREP -v '//' | $BIN_TR '\n' ' ' | $BIN_SED 's/#define /-D/g' | $BIN_SED 's/ *$//')

DEFINES="${DEFINES} -D${PROJECT_NAME^^}_PLATFORM_${TARGET_PLATFORM^^} -D${PROJECT_NAME^^}_ARCHITECTURE_${TARGET_ARCHITECTURE^^}"

################################################################################

INCLUDES=$($BIN_CAT "./${PROJECT_NAME}.includes" | $BIN_GREP -v '//' | $BIN_TR '\n' ' ' | $BIN_SED 's/ *$//' | $BIN_SED 's/ / -I/g')
INCLUDES="-I"${INCLUDES}

################################################################################

if [ ! -z "${LIBRARIES_PATHS}" ]; then
    LIBRARIES_PATHS=$($BIN_ECHO ${LIBRARIES_PATHS} | $BIN_SED 's/ *$//' | $BIN_SED 's/ / -L/g')
    LIBRARIES_PATHS="-L"${LIBRARIES_PATHS}
fi

################################################################################

if [ ! -z "${LIBRARIES_DYNAMIC}" ]; then
    case "$TARGET_PLATFORM" in
        Windows)
            LIBRARIES_DYNAMIC=$($BIN_ECHO ${LIBRARIES_DYNAMIC} | $BIN_SED 's/ *$//' | $BIN_SED 's/ /.dll -l:/g')
            LIBRARIES_DYNAMIC="-l:"${LIBRARIES_DYNAMIC}".dll"
            ;;
        *)
            LIBRARIES_DYNAMIC=$($BIN_ECHO ${LIBRARIES_DYNAMIC} | $BIN_SED 's/ *$//' | $BIN_SED 's/ / -l/g')
            LIBRARIES_DYNAMIC="-l"${LIBRARIES_DYNAMIC}
            ;;
    esac
fi

################################################################################

if [ ! -z "${LIBRARIES_STATIC}" ]; then
    case "$TARGET_PLATFORM" in
        Windows)
            LIBRARIES_STATIC=$($BIN_ECHO ${LIBRARIES_STATIC} | $BIN_SED 's/ *$//' | $BIN_SED 's/ /.lib -l:/g')
            LIBRARIES_STATIC="-l:"${LIBRARIES_STATIC}".lib"
            ;;
        *)
            LIBRARIES_STATIC=$($BIN_ECHO ${LIBRARIES_STATIC} | $BIN_SED 's/ *$//' | $BIN_SED 's/ /.a -l:lib/g')
            LIBRARIES_STATIC="-l:lib"${LIBRARIES_STATIC}".a"
            ;;
    esac
fi

################################################################################

PrintLine
$BIN_ECHO "$(Now) [ConfigurationData] "'$PROJECT_NAME'"                 : $PROJECT_NAME"
$BIN_ECHO "$(Now) [ConfigurationData] "'$CONFIGURATION_FILE'"           : $CONFIGURATION_FILE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$TARGET_PLATFORM'"              : $TARGET_PLATFORM"
$BIN_ECHO "$(Now) [ConfigurationData] "'$TARGET_ARCHITECTURE'"          : $TARGET_ARCHITECTURE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BUILD_TYPE'"                   : $BUILD_TYPE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BUILD_TRACE'"                  : $BUILD_TRACE"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BUILD_SAVE_INTERMEDIATE_FILES'": $BUILD_SAVE_INTERMEDIATE_FILES"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BUILD_PRINT_COMMANDS'"         : $BUILD_PRINT_COMMANDS"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BIN_COMPILER'"                 : $BIN_COMPILER"
$BIN_ECHO "$(Now) [ConfigurationData] "'$BIN_STRIPPER'"                 : $BIN_STRIPPER"
$BIN_ECHO "$(Now) [ConfigurationData] "'$DIRECTORY_SOURCES'"            : $DIRECTORY_SOURCES"
$BIN_ECHO "$(Now) [ConfigurationData] "'$DIRECTORY_BUILD'"              : $DIRECTORY_BUILD"
$BIN_ECHO "$(Now) [ConfigurationData] "'$CFLAGS'"                       : $CFLAGS"
$BIN_ECHO "$(Now) [ConfigurationData] "'$DEFINES'"                      : $DEFINES"
$BIN_ECHO "$(Now) [ConfigurationData] "'$INCLUDES'"                     : $INCLUDES"
$BIN_ECHO "$(Now) [ConfigurationData] "'$LIBRARIES_PATHS'"              : $LIBRARIES_PATHS"
$BIN_ECHO "$(Now) [ConfigurationData] "'$LIBRARIES_DYNAMIC'"            : $LIBRARIES_DYNAMIC"
$BIN_ECHO "$(Now) [ConfigurationData] "'$LIBRARIES_STATIC'"             : $LIBRARIES_STATIC"

if [ "$BUILD_TRACE" == false ]; then
    PrintLine
fi

################################################################################

DIRECTORY_TARGET="${DIRECTORY_BUILD}/${TARGET_PLATFORM}/${TARGET_ARCHITECTURE}/${BUILD_TYPE}"

################################################################################

if [ "$BUILD_TRACE" == true ]; then
    PrintLine
    $BIN_ECHO "$(Now) [ConfigurationProcessing] "'$DIRECTORY_TARGET'": $DIRECTORY_TARGET"
fi

################################################################################

if [ "$BUILD_TRACE" == true ]; then
    PrintLine
    $BIN_ECHO "$(Now) [Clean] [Begin]"
fi

if [ -d "${DIRECTORY_TARGET}" ]; then
    set +e
    $BIN_RM -rf "${DIRECTORY_TARGET}"
    set -e
else
    if [ "$BUILD_TRACE" == true ]; then
        $BIN_ECHO "$(Now) [Clean] "'$DIRECTORY_TARGET'": $DIRECTORY_TARGET"
        $BIN_ECHO "$(Now) [Clean] "'$DIRECTORY_TARGET'" does not exist, cleaning skipped"
    fi
fi

if [ "$BUILD_TRACE" == true ]; then
    $BIN_ECHO "$(Now) [Clean] [End]"
fi

################################################################################

if [ "$BUILD_TRACE" == true ]; then
    PrintLine
    $BIN_ECHO "$(Now) [CreateTargetDirectory] [Begin]"
fi

$BIN_MKDIR -p ${DIRECTORY_TARGET}

if [ "$BUILD_TRACE" == true ]; then
    $BIN_ECHO "$(Now) [CreateTargetDirectory] [End]"
fi

################################################################################

FILES_SOURCES=$($BIN_FIND ${DIRECTORY_SOURCES} -type f -not -wholename "./Sources/3rdParty/qdbmp/samples/*.c" -name "*.c")

################################################################################

PrintLine

for FILE_SOURCE in ${FILES_SOURCES}
do

    if [ "$BUILD_SAVE_INTERMEDIATE_FILES" == true ]; then

        FILE_BASENAME=$($BIN_BASENAME ${FILE_SOURCE})
        FILE_PREPROCESSED=$($BIN_ECHO ${FILE_BASENAME} | $BIN_SED 's/\.c//')".i"
        FILE_ASSEMBLER=$($BIN_ECHO ${FILE_BASENAME} | $BIN_SED 's/\.c//')".s"
        FILE_OBJECT=$($BIN_ECHO ${FILE_BASENAME} | $BIN_SED 's/\.c//')".o"

        if [ "$BUILD_TRACE" == true ]; then
            $BIN_ECHO "$(Now) [Build] "'$FILE_SOURCE'"      : $FILE_SOURCE"
            $BIN_ECHO "$(Now) [Build] "'$FILE_BASENAME'"    : $FILE_BASENAME"
            $BIN_ECHO "$(Now) [Build] "'$FILE_PREPROCESSED'": $FILE_PREPROCESSED"
            $BIN_ECHO "$(Now) [Build] "'$FILE_ASSEMBLER'"   : $FILE_ASSEMBLER"
            $BIN_ECHO "$(Now) [Build] "'$FILE_OBJECT'"      : $FILE_OBJECT"
        fi

        COMMAND_PREPROCESS="$BIN_COMPILER -E $CFLAGS $DEFINES $INCLUDES -o ${DIRECTORY_TARGET}/${FILE_PREPROCESSED} ${FILE_SOURCE}"
        COMMAND_COMPILE="$BIN_COMPILER -S $CFLAGS $DEFINES $INCLUDES -o ${DIRECTORY_TARGET}/${FILE_ASSEMBLER} ${DIRECTORY_TARGET}/${FILE_PREPROCESSED}"
        COMMAND_ASSEMBLE="$BIN_COMPILER -c $CFLAGS $DEFINES $INCLUDES -o ${DIRECTORY_TARGET}/${FILE_OBJECT} ${DIRECTORY_TARGET}/${FILE_ASSEMBLER}"

        if [ "$BUILD_PRINT_COMMANDS" == true ]; then
            $BIN_ECHO "$(Now) [Build] [Preprocess] $COMMAND_PREPROCESS"
        fi

        $COMMAND_PREPROCESS

        if [ "$BUILD_PRINT_COMMANDS" == true ]; then
            $BIN_ECHO "$(Now) [Build] [Compile] $COMMAND_COMPILE"
        fi

        $COMMAND_COMPILE

        if [ "$BUILD_PRINT_COMMANDS" == true ]; then
            $BIN_ECHO "$(Now) [Build] [Assemble] $COMMAND_ASSEMBLE"
        fi

        $COMMAND_ASSEMBLE

    else

        FILE_BASENAME=$($BIN_BASENAME ${FILE_SOURCE})
        FILE_OBJECT=$($BIN_ECHO ${FILE_BASENAME} | $BIN_SED 's/\.c//')".o"

        if [ "$BUILD_TRACE" == true ]; then
            $BIN_ECHO "$(Now) [Build] "'$FILE_SOURCE'"      : $FILE_SOURCE"
            $BIN_ECHO "$(Now) [Build] "'$FILE_BASENAME'"    : $FILE_BASENAME"
            $BIN_ECHO "$(Now) [Build] "'$FILE_OBJECT'"      : $FILE_OBJECT"
        fi

        COMMAND_BUILD="$BIN_COMPILER -c ${CFLAGS} ${DEFINES} ${INCLUDES} -o ${DIRECTORY_TARGET}/${FILE_OBJECT} ${FILE_SOURCE}"

        if [ "$BUILD_PRINT_COMMANDS" == true ]; then
            $BIN_ECHO "$(Now) [Build] [Preprocess, Compile, Assemble] $COMMAND_BUILD"
        fi

        $COMMAND_BUILD

    fi

done

################################################################################

FILES_OBJECTS=$($BIN_FIND ${DIRECTORY_TARGET} -type f -name "*.o")
FILES_OBJECTS=$($BIN_ECHO ${FILES_OBJECTS} | $BIN_TR '\n' ' ')
FILES_OBJECTS=$($BIN_ECHO ${FILES_OBJECTS} | $BIN_SED 's/ *$//')

################################################################################

if [ "$BUILD_TRACE" == true ]; then
    PrintLine
    $BIN_ECHO "$(Now) [Build] "'$FILES_OBJECTS'": $FILES_OBJECTS"
fi

################################################################################

case "$TARGET_PLATFORM" in
    Windows)
        TARGET_NAME_EXECUTABLE="${PROJECT_NAME}.exe"
        ;;
    *)
        TARGET_NAME_EXECUTABLE="${PROJECT_NAME}"
        ;;
esac

################################################################################

COMMAND_LINK="$BIN_COMPILER ${CFLAGS} ${DEFINES} ${INCLUDES} ${LIBRARIES_PATHS} ${LIBRARIES_DYNAMIC} -o ${DIRECTORY_TARGET}/${TARGET_NAME_EXECUTABLE} ${FILES_OBJECTS} ${LIBRARIES_STATIC}"

if [ "$BUILD_PRINT_COMMANDS" == true ]; then
    PrintLine
    $BIN_ECHO "$(Now) [Build] [Link] $COMMAND_LINK"
fi

$COMMAND_LINK

################################################################################

case "$BUILD_TYPE" in
    Release)

        COMMAND_STRIP_EXECUTABLE="$BIN_STRIPPER -s ${DIRECTORY_TARGET}/${TARGET_NAME_EXECUTABLE}"

        if [ "$BUILD_PRINT_COMMANDS" == true ]; then
            PrintLine
            $BIN_ECHO "$(Now) [Build] [StripExecutable] $COMMAND_STRIP_EXECUTABLE"
        fi

        $COMMAND_STRIP_EXECUTABLE

        ;;
    *)
        ;;
esac

################################################################################

PrintLine

################################################################################

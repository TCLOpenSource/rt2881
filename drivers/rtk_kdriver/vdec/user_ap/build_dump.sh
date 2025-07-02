#!/bin/bash

# New additions here
Chips=(rtd2885p rtd2851f rtd6748)

# Default vars
BIN_NAME="rtk_vdec_dump"
BIN_DIR="./bin"
BIN_PATH=$BIN_DIR/$BIN_NAME
DUMP_DIR="./dump"
DUMP_MAKEFILE="Makefile.dump"
DUMP_MAKEFILE_PATH=$DUMP_DIR/$DUMP_MAKEFILE
BoolTrue="1"
BoolFalse="0"

# Show the list of all support chip name
show_chip_names()
{
	echo "support chip_arg: ${Chips[@]}"
}

# Show the example of cross prefix
show_cross_prefix()
{
	echo "need to set cross_prefix to compile source"
	echo "ex: -cross /cross/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-"
}

# Show usage description
usage()
{
	echo "usage: $0 -chip <chip_arg> -cross <cross_prefix>"
	show_chip_names
	show_cross_prefix
}

# Parse cmd args
while [ "`echo $1 | cut -c1`" = "-" ]
do
    case "$1" in
        -chip)
                Chip=$2
                shift 2
            ;;
        -cross)
                Cross=$2
                shift 2
            ;;
        *)
                usage
                exit
            ;;
	esac
done

# Validate args
if [ -z $Chip ] || [ -z $Cross ]; then
    echo "chip or cross empty, requires arguments"
    usage
    exit
fi

isChipValid=$BoolFalse
for currChip in ${Chips[@]}
do
	if [ $currChip == $Chip ]; then
		isChipValid=$BoolTrue
		break
	fi
done
if [ $isChipValid == $BoolFalse ]; then
	echo "invalid chip_arg"
	show_chip_names
	exit
fi

if [ -d $(dirname $Cross) ]; then
	if [ -f $(dirname $Cross)/$(basename $Cross)g++ ]; then
		export Chip
		export Cross
		export BIN_NAME
		export BIN_PATH
	else
		echo "cross compiler not exist"
		show_cross_prefix
		exit
	fi
else
	echo "invalid cross_prefix directory"
	show_cross_prefix
	exit
fi

if [ -d $DUMP_DIR ]; then
	if [ -f $DUMP_MAKEFILE_PATH ]; then
		export DUMP_DIR
		make -f $DUMP_MAKEFILE_PATH clean
		make -f $DUMP_MAKEFILE_PATH
	else
		echo "invalid dump makefile \"$DUMP_MAKEFILE_PATH\""
		exit
	fi
else
	echo "invalid dump directory \"$DUMP_DIR\""
	exit
fi

if [ -f $BIN_PATH ]; then
	echo ""
	echo "build successfully"
	echo "output in \"$BIN_PATH\""
else
	echo "no binary output, \"$BIN_PATH\" build failed"
	exit
fi
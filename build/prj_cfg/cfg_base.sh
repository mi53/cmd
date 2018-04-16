#!/bin/bash



#需要配置的项目。
SRC_INC=""
SRC_DIR="$BUILD_ROOT/src/cmd_base"
DEST_DIR="$BUILD_ROOT/bin/cmd_base"
DEST_BIN="cmd_base.a"
DYN_LIBS="-lpthread"
STAT_LIBS=""
TMP_DIR="$BUILD_ROOT/bin/tmp/cmd_base"
BUILD_TYPE="STAT_LIB"

#需要重置的配置。

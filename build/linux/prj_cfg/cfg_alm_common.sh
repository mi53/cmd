#!/bin/bash



#需要配置的项目。
SRC_INC="$BUILD_ROOT/src/cmd_base"
SRC_DIR="$BUILD_ROOT/src/alm_common"
DEST_DIR="$BUILD_ROOT/bin/alm_common"
DEST_BIN="alm_common.a"
DYN_LIBS="-lpthread"
STAT_LIBS=""
TMP_DIR="$BUILD_ROOT/bin/tmp/alm_common"
BUILD_TYPE="STAT_LIB"

#需要重置的配置。

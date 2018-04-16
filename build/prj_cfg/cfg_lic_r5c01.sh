#!/bin/bash

echo 信息: 开始配置lic_r5c01工程。

#需要配置的项目。
SRC_INC="$BUILD_ROOT/import/r5c01/include"
SRC_DIR="$BUILD_ROOT/import/r5c01/src"
DEST_DIR="$BUILD_ROOT/lib/r5c01"
DEST_BIN="lic_r5c01.a"
DYN_LIBS=""
STAT_LIBS=""
TMP_DIR="$BUILD_ROOT/bin/tmp/lic_r5c01"
BUILD_TYPE="STAT_LIB"

#需要重置的配置。
BUILD_STAT_LIB_FLAGS="$BUILD_STAT_LIB_FLAGS -D__LINUX__ -DLIC_INC_DAT"

#!/bin/bash

echo 信息: 开始配置lic_r2c05工程。

#需要配置的项目。
SRC_INC="$BUILD_ROOT/import/r2c05/include"
SRC_DIR="$BUILD_ROOT/import/r2c05/src"
DEST_DIR="$BUILD_ROOT/lib/r2c05"
DEST_BIN="lic_r2c05.a"
DYN_LIBS=""
STAT_LIBS=""
TMP_DIR="$BUILD_ROOT/bin/tmp/lic_r2c05"
BUILD_TYPE="STAT_LIB"

#需要重置的配置。
BUILD_STAT_LIB_FLAGS="$BUILD_STAT_LIB_FLAGS -D__LINUX__ -DLIC_INC_DAT"

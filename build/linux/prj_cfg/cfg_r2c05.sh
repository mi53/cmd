#!/bin/bash

echo 信息: 开始配置test_r2c05工程。

#需要配置的项目。
SRC_DIR="$BUILD_ROOT/src/test_r2c05"
SRC_INC="$BUILD_ROOT/import/r2c05/include $BUILD_ROOT/src/cmd_base $BUILD_ROOT/src/alm_common"
DEST_DIR="$BUILD_ROOT/bin/test_r2c05"
DEST_BIN="run.out"
DYN_LIBS="-lpthread"
STAT_LIBS="$BUILD_ROOT/bin/alm_common/alm_common.a $BUILD_ROOT/bin/cmd_base/cmd_base.a $BUILD_ROOT/lib/r2c05/lic_r2c05.a"
TMP_DIR="$BUILD_ROOT/bin/tmp/test_r2c05"
BUILD_TYPE="EXE"

#需要重置的配置。

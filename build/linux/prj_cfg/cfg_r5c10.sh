#!/bin/bash

echo 信息: 开始配置test_r5c10工程。

#需要配置的项目。
SRC_DIR="$BUILD_ROOT/src/test_r5c10"
SRC_INC="$BUILD_ROOT/import/r5c10/include $BUILD_ROOT/src/cmd_base $BUILD_ROOT/src/alm_common"
DEST_DIR="$BUILD_ROOT/bin/test_r5c10"
DEST_BIN="run.out"
DYN_LIBS="-lpthread"
STAT_LIBS="$BUILD_ROOT/bin/alm_common/alm_common.a $BUILD_ROOT/bin/cmd_base/cmd_base.a $BUILD_ROOT/lib/r5c10/lic_r5c10.a"
TMP_DIR="$BUILD_ROOT/bin/tmp/test_r5c10"
BUILD_TYPE="EXE"

#需要重置的配置。

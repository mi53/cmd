#!/bin/bash

#更新公共配置。
BUILDER="gcc"
BUILD_ROOT="/home/xx/support"
BUILD_FLAGS="-m64 -O3 -DALM_BYTE_ORDER=0 -D_LIC_RELEASE -DLINUX -DALM_CPU_X86"
BUILD_STAT_LIB_FLAGS="$BUILD_FLAGS"
BUILD_EXE_FLAGS="$BUILD_FLAGS"
BUILD_DYN_LIB_FLAGS="-shared -fPIC $BUILD_FLAGS"
TMP_AR="_tmp_stat_lib_.a"
BUILD_SUB_PRJ_DIR=$BUILD_ROOT/build/linux/prj_cfg

if test ! -x "$BUILD_ROOT" ; then
echo 错误: 目录"$BUILD_ROOT"不存在。
exit
fi

if test ! -x "$BUILD_SUB_PRJ_DIR" ; then
echo 错误: 目录"$BUILD_SUB_PRJ_DIR"不存在。
#exit
fi
chmod -R 744 "$BUILD_SUB_PRJ_DIR" 

#更新项目配置。
if [ "$1" == "r2c05" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_r2c05.sh
elif [ "$1" == "r5c10" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_r5c10.sh
elif [ "$1" == "r5c01" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_r5c01.sh
elif [ "$1" == "r5c00" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_r5c00.sh
elif [ "$1" == "base" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_base.sh
elif [ "$1" == "alm_common" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_alm_common.sh
elif [ "$1" == "lic_r2c05" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_lic_r2c05.sh
elif [ "$1" == "lic_r5c10" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_lic_r5c10.sh
elif [ "$1" == "lic_r5c01" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_lic_r5c01.sh
elif [ "$1" == "lic_r5c00" ]; then
. $BUILD_SUB_PRJ_DIR/cfg_lic_r5c00.sh
else
echo 错误: 参数只能为base/alm_common/lic_r5c10/r5c10/lic_r2c05/r2c05/lic_r5c00/r5c00/lic_r5c01/r5c01。
exit
fi

#初始化临时变量。
DIR_INC=""
CODE_FILES_TMP=""
TMP_OBJS=""
TMP_BUILD="$TMP_DIR/_1_tmp_lib_build_1_.sh"
BUILD_FLAGS=""

#删除目标
rm -f -R "$DEST_DIR/$DEST_BIN"

#代码换行转换成unix风格。
#find "$SRC_DIR" -type f | xargs dos2unix -o

#得到全部路径
PRJ_INC()
{
	if [ "$1" != "" ]; then
	DIR_INC_TMP="-I \"$1\""
	
	#for var in `ls -R "$1" | grep : |awk -F : '{print $1}'`
	for para in $1; do
		for var in `find "$para"`; do
			if test -x "$var"
			then
				DIR_INC_TMP="$DIR_INC_TMP -I \"$var\""
			fi
		done
	done
	
	DIR_INC="$DIR_INC $DIR_INC_TMP"
	fi
}

#编译生成.o及打包成临时静态库。
PRJ_LIB()
{
	if [ "$1" != "" ]; then
	
	TMP_OBJS=`find "$1" | grep "\.c" | grep -v "\.svn"`
	OBJS=""
	CFILES=""
	
	for var in $TMP_OBJS
	do
		OBJS="$OBJS `echo $var | awk -F / '{print $NF}'| awk -F . '{print $1}'`.o"
		CFILES="$CFILES \"$var\""
	done
	
	DIR_SV=`pwd`
	echo cd $TMP_DIR >> "$TMP_BUILD"
	echo $BUILDER $BUILD_FLAGS $DIR_INC -c $CFILES >> "$TMP_BUILD"
	echo ar rcs "$TMP_DIR/$TMP_AR" "$OBJS" >> "$TMP_BUILD"
	echo cd $DIR_SV >> "$TMP_BUILD"
	
	fi
}

#创建临时编译脚本
#if test -d "$TMP_DIR"; then 
#rm -R "$TMP_DIR" 
#fi
mkdir -p "$TMP_DIR"
mkdir -p "$DEST_DIR"
echo \#!/bin/bash > "$TMP_BUILD"
echo rm -f "$DEST_DIR/$DEST_BIN" >> "$TMP_BUILD"

PRJ_INC "$SRC_INC"
PRJ_INC "$SRC_DIR"

#编译生成静态库。
if [ "$BUILD_TYPE" == "STAT_LIB" ];then
BUILD_FLAGS=$BUILD_STAT_LIB_FLAGS
PRJ_LIB "$SRC_DIR"
echo 信息: 编译静态库。
echo cp "$TMP_DIR/$TMP_AR" "$DEST_DIR/$DEST_BIN" >> "$TMP_BUILD"

#编译生成可执行文件。
elif [ "$BUILD_TYPE" == "EXE" ];then
BUILD_FLAGS=$BUILD_EXE_FLAGS
PRJ_LIB "$SRC_DIR"
echo 信息: 编译可执行文件。
echo $BUILDER $BUILD_FLAGS $DIR_INC "$TMP_DIR/$TMP_AR" $STAT_LIBS $DYN_LIBS -o "$DEST_DIR/$DEST_BIN" >> "$TMP_BUILD"

#编译生成动态库。
elif [ "$BUILD_TYPE" == "DYN_LIB" ];then
BUILD_FLAGS=$BUILD_DYN_LIB_FLAGS
PRJ_LIB "$SRC_DIR"
echo 信息: 编译动态库。
echo $BUILDER $BUILD_FLAGS $DIR_INC "$TMP_DIR/$TMP_AR" $STAT_LIBS $DYN_LIBS -o "$DEST_DIR/$DEST_BIN" >> "$TMP_BUILD"

#错误
else
echo 错误: 编译目标不是STAT_LIB/EXE/DYN_LIB中的某一个。
exit
fi

echo 信息: 生成"$DEST_DIR/$DEST_BIN"成功。

#执行临时编译脚本，执行完成后删除。
chmod 777 "$TMP_BUILD"
"$TMP_BUILD"

#编译完成，删除临时文件。
if test -d "$TMP_DIR"; then 
rm -R "$TMP_DIR" 
fi

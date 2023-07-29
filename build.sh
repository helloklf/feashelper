OUT="./out"
BUILD_DIR="./build/cmake"
VERSION="v1.3.1"

rm -r ./target > /dev/null 2>&1

cmake_build()
{
	local TARGET=$1
	local METHOD=$2
	local ABI=$3

	if [[ $METHOD == "Ninja" ]]; then
		local BUILD_METHOD="-G Ninja"
		local MAKE_CMD="time -p ninja -C $OUT"
	elif [[ $METHOD == "make" ]]; then
		local MAKE_CMD="time -p make -C $OUT -j$(nproc)"
	fi;

	if [[ $TARGET == "Android" ]]; then
		local ANDROID_PLATFORM=$4
		cmake -S ${BUILD_DIR} -B $OUT ${BUILD_METHOD} \
			-DNDK_CCACHE="ccache" \
			-DCMAKE_BUILD_TYPE="Release" \
			-DANDROID_PLATFORM="$ANDROID_PLATFORM" \
			-DANDROID_ABI="$ABI" \
			-DANDROID_STL="none" \
			-DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
			-DCMAKE_C_FLAGS="" \
			-DCMAKE_CXX_FLAGS="" \
			-DANDROID_USE_LEGACY_TOOLCHAIN_FILE="OFF"
	fi

	${MAKE_CMD}
}

build()
{
	local TARGET=$1
	local ABI=$2
	local PLATFORM=$3

	rm -r $OUT > /dev/null 2>&1

	local NINJA=`which ninja`
	if [[ -f $NINJA ]]; then
		local METHOD="Ninja"
	else
		local METHOD="make"
	fi

	cmake_build "${TARGET}" "${METHOD}" "${ABI}" "${PLATFORM}"

	[ "${PLATFORM}" == "WSL" ] && TARGET="WSL"

	local BUILD="$OUT"
	local FEASHelper_BIN="$BUILD/FEASHelper"
	local TARGE_DIR_NAME="FEASHelper-${VERSION}_${ABI}-$(TZ=UTC-8 date +%y%m%d%H%M)"
	local TARGET_DIR_PATH="./target/${TARGET}_${ABI}/${TARGE_DIR_NAME}"

	if [ -f "$FEASHelper_BIN" ]; then
		echo "复制文件中..."
		[[ ! -d "$TARGET_DIR_PATH" ]] && mkdir -p ${TARGET_DIR_PATH}
		cp -af $FEASHelper_BIN ${TARGET_DIR_PATH}
		cp -af ./Module/* ${TARGET_DIR_PATH}
		touch -c -d "2009-01-01 00:00:00" ${TARGET_DIR_PATH}/*
		echo "编译成功: ${TARGE_DIR_NAME}"
	else
		echo "error"
		exit 1
	fi
}

build "Android" "arm64-v8a" "android-31"
exit 0

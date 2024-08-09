#!/bin/bash
# Compile script for Hexagon-kernel

SECONDS=0 # builtin bash timer
TC_DIR="/home/tew404/lisa-Kernel/neutron-clang-10032024"
TC_DIRR="/home/tew404/lisa-Kernel/Clang-20.0.0git-20240807"
DEFCONFIG="lisa_defconfig"
ZIPNAME="Hexagon-kernel-lisa-$(date '+%Y%m%d-%H%M').zip"

if test -z "$(git rev-parse --show-cdup 2>/dev/null)" &&
   head=$(git rev-parse --verify HEAD 2>/dev/null); then
	ZIPNAME="${ZIPNAME::-4}-$(echo $head | cut -c1-8).zip"
fi

MAKE_PARAMS="O=out \
	ARCH=arm64  \
 	CC=$TC_DIR/bin/clang  \
	CLANG_TRIPLE=$TC_DIRR/bin/aarch64-linux-gnu- \
	CROSS_COMPILE=$TC_DIRR/bin/aarch64-linux-gnu-  \
	CROSS_COMPILE_ARM32=$TC_DIRR/bin/arm-linux-gnueabi-  \
	LLVM=1 \
	LLVM_IAS=1"

export PATH="$TC_DIR/bin:$PATH"

if [[ $1 = "-r" || $1 = "--regen" ]]; then
	make $MAKE_PARAMS $DEFCONFIG savedefconfig
	cp out/defconfig arch/arm64/configs/$DEFCONFIG
	echo -e "\nSuccessfully regenerated defconfig at $DEFCONFIG"
	exit
fi

rm -rf out
mkdir -p out

make $MAKE_PARAMS $DEFCONFIG

echo -e "\nStarting compilation...\n"
make -j$(nproc --all) $MAKE_PARAMS || exit $?

kernel="out/arch/arm64/boot/Image"
dtb="out/arch/arm64/boot/dts/vendor/qcom/yupik.dtb"
dtbo="out/arch/arm64/boot/dtbo.img"

if [ ! -f "$kernel" ] || [ ! -f "$dtb" ] || [ ! -f "$dtbo" ]; then
	echo -e "\nCompilation failed!"
	exit 1
fi

rm -rf AnyKernel3/Image
rm -rf AnyKernel3/dtb
#rm -rf AnyKernel3/dtbo.img

cp $kernel AnyKernel3
cp $dtb AnyKernel3/dtb
#cp $dtbo AnyKernel3

cd AnyKernel3
zip -r9 "../$ZIPNAME" * -x .git README.md *placeholder
cd ..
echo -e "\nCompleted in $((SECONDS / 60)) minute(s) and $((SECONDS % 60)) second(s) !"
echo "Zip: $ZIPNAME"

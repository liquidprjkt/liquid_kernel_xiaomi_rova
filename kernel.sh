#!/bin/bash
# Simple script to build kernel and upload it.
# by @sarthakroy2002, with some edits from @techyminati & @aScordino

## Use ccache
export USE_CCACHE=1
ccache -M 100G >/dev/null 2>&1

## Export important variables
export ARCH=arm64
export KBUILD_BUILD_HOST="liquidprjkt"
export KBUILD_BUILD_USER="veshrao"
KERNELDIR=${PWD}

## Clone compilators & AnyKernel
git clone --depth=1 https://gitlab.com/dakkshesh07/neutron-clang ${HOME}/compiling/clang  >/dev/null 2>&1
git clone --depth=1 https://github.com/veshrao/AnyKernel3/ ${HOME}/compiling/AnyKernel3 >/dev/null 2>&1

# Function to compile the kernel
function compile() 
{
PATH="${HOME}/compiling/clang/bin:$PATH"
KBUILD_COMPILER_STRING=$(${HOME}/compiling/clang/bin/clang --version | head -n 1 | perl -pe 's/\(http.*?\)//gs' | sed -e 's/  */ /g' -e 's/[[:space:]]*$//')

## Start the actual compilation process
make rova_defconfig
make -j$(nproc --all) ARCH=arm64 \
                      CROSS_COMPILE=aarch64-linux-gnu- \
                      CROSS_COMPILE_ARM32=arm-linux-gnueabi- \
                      CC=clang \
                      LLVM=1
}

# Function to upload the kernel flashable AnyKernel file
# to temp.sh
function upload()

{
## Edit those kernel variables.
KERNEL_NAME=liquid
CODENAME=LIMEROCK
VERSION=1.2
DATE=$(date +"%d-%m-%Y_%H-%M")
export ZIP="$KERNEL_NAME"-"$VERSION"-"$CODENAME"-"$DATE".zip
cd ${HOME}/compiling/AnyKernel3/
cp "$KERNELDIR"/out/arch/arm64/boot/Image.gz-dtb ${HOME}/compiling/AnyKernel3
zip -r "$ZIP" * -x README.md  >/dev/null 2>&1 
LINK=$(curl -T "$ZIP" temp.sh)
MD5="$(md5sum "$ZIP")"
echo -e "$LINK"
echo -e "$MD5"

## Cleanup of AnyKernel. Don't worry, all these files were copied, not moved.
rm -rf ${HOME}/compiling/AnyKernel3/
cd "$KERNELDIR"
}

compile
upload
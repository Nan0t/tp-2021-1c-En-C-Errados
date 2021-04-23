#!/bin/bash

declare SCRIPT_DIR="$( dirname $0 )"
declare PTR_SIZE=$( getconf LONG_BIT )
declare ARCH="x86"

cd $SCRIPT_DIR

if [ $PTR_SIZE=="64" ];then
    ARCH="x86_64"
fi


../premake/$arch/premake5 --arch=$ARCH --file=../premake5.lua gmake2
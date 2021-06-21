#!/bin/sh

cd `dirname $0`

if [ "$1" -eq "release" ];then
	BUILD_TYPE="Release"
	cd build/release
else
	BUILD_TYPE="Debug"
	cd build/debug
	
fi

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ../../prj/cmake

echo $?

if [ $? -eq 0 ];then
	make
	if [ $? -eq 0 ]; then
mv *.exe ../../run/bin
		echo "success!"
	fi
fi


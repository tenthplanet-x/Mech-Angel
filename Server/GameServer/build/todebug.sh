#!/bin/sh

show_usage="args: [--selfarg=]"
#参数
opt_selfarg=""
GETOPT_ARGS=`getopt -o s: -al selfarg: -- "$@"`
eval set -- "$GETOPT_ARGS"
#获取参数
while [ -n "$1" ]
do
	case "$1" in
		-s|--selfarg) opt_selfarg=$2; shift 2;;
		--) break ;;
		*) echo $1,$2,$show_usage; break ;;
	esac
done

opt_selfarg=$(echo $opt_selfarg | awk 'BEGIN {FS="|"; outInfo=""} END {for(i=1;i<=NF;i++) outInfo=(outInfo""$i" "); print outInfo}')
echo "opt_selfarg:"$opt_selfarg

cd ..
mkdir -p ./build/debug
rm -f ./build/0RELEASE
touch ./build/0DEBUG
cmake -DCMAKE_BUILD_TYPE=Debug -DEXECUTABLE_OUTPUT_PATH="./build/debug" $opt_selfarg .
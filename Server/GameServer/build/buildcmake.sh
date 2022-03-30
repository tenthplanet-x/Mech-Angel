#!/bin/sh
fileList() {
	local basedir=$1
	local cpptype=$2
	for dir in $(ls $basedir)
	do
		dir=$basedir$dir
		if [ -f "$dir" -o -L "$dir" ];then
			local filename=`echo $dir | awk -F"." '{if(NF>2){print $(NF);}}'`
			if [ -z "$cpptype" ]; then
				if [[ $filename = "c" || $filename = "cc" || $filename = "cpp" ]]; then
					echo ${dir:2} 
				fi
			else
				if [ $filename = $cpptype ]; then
					echo ${dir:2} 
				fi
			fi
		else
			fileList $dir/
		fi
	done
}

# dir model dirs
builddir() {
	local strdir=$1
	local model=$2
	local append=$3
	local cpptype=$4
	local basedir=`pwd`
	echo "build $strdir"
	cd $strdir
	rm -f CMakeLists.txt
	echo "# CMakeLists file">>CMakeLists.txt
	echo "">>CMakeLists.txt
	echo "add_library($model  ">>CMakeLists.txt
	
	fileList "./" $cpptype | while read line
	do
		echo $append/$line >> CMakeLists.txt
	done
	echo ")">>CMakeLists.txt
	cd $basedir
}

makesecpath(){
	local basedir=$1
	local todir=$2
	local usedir=$3
	local libname=$4
	local diradd=$5
	local cpptype=$6
	local secdir=$7
	cd $basedir/$todir
	builddir $usedir $libname "${diradd}${todir}${usedir}" $cpptype
	mkdir -p $basedir/$secdir
	mv $usedir/CMakeLists.txt $baseDir/$secdir
}

cd ..
baseDir=$(pwd)

cd ../../BuildScripts
sh ./auto_version.sh
cd $baseDir

builddir Src libGameServer "." ""
builddir linux libLinux "." ""

makesecpath $baseDir "../../Common/" Include libCommon "../../" "" "cmkes/common"
makesecpath $baseDir "../../Common/GameLogic/" Src libGameLogic "../../" "" "cmkes/gamelogic"
makesecpath $baseDir "../../Common/Protocol/" src libProtocol "../../" "" "cmkes/protocol"
makesecpath $baseDir "../../Common/Utility/" Src libUtility "../../" "" "cmkes/utility"
makesecpath $baseDir "../../Common/Network/" src libNetwork "../../" "" "cmkes/network"
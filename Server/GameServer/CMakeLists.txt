# CMakeLists file
cmake_minimum_required(VERSION 2.6)

project (jz_server)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
if (ROBOT_ENABLE)
	set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -m64 -ggdb -DDEBUG -D_SHIPPING -DT6_SERVER -D_SERVER_BUILD -DROBOT_ENABLE -DMSG_TO_GAME -Wno-deprecated")
	set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -m64 -D_SHIPPING -DT6_SERVER -D_SERVER_BUILD -DMSG_TO_GAME -DROBOT_ENABLE -Wno-deprecated")
else ()
	set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -m64 -ggdb -DDEBUG -D_SHIPPING -DT6_SERVER -D_SERVER_BUILD -DMSG_TO_GAME -Wno-deprecated")
	set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -m64 -D_SHIPPING -DT6_SERVER -D_SERVER_BUILD -DMSG_TO_GAME -Wno-deprecated")
endif ()
set(CMAKE_EXE_LINKER_FLAGS "-g -Wl")

set(LINK_BASE_SIMBLES  libGameLogic libProtocol libUtility libNetwork libCommon libLinux 
					  -lboost_system-mt -lboost_thread-mt -lboost_chrono-mt -lboost_regex-mt -lrt -lz -llzma 
					  -lprotobuf-lite -ldl -llua -liconv -lssl)
					
find_path(MYSQL_INCLUDE_PATH mysql.h /usr/local/mysql/include /usr/include/mysql)
if (MYSQL_INCLUDE_PATH)
	find_path(MYSQL_LIB_PATH libmysqlclient_r.a /usr/local/mysql/lib /usr/lib64/mysql)
else()
	set(MYSQL_INCLUDE_PATH /usr/local/mysql/include)
	set(MYSQL_LIB_PATH /usr/local/mysql/lib)
endif()

include_directories("/usr/local"
					"/usr/local/include"
					"${MYSQL_INCLUDE_PATH}"
					"/usr/local/sqlite3/include"
					"${PROJECT_SOURCE_DIR}/linux"
					"${PROJECT_SOURCE_DIR}/Src"
					"${PROJECT_SOURCE_DIR}/Src/Logic"
					"${PROJECT_SOURCE_DIR}/Src/NetWork"
					"${PROJECT_SOURCE_DIR}/Src/Handler"
					"${PROJECT_SOURCE_DIR}/Src/HttpHandler"
					"${PROJECT_SOURCE_DIR}/Src/Utility"
					"${PROJECT_SOURCE_DIR}/Src/GameStory"
					"${PROJECT_SOURCE_DIR}/Src/Transcription"
					"${PROJECT_SOURCE_DIR}/Src/StateMachine"
					"${PROJECT_SOURCE_DIR}/Src/Script"
					"${PROJECT_SOURCE_DIR}/Src/ResourceAlert"
					"${PROJECT_SOURCE_DIR}/Src/Transaction"
					"${PROJECT_SOURCE_DIR}/Src/Path"
					"${PROJECT_SOURCE_DIR}/Src/Skill"
					"${PROJECT_SOURCE_DIR}/Src/Rank"
					"${PROJECT_SOURCE_DIR}/Src/Text"
					"${PROJECT_SOURCE_DIR}/Src/Arena"
					"${PROJECT_SOURCE_DIR}/Src/Activity"
					"${PROJECT_SOURCE_DIR}/Src/Newbee"
					"${PROJECT_SOURCE_DIR}/Src/AsyHttpClient"
					"${PROJECT_SOURCE_DIR}/Src/VIP"
					"${PROJECT_SOURCE_DIR}/Src/CSV"
					"${PROJECT_SOURCE_DIR}/Src/DailyDataReport"
					"${PROJECT_SOURCE_DIR}/Src/FacebookReward"
					"${PROJECT_SOURCE_DIR}/Src/MessageChannel"
					"${PROJECT_SOURCE_DIR}/Src/Activity"
					"${PROJECT_SOURCE_DIR}/Src/TextFilterChannel"
					"${PROJECT_SOURCE_DIR}/../../Common/Include"
					"${PROJECT_SOURCE_DIR}/../../Common/Utility/Include"
					"${PROJECT_SOURCE_DIR}/../../Common/Protocol/include/cs"
					"${PROJECT_SOURCE_DIR}/../../Common/Protocol/include/ss"
					"${PROJECT_SOURCE_DIR}/../../Common/Protocol/include/com"
					"${PROJECT_SOURCE_DIR}/../../Common/Protocol/include/common"
					"${PROJECT_SOURCE_DIR}/../../Common/GameLogic/Include"
					"${PROJECT_SOURCE_DIR}/../../Common/GameLogic/Src"
					"${PROJECT_SOURCE_DIR}/../../Common/Network/include"
					"${PROJECT_SOURCE_DIR}/../../Dependency/lua-5.1.4/src"
					"${PROJECT_SOURCE_DIR}/../../Dependency/Sqlite3/src"
					"${PROJECT_SOURCE_DIR}/../../Dependency/rapidjson/include"
					"${PROJECT_SOURCE_DIR}/../../Server/linux/include"
					"${PROJECT_SOURCE_DIR}/../../Server/linux/lzma"
					"${PROJECT_SOURCE_DIR}/../../Dependency/openssl/include"
					)
link_directories("${PROJECT_SOURCE_DIR}/../../Server/linux/lib"
                 "/usr/local/lib"
				 "${MYSQL_LIB_PATH}"
				 "/usr/local/sqlite3/lib"
				)
				

#子目录
add_subdirectory(Src)
add_subdirectory(linux)
add_subdirectory(cmkes/common)
add_subdirectory(cmkes/gamelogic)
add_subdirectory(cmkes/protocol)
add_subdirectory(cmkes/utility)
add_subdirectory(cmkes/network)

#执行对象
add_executable(gamesvrd		Src/Main.cpp)
#链接
target_link_libraries(gamesvrd		libGameServer ${LINK_BASE_SIMBLES} -lpthread)

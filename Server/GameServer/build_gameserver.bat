@echo off
rem /All 
BuildConsole.exe GameServer.sln /prj="*" /build   /Log=build_gameserver.log  /OpenMonitor /cfg="Shipping_Server|x64"
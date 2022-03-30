#pragma once
/*
由于我们在进行Lua的函数调用时候依赖LuaPushValue和LuaGetValue进行参数的自动压栈和参数获取操作
对于每一个导出到Lua的结构体和类我们都会自动生成对应的这两个函数，但是在C++中调用Lua函数时
要保证我们在当前编译单元都能看到Lua函数的参数中涉及到的C++类的LuaPushValue和LuaGetValue重载
所以我们把这些函数统一生成到一个inl文件中，在这里包含
*/
#include "LS_LuaLib.h"
#include "SC_GameLogicScriptInterfaceHeader.h"
#include "SC_ServerScriptInterfaceHeader.h"
#include "LS_LuaState.h"
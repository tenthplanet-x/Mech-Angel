//公共结构定义文件
#ifndef _H_CommandData_H_
#define _H_CommandData_H_
#include "LC_Define.h"

//配置
struct SKeyValueConfig
{
	SKeyValueConfig()
	{
		key = 0;
		value = 0;
	}

	SKeyValueConfig( int32_t _key, int32_t _value )
	{
		key = _key;
		value = _value;
	}

	int32_t key;
	int32_t value;
};

struct SItemConfig
{
	SItemConfig()
	{
		id = 0;
		num = 0;
		bind = 0;
	}

	SItemConfig( int32_t _id, int32_t _num, int32_t _bind )
	{
		id = _id;
		num = _num;
		bind = _bind;
	}
	int32_t id;
	int32_t num;
	int32_t bind;
};

#endif

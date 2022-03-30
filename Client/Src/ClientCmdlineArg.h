#pragma once

#include <map>
#include "StringDefine.h"

//////////////////////////////////////////////////////////////////////////

class ClientCmdlineArg
{
public:

	typedef std::map<StringType, StringType>	KVMap;

	//////////////////////////////////////////////////////////////////////////

	static const StringType	&	Get();
	static const KVMap	&		GetAllKV(void);

	//////////////////////////////////////////////////////////////////////////

	static void			Assign(const StringType & strCmdline);
	static void			SetKV(const StringType & key, const StringType & value);
	static void			Clear();

	//////////////////////////////////////////////////////////////////////////

	static bool					HasValue(const StringType & key);
	static const StringType &	GetValue(const StringType & key);
	static StringType			GetValueSafe(const StringType & key);
	static bool					TryGetValue(const StringType & key, StringType & value);

	//////////////////////////////////////////////////////////////////////////
};

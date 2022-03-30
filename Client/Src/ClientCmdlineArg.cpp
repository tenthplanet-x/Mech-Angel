#include "ClientPCH.h"
#include "ClientCmdlineArg.h"

//////////////////////////////////////////////////////////////////////////

static StringType				g_cmdline;
static ClientCmdlineArg::KVMap	g_cmdlineArg;

//////////////////////////////////////////////////////////////////////////

const StringType & ClientCmdlineArg::Get()
{
	return g_cmdline;
}

const ClientCmdlineArg::KVMap	& ClientCmdlineArg::GetAllKV(void)
{
	return g_cmdlineArg;
}

//////////////////////////////////////////////////////////////////////////

void ClientCmdlineArg::Assign(const StringType & strCmdline)
{
	g_cmdline = strCmdline;
	g_cmdlineArg.clear();

	TStringVector kCmds = Utility::UT_BaseStringUtility::SplitString(g_cmdline, ' ');
	for (TStringVector::iterator it = kCmds.begin(); it != kCmds.end(); ++it)
	{
		TStringVector kArg = Utility::UT_BaseStringUtility::SplitString(*it, '=');
		if (kArg.size() == 2)
		{
			const StringType & k = kArg[0];
			const StringType & v = kArg[1];

			g_cmdlineArg[k] = v;
		}
	}
}
void ClientCmdlineArg::SetKV(const StringType & key, const StringType & value)
{
	g_cmdlineArg[key] = value;
}

void ClientCmdlineArg::Clear()
{
	g_cmdline.clear();
	g_cmdlineArg.clear();
}

//////////////////////////////////////////////////////////////////////////

bool ClientCmdlineArg::HasValue(const StringType & key)
{
	bool b = g_cmdlineArg.find(key) != g_cmdlineArg.end();
	return b;
}

const StringType & ClientCmdlineArg::GetValue(const StringType & key)
{
	return g_cmdlineArg[key];
}

StringType ClientCmdlineArg::GetValueSafe(const StringType & key)
{
	StringType	r;
	TryGetValue(key, r);
	return r;
}

bool ClientCmdlineArg::TryGetValue(const StringType & key, StringType & value)
{
	KVMap::iterator iter = g_cmdlineArg.find(key);
	if (iter != g_cmdlineArg.end())
	{
		value = iter->second;

		return true;
	}
	return false;
}

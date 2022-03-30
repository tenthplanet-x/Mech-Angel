#ifndef __UT_ACTIVATION_CODE_H__
#define __UT_ACTIVATION_CODE_H__

#pragma once

#include "redis.pb.h"

#define UT_ACTIVATION_CODE_ASC_BASE 0x01
#define UT_ACTIVATION_CODE_ASC_UPPER 0xFF

#define UT_ACTIVATION_CODE_BIT_CODE_ID 5
#define UT_ACTIVATION_CODE_BIT_CODE_SERIAL 5

#define UT_ACTIVATION_CODE_RAW_BIT_GRANU 2
#define UT_ACTIVATION_CODE_FMT_BIT_GRANU 3

const int UT_ACTIVATION_CODE_BIT_HEAD = (UT_ACTIVATION_CODE_BIT_CODE_ID + UT_ACTIVATION_CODE_RAW_BIT_GRANU - 1)/UT_ACTIVATION_CODE_RAW_BIT_GRANU*UT_ACTIVATION_CODE_RAW_BIT_GRANU;
const int UT_ACTIVATION_CODE_BIT_BODY = (UT_ACTIVATION_CODE_BIT_CODE_ID + UT_ACTIVATION_CODE_BIT_CODE_SERIAL + UT_ACTIVATION_CODE_RAW_BIT_GRANU - 1)/UT_ACTIVATION_CODE_RAW_BIT_GRANU*UT_ACTIVATION_CODE_RAW_BIT_GRANU;

namespace Utility{
	enum ActitationCodeFlag
	{
		UT_ACT_CODE_FG_PRIVATE_KEY = 0,
		UT_ACT_CODE_FG_PUBLIC_KEY,
	};
	struct ActivationCodeEntry
	{
		ActivationCodeEntry()
			:iCodeID(0)
			,iCodeSerial(0)
		{}

		std::string FormatActivationCodeHead(void);
		std::string FormatActivationCode(void);
		bool ParseActivationCode(const std::string& sCode);

		int32_t iCodeID;
		int32_t iCodeSerial;
	};

	extern bool CheckCodeKeyValid(const redis_define::encrypt_key_entry& kcodedef, bool bilateral);
	extern bool AutoCodeDef(redis_define::activation_code_define& kcodedef);
	extern bool GenerateActivationCode(redis_define::activation_code_define& kcodedef, mem::list<std::string>& kFinalCodes);
	

	extern std::string Dec2AscII(int64_t iVal, unsigned char base, unsigned char upper);
	extern int64_t AscII2Dec(const std::string& sVar, unsigned char base, unsigned char upper);

	extern bool ParseActivationCode(Redis_Handler* pkRedisHandler, const std::string& sFinalCode, ActivationCodeEntry& kCode);
	extern bool EmbedActivationCode(Redis_Handler* pkRedisHandler, redis_define::activation_code_define& kcodedef);
}
#endif
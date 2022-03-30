#ifdef ACTIVATION_CODE
#include "ACGameServerApp.h"
#else
#include "GameServerApp.h"
#endif
#include "UT_ActivationCode.h"

#include "UT_Redis.h"

#include "openssl/rsa.h"
#include "openssl/err.h"

#include "boost/lexical_cast.hpp"

#include <algorithm>

using namespace redispp;
using namespace Utility;

namespace Utility{

	static const char kCodeSet[] = {
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
		65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 
		97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122
	};

	struct InnerCodeSet
	{
		InnerCodeSet()
			:szCodeSize(0)
		{
			memset(kCodeSetReverse, 0, sizeof(kCodeSetReverse));
			InitCodeSet();
		}

		inline char TheFillChar(void) const
		{
			return kCodeSet[0];
		}

		void InitCodeSet(void)
		{
			szCodeSize = sizeof(kCodeSet);

			for (size_t i=0; i<szCodeSize; ++i)
			{
				kCodeSetReverse[(int)kCodeSet[i]] = (char)i;
			}
		}

		std::string ToCodeSetStr(uint64_t uiVal)
		{
			std::string ss;

			do 
			{
				ss.push_back(kCodeSet[uiVal % szCodeSize]);
				uiVal /= szCodeSize;
			} while (uiVal);

			return std::string(ss.rbegin(), ss.rend());
		}

		uint64_t FromCodeSetStr(const std::string& str)
		{
			uint64_t uiVal = 0;

			std::string::const_iterator cit = str.begin();
			for (; cit!=str.end(); ++cit)
			{
				uiVal *= szCodeSize;

				uiVal += kCodeSetReverse[(int)*cit];
			}

			return uiVal;
		}

		size_t szCodeSize;
		char kCodeSetReverse[UT_ACTIVATION_CODE_ASC_UPPER-UT_ACTIVATION_CODE_ASC_BASE+1]; 
	};

	static InnerCodeSet stInnerCoder;

std::string Dec2AscII(int64_t iVal, unsigned char base, unsigned char upper)
{
	std::string rslt;
	unsigned int interval = upper - base;
	uint64_t iTemp = iVal;
	do
	{
		rslt.push_back(base + (uint8_t)(iTemp % interval));
		iTemp = iTemp/interval;
	}while (iTemp);

	return std::string(rslt.rbegin(), rslt.rend());
}
int64_t AscII2Dec(const std::string& sVar, unsigned char base, unsigned char upper)
{
	int64_t iVal = 0;
	unsigned int interval = upper - base;
	std::string::const_iterator cit = sVar.begin();

	for (; cit!=sVar.end(); ++cit)
	{
		iVal *= interval;
		iVal += ((unsigned char)*cit - base);
	}
	return iVal;
}

unsigned char Dec2Hex(unsigned int dec)
{
	if (dec>=0 && dec <10)
	{
		return '0' + dec;
	}
	else if (dec >= 10)
	{
		return 'A' + dec - 10; 
	}
	return 0;
}

unsigned int Hex2Dec(unsigned char ch)
{
	unsigned char r = 0;
	if (ch>='A' && ch <= 'F')
	{
		r = 0x0A + ch - 'A';  
	}
	else if(ch>='0' && ch <= '9')
	{
		r = ch - '0';
	}
	else if (ch>='a' && ch <= 'f')
	{
		r = 0x0A + ch - 'a';  
	}

	return r;
}

std::string BinStr2CharStr(const std::string& sVar, size_t sz)
{
	std::stringstream ss;
	ss.fill(stInnerCoder.TheFillChar());

	std::string sBin = sVar.substr(0, sz);
	while (sBin.size() < sz)
	{
		sBin.push_back('\0');
	}

	uint64_t uiVal = 0;

	//word >> kCodeSet 2:3	
	std::string::const_iterator cit = sBin.begin();
	int i = 1;
	for (; cit!=sBin.end(); ++cit, i^=1)
	{
		unsigned char ch = (*cit);
		uiVal = uiVal << 8;
		uiVal |= ch & 0xFF;

		if (i==0)
		{
			ss.width(UT_ACTIVATION_CODE_FMT_BIT_GRANU);
			ss << stInnerCoder.ToCodeSetStr(uiVal); 
			uiVal = 0;
		}
	}

	if (uiVal)
	{
		ss.width(UT_ACTIVATION_CODE_FMT_BIT_GRANU);
		ss << stInnerCoder.ToCodeSetStr(uiVal);
	}

	return ss.str();
}
std::string CharStr2BinStr(const std::string& sVar)
{
	std::stringstream ss;

	//kCodeSet >> word 3:2
	size_t iStep = UT_ACTIVATION_CODE_FMT_BIT_GRANU;
	size_t iPos = 0;

	while(iPos < sVar.size())
	{
		uint64_t uiVal = stInnerCoder.FromCodeSetStr(sVar.substr(iPos, iStep));

		unsigned char stmp[UT_ACTIVATION_CODE_RAW_BIT_GRANU] = {0};		
		for(int i=sizeof(stmp)-1; uiVal && i>=0; --i) 
		{
			stmp[i] = (unsigned char)(uiVal & 0xFF);
			uiVal = uiVal >> 8;
		};
		
		ss << std::string((char*)stmp, 2);

		iPos += iStep;
	}

	return ss.str();
}
std::string ActivationCodeEntry::FormatActivationCodeHead(void)
{
	std::stringstream stemp;

	stemp.fill(UT_ACTIVATION_CODE_ASC_BASE);
	stemp.width(UT_ACTIVATION_CODE_BIT_CODE_ID);
	stemp << Dec2AscII(iCodeID, UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);

	return stemp.str();
}
std::string ActivationCodeEntry::FormatActivationCode(void)
{
	std::stringstream stemp;

	stemp.fill(UT_ACTIVATION_CODE_ASC_BASE);
	stemp.width(UT_ACTIVATION_CODE_BIT_CODE_ID);
	stemp << Dec2AscII(iCodeID, UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);

	stemp.fill(UT_ACTIVATION_CODE_ASC_BASE);
	stemp.width(UT_ACTIVATION_CODE_BIT_CODE_SERIAL);
	stemp << Dec2AscII(iCodeSerial, UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);

	return stemp.str();
}
bool ActivationCodeEntry::ParseActivationCode(const std::string& sCode)
{
	if (sCode.empty())
	{
		return false;
	}

	int iOff = 0;
	do
	{
		iCodeID = (int32_t)AscII2Dec(sCode.substr(iOff, UT_ACTIVATION_CODE_BIT_CODE_ID), UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);
		iOff += UT_ACTIVATION_CODE_BIT_CODE_ID;
		if (sCode.empty())
		{
			break;
		}

		iCodeSerial = (int32_t)AscII2Dec(sCode.substr(iOff, UT_ACTIVATION_CODE_BIT_CODE_SERIAL), UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);
		iOff += UT_ACTIVATION_CODE_BIT_CODE_SERIAL;
		if (sCode.empty())
		{
			break;
		}
	}while(0);

	return true;
}

bool ActivationDecrypt(Redis_Handler* pkRedisHandler, int32_t iCodeID, const std::string& sEncryptCode, std::string& sCode)
{
	if (NULL == pkRedisHandler)
	{
		return false;
	}

	int32_t iRet = 0;
	bool rslt = true;

	const std::string sCodeID = boost::lexical_cast<std::string>(iCodeID);
	const std::string& sRedisPrefix = GetGameServerApp()->GetRedisGlobalPrefix();

	StringReply kDefineReply;
	UT_REDIS_CMD_WITH_REPLY(iRet, pkRedisHandler, kDefineReply, hget, sRedisPrefix+UT_REDIS_KEY_CODE_DEFINES, sCodeID);

	if (iRet!=0 || !kDefineReply.result())
	{
		return false;
	}

	redis_define::activation_code_define kcodedefine;
	if(!kcodedefine.ParseFromString(kDefineReply))
	{
		return false;
	}

	switch(kcodedefine.encrypt_key().type())
	{
	case UT_ACT_CODE_FG_PRIVATE_KEY:
		{
			const unsigned char* Pt = (const unsigned char*)kcodedefine.encrypt_key().pri().c_str();

			RSA *p_rsa = d2i_RSAPrivateKey(NULL, &Pt, (long)kcodedefine.encrypt_key().pri().size());
			if (NULL == p_rsa)
			{
				return false;
			}
			int rsa_len = RSA_size(p_rsa);
			unsigned char buf[4096] = {0}; 

			int iSize = RSA_private_decrypt(rsa_len, (const unsigned char*)sEncryptCode.substr(UT_ACTIVATION_CODE_BIT_HEAD).c_str(), buf, p_rsa, RSA_NO_PADDING);

			if (iSize >= 0)
			{
				sCode.append((const char *)buf, iSize);
			}
			else
			{
				rslt = false;
			}

			RSA_free(p_rsa);	
		}
		break;
	case UT_ACT_CODE_FG_PUBLIC_KEY:
		{
			const unsigned char* Pt = (const unsigned char*)kcodedefine.encrypt_key().pub().c_str();

			RSA *p_rsa = d2i_RSAPublicKey(NULL, &Pt, (long)kcodedefine.encrypt_key().pub().size());
			if (NULL == p_rsa)
			{
				return false;
			}
			int rsa_len = RSA_size(p_rsa);
			unsigned char buf[4096] = {0}; 

			int iSize = RSA_public_decrypt(rsa_len, (const unsigned char*)sEncryptCode.substr(UT_ACTIVATION_CODE_BIT_HEAD).c_str(), buf, p_rsa, RSA_NO_PADDING);

			if (iSize >= 0)
			{
				sCode.append((const char *)buf, iSize);
			}
			else
			{
				rslt = false;
			}

			RSA_free(p_rsa);	
		}
		break;
	default:
		break;
	}

	return true;
}
bool ParseActivationCode(Redis_Handler* pkRedisHandler, const std::string& sFinalCode, ActivationCodeEntry& kCode)
{
	if (NULL==pkRedisHandler || sFinalCode.empty())
	{
		return false;
	}

	std::string sBinStr = CharStr2BinStr(sFinalCode);

	int iOff = 0;
	kCode.iCodeID = (int32_t)AscII2Dec(sBinStr.substr(iOff, UT_ACTIVATION_CODE_BIT_CODE_ID), UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);

	std::string sCode;

	if (!ActivationDecrypt(pkRedisHandler, kCode.iCodeID, sBinStr, sCode))
	{
		return false;
	}
	if (sCode.empty())
	{
		return false;
	}

	if( !kCode.ParseActivationCode(sCode) )
	{
		return false;
	}


	return true;
}
bool GenerateRSAKey(int iBits, unsigned long e, std::string& pub, std::string& pri)
{
	RSA *p_rsa = RSA_new();
	BIGNUM *p_bne = BN_new();
	BN_set_word(p_bne, e);

	RSA_generate_key_ex(p_rsa, iBits, p_bne, NULL);

// 	FILE* fp = fopen("key", "w");
// 
// 	RSA_print_fp(fp, p_rsa, 0);
// 	fclose(fp);
// 
// 	RSA* pPubRsa = RSAPublicKey_dup(p_rsa);
// 	//RSA* pPriRsa = RSAPrivateKey_dup(p_rsa);
// 	unsigned char efrom[2048] = "0123456789";
// 	unsigned char eto[2048] = {0};
// 	int eSize = RSA_public_encrypt(RSA_size(p_rsa), efrom, eto, p_rsa,  RSA_NO_PADDING);
// 	eSize = RSA_private_encrypt(RSA_size(p_rsa), efrom, eto, p_rsa,  RSA_NO_PADDING);


	unsigned char buf[4096] = {0};
	unsigned char* pt = NULL;
	int iSize = 0;
	if (p_rsa)
	{
		pt = buf;
		iSize = i2d_RSAPublicKey(p_rsa, &pt);
		if (iSize>=0)
		{
			pub.append((char*)buf, iSize);
		}
	}

	if (p_rsa)
	{
		pt = buf;
		iSize = i2d_RSAPrivateKey(p_rsa, &pt);
		if (iSize>=0)
		{
			pri.append((char*)buf, iSize);
		}
	}

	//RSA_free(pPubRsa);
	//RSA_free(pPriRsa);
	RSA_free(p_rsa);
	return true;
}
bool CheckCodeKeyValid(const redis_define::encrypt_key_entry& kkey, bool bilateral)
{
	if (bilateral)
	{
		return kkey.pub().size() && kkey.pri().size();
	}

	switch(kkey.type())
	{
	case UT_ACT_CODE_FG_PRIVATE_KEY:
		{
			if (kkey.pri().empty())
			{
				return false;
			}
		}
		break;
	case UT_ACT_CODE_FG_PUBLIC_KEY:
		{
			{
				return false;
			}
		}
		break;
	default:
		return false;
		break;
	}
	return true;
}
bool AutoCodeDef(redis_define::activation_code_define& kcodedef)
{
	redis_define::encrypt_key_entry* pkentry = kcodedef.mutable_encrypt_key();
	if (!CheckCodeKeyValid(kcodedef.encrypt_key(), false))
	{
		GenerateRSAKey(UT_ACTIVATION_CODE_BIT_BODY*8, RSA_F4, *pkentry->mutable_pub(), *pkentry->mutable_pri());
	}
	else
	{
		switch (kcodedef.encrypt_key().type())
		{
		case UT_ACT_CODE_FG_PRIVATE_KEY:
			{
				if (pkentry->pub().size())
				{
					const unsigned char* Pt = (const unsigned char*)pkentry->pub().c_str();
					RSA *p_rsa = d2i_RSAPublicKey(NULL, &Pt, (long)pkentry->pub().size());
					if (NULL == p_rsa)
					{
						return false;
					}
					RSA_free( p_rsa);
				}

				if (pkentry->pri().size())
				{
					const unsigned char* Pt = (const unsigned char*)pkentry->pri().c_str();
					RSA *p_rsa = d2i_RSAPrivateKey(NULL, &Pt, (long)pkentry->pri().size());
					if (NULL == p_rsa)
					{
						return false;
					}
					RSA_free( p_rsa);
				}
				else
				{
					return false;
				}
			}
			break;
		case UT_ACT_CODE_FG_PUBLIC_KEY:
			{
				if (pkentry->pri().size())
				{
					const unsigned char* Pt = (const unsigned char*)pkentry->pri().c_str();
					RSA *p_rsa = d2i_RSAPrivateKey(NULL, &Pt, (long)pkentry->pri().size());
					if (NULL == p_rsa)
					{
						return false;
					}
					RSA_free( p_rsa);
				}

				if (pkentry->pub().size())
				{
					const unsigned char* Pt = (const unsigned char*)pkentry->pub().c_str();
					RSA *p_rsa = d2i_RSAPublicKey(NULL, &Pt, (long)pkentry->pub().size());
					if (NULL == p_rsa)
					{
						return false;
					}
					RSA_free( p_rsa);
				}
				else
				{
					return false;
				}
			}
			break;
		default:
			return false;
			break;
		}
	}
	return true;
}
bool GenerateActivationCode(redis_define::activation_code_define& kcodedef, mem::list<std::string>& kFinalCodes)
{
	ActivationCodeEntry kCode;
	kCode.iCodeID = kcodedef.id();
	kCode.iCodeSerial = 0;

	int32_t iRet = 0;
	std::string sHead = BinStr2CharStr(kCode.FormatActivationCodeHead(), UT_ACTIVATION_CODE_BIT_HEAD);
	unsigned char buf[4096] = {0}; 


	redis_define::encrypt_key_entry* pkentry = kcodedef.mutable_encrypt_key();

	switch(pkentry->type())
	{
	case UT_ACT_CODE_FG_PRIVATE_KEY:
		{
			const unsigned char* Pt = (const unsigned char*)pkentry->pub().c_str();

			RSA *p_rsa = d2i_RSAPublicKey(NULL, &Pt, (long)pkentry->pub().size());
			if (NULL == p_rsa)
			{
				return false;
			}

			for (; kCode.iCodeSerial<kcodedef.max_serial(); ++kCode.iCodeSerial)
			{
				std::string sCode = kCode.FormatActivationCode();
				int iSize = RSA_public_encrypt(RSA_size(p_rsa), (unsigned char *)sCode.c_str(), buf, p_rsa,  RSA_NO_PADDING);
				if (iSize < 0)
				{
					iRet = ERR_get_error();
					break;
				}

				std::string sFormatCode = sHead + BinStr2CharStr(std::string((char*)buf, iSize), UT_ACTIVATION_CODE_BIT_BODY);
				kFinalCodes.push_back(sFormatCode);

// 				{
// 					std::string sBinStr = CharStr2BinStr(sFormatCode);
// 
// 					int iOff = 0;
// 					kCode.iCodeID = (int32_t)AscII2Dec(sBinStr.substr(iOff, UT_ACTIVATION_CODE_BIT_CODE_ID), UT_ACTIVATION_CODE_ASC_BASE, UT_ACTIVATION_CODE_ASC_UPPER);
// 
// 					std::string sOutCode;
// 
// 					const unsigned char* upt = (const unsigned char*)pkentry->pri().c_str();
// 					RSA *up_rsa = d2i_RSAPrivateKey(NULL, &upt, pkentry->pri().size());
// 					if (NULL == up_rsa)
// 					{
// 						return false;
// 					}
// 					int rsa_len = RSA_size(up_rsa);
// 					unsigned char ubuf[4096] = {0}; 
// 
// 					int uiSize = RSA_private_decrypt(rsa_len, (const unsigned char*)sBinStr.substr(UT_ACTIVATION_CODE_BIT_HEAD).c_str(), ubuf, up_rsa, RSA_NO_PADDING);
// 					
// 					if (uiSize >= 0)
// 					{
// 						sOutCode.append((const char *)ubuf, uiSize);
// 					}
// 
// 					RSA_free(up_rsa);	
// 
// 					if (sCode != sOutCode)
// 					{
// 						printf("hahah\n");
// 					}
// 				}
			}

			RSA_free( p_rsa);
		}
		break;
	case UT_ACT_CODE_FG_PUBLIC_KEY:
		{
			const unsigned char* Pt = (const unsigned char*)pkentry->pri().c_str();

			RSA *p_rsa = d2i_RSAPrivateKey(NULL, &Pt, (long)pkentry->pri().size());
			if (NULL == p_rsa)
			{
				return false;
			}

			for (; kCode.iCodeSerial<kcodedef.max_serial(); ++kCode.iCodeSerial)
			{
				std::string sCode = kCode.FormatActivationCode();
				int iSize = RSA_private_encrypt(RSA_size(p_rsa), (unsigned char *)sCode.c_str(), buf, p_rsa,  RSA_NO_PADDING);
				if (iSize < 0)
				{
					iRet = ERR_get_error();
					break;
				}
				kFinalCodes.push_back(sHead + BinStr2CharStr(std::string((char*)buf, iSize), UT_ACTIVATION_CODE_BIT_BODY));
			}

			RSA_free( p_rsa);
		}
		break;
	default:
		break;
	}

	if (iRet!=0)
	{
		return false;
	}
	return true;
}

bool EmbedActivationCode(Redis_Handler* pkRedisHandler, redis_define::activation_code_define& kcodedef)
{
	if (NULL==pkRedisHandler)
	{
		printf("Bad server connection!please check server config!\n");
		return false;
	}

	redis_define::encrypt_key_entry* pkentry = kcodedef.mutable_encrypt_key();
	if (kcodedef.id()<=0 || pkentry->pub().empty() || pkentry->pri().empty())
	{
		printf("Bad define info!\n");
		return false;
	}

	std::string sCodeID = boost::lexical_cast<std::string>(kcodedef.id());

	const std::string& sRedisPrefix = GetGameServerApp()->GetRedisGlobalPrefix();

	int iRet = 0;
	UT_REDIS_CMD_BEGIN(iRet, pkRedisHandler);

	redispp::StringReply kCodeDefineReply = pkRedisConn->hget(sRedisPrefix+UT_REDIS_KEY_CODE_DEFINES, sCodeID);

	if (kCodeDefineReply.result())
	{
		printf("already defined code define!redefine!\n");
	}

	pkRedisConn->hset(sRedisPrefix+UT_REDIS_KEY_CODE_DEFINES, sCodeID, kcodedef.SerializeAsString());

	UT_REDIS_CMD_END(iRet, pkRedisHandler);

	if (iRet!=0)
	{
		if (pkRedisHandler)
		{
			printf("Some Error happened while embed[%d][%s]\n", iRet, pkRedisHandler->GetLastError().c_str());
		}
		return false;
	}

	return true;
}
}

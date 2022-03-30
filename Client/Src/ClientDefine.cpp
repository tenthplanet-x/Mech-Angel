#include "ClientPCH.h"
#include "ClientResDefine.h"


const char* SUB_TP_NAME(int index)
{
	switch(index)
	{
	case GameLogic::LC_SUB_TP_VEHICLE:
		return GET_MSG(CLIENTDEFINE_H_001);
	case GameLogic::LC_SUB_TP_ARROW:
		return GET_MSG(CLIENTDEFINE_H_002);
	case GameLogic::LC_SUB_TP_DEVA_EYE:
		return GET_MSG(CLIENTDEFINE_H_003);
	case GameLogic::LC_SUB_TP_VEHICLE_FIGHTING:
		return GET_MSG(CLIENTDEFINE_H_004);
	case GameLogic::LC_SUB_TP_TALISMAN:
		return GET_MSG(CLIENTDEFINE_H_005);
	case GameLogic::LC_SUB_TP_SPIRIT_PET:
		return GET_MSG(CLIENTDEFINE_H_006);
	case GameLogic::LC_SUB_TP_SPIRIT_WEAPON:
		return GET_MSG(CLIENTDEFINE_H_007);
	case GameLogic::LC_SUB_TP_HIDDEN_WEAPON:
		return GET_MSG(CLIENTDEFINE_H_008);
	case GameLogic::LC_SUB_TP_MUHUN:
		return GET_MSG(CLIENTDEFINE_H_009);
	case GameLogic::LC_SUB_TP_PLOUGH:
		return GET_MSG(CLIENTDEFINE_H_010);
	case GameLogic::LC_SUB_TP_EVIL:
		return GET_MSG(CLIENTDEFINE_H_011);
	case GameLogic::LC_SUB_TP_FAN:
		return GET_MSG(CLIENTDEFINE_H_015);
	case GameLogic::LC_SUB_TP_SUPER_BOW:
		return GET_MSG(CLIENTDEFINE_H_016);
	case GameLogic::LC_SUB_TP_HOOK:
		return GET_MSG(CLIENTDEFINE_H_017);
	case GameLogic::LC_SUB_TP_SPIRIT_FAN:
		return GET_MSG(CLIENTDEFINE_H_018);
	}
	return "";
}


const char* CASH_TYPE_NAME(int index)
{
	switch(index)
	{
	case 1:
		return GET_MSG(CLIENTDEFINE_H_012);
	case 2:
		return GET_MSG(CLIENTDEFINE_H_019);
	case 3:
		return GET_MSG(CLIENTDEFINE_H_013);
	case 4:
		return GET_MSG(CLIENTDEFINE_H_014);
	case 6:
		return "<color val=0xffffffff><image val=16>Item/Item_image_点数-紫</image></color>";
	case 7:
		return "<color val=0xffffffff><image val=16>Item/Item_image_点数-橙色</image></color>";
	}

	return "";
}

const char* ITEM_GRADE_CHAR(int index)
{
	switch(index)
	{
	case 3:
		return GET_MSG(LC_RANK_ITEM_GRADE_R);
	case 4:
		return GET_MSG(LC_RANK_ITEM_GRADE_SR);
	case 5:
		return GET_MSG(LC_RANK_ITEM_GRADE_SSR);
	default:
		return "";
	}
}

const char* ITEM_GRADE_TEXT(int index)
{
	switch(index)
	{
	case 3:
		return GET_MSG(LC_RANK_ITEM_GRADE_TEXT_R);
	case 4:
		return GET_MSG(LC_RANK_ITEM_GRADE_TEXT_SR);
	case 5:
		return GET_MSG(LC_RANK_ITEM_GRADE_TEXT_SSR);
	default:
		return "";
	}
}

const char* RUNE_ITEM_GRADE_CHAR(int index)
{
	switch(index)
	{
	case 2:
		return GET_MSG(LC_RUNE_ITEM_GRADE_2);
	case 3:
		return GET_MSG(LC_RUNE_ITEM_GRADE_3);
	case 4:
		return GET_MSG(LC_RUNE_ITEM_GRADE_4);
	case 5:
		return GET_MSG(LC_RUNE_ITEM_GRADE_5);
	default:
		return "";
	}
}
#ifndef _LC_NOVICE_TUTORIAL_H_
#define _LC_NOVICE_TUTORIAL_H_
#pragma once

#include "LC_Define.h"
#include "char_data.pb.h"

class LC_ServerPlayer;
class CNoviceTutorial
{
public:
	CNoviceTutorial();
	~CNoviceTutorial();

	void LoadData(const ::char_data_define::PBNoviceTutorial& data);
	void SaveData(::char_data_define::PBNoviceTutorial* pkData);

	void AddNoviceTutorial(LC_ServerPlayer* pkOwnerPlayer, uint32_t tutorialID);
	void GetNoviceTutorialReward(LC_ServerPlayer* pkOwnerPlayer, uint32_t tutorialID);
	void SyncNoviceTutorialInfo(LC_ServerPlayer* pkOwnerPlayer);
protected:
private:
	mem::vector<uint32_t> m_vNoviceTutorialRecord;//已观看新手教程章节id
	mem::vector<uint32_t> m_vGetRewardNoviceTutorial;//已领取新手教程奖励的id
};

#endif

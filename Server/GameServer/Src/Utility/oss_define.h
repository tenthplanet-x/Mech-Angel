#ifndef OSS_DEFINE_H_DFGFD45353
#define OSS_DEFINE_H_DFGFD45353

#include "oss_log.h"
#include "ajson_mem.hpp"
#include "LC_PackStruct.h"
#include "LC_ServerPlayer.h"
#include "LC_DailyDataReport.h"

class LC_ServerNPC;

namespace GameLogic
{
	class LC_Task;
	struct PackEntryOPRecord;
	struct PackOPRecordSet;
}

//reason: enum SystemFunctionType

class LC_ServerGuildInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_world_id(LC_ServerPlayer* player = NULL);
int cur_svr_id();
StringType make_ip_string(LC_ServerPlayer* player);
const StringType& make_Account_string(LC_ServerPlayer* player);
StringType make_item_uid(GameLogic::LC_BackPackEntry* pack);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��һ��

//����
void oss_role_lvup(LC_ServerPlayer* player, int nReason);
//��½
void oss_login(LC_ServerPlayer* player);
//�ǳ�
void oss_logout(LC_ServerPlayer* player);
//����
void oss_online(int count);
//��ֵ
void oss_recharge(LC_ServerPlayer* player, int32_t yb, const std::string& sn, uint32_t viplvl, int64_t vipcredit, int srcType);
void oss_recharge(const unique_id_impl& charID, int32_t yb, const std::string& sn, int srcType);
//����
void oss_role_comsume(LC_ServerPlayer* player, int cost_type, int64_t cost_yb, int64_t bind_yb, int shop_type, int goods_type, int shop_id, int shop_count, const StringType& item_id);
//����ʼ
void oss_role_task_start(LC_ServerPlayer* player, GameLogic::LC_Task* task, GameLogic::LC_BackPackEntry* nitem, GameLogic::LC_BackPackEntry* litem);
//�������
void oss_role_task_finished(LC_ServerPlayer* player, GameLogic::LC_Task* task, GameLogic::LC_BackPackEntryPtrList items, GameLogic::LC_BackPackEntry* litem, int gp, int exp, int sp);
//����ȡ��
void oss_role_task_cancel(LC_ServerPlayer* player, GameLogic::LC_Task* task, GameLogic::LC_BackPackEntry* nitem, GameLogic::LC_BackPackEntry* litem);
//��������
void oss_role_sk_lvup(LC_ServerPlayer* player, int sk_id, int type, int sp, int cash);
//�ǳ�ֵ���Ԫ��
void yb_income(LC_ServerPlayer* player, int yb, const std::string& reason);
//��������
void oss_party_build(LC_ServerPlayer* player, int nID, const StringType& sName, int nLvl, int nMoney, int nPolity);
//��ɢ����
void oss_party_dismiss(LC_ServerPlayer* player, int nID, LC_ServerGuildInfo*  guild);
//��������
void oss_party_levelup(LC_ServerPlayer* player, int nID, const StringType& sName, int nLvl, int nMoney, int nPolity);
//���������Ա
void oss_party_addmem(unique_id_type operateId, unique_id_type charID, int nID, const StringType& sName, int nLvl, int nMoney, int nReason);
//ɾ�������Ա
void oss_party_delmem(unique_id_type operateId, unique_id_type charID, int nID, const StringType& sName, int nLvl, int nMoney, int nReason);
//����ɱ��
void oss_role_killed_by_monster(LC_ServerPlayer* player, LC_ServerNPC* npc);
//�����ɱ��
void oss_role_killed_by_player(LC_ServerPlayer* player, LC_ServerPlayer* kill, int pk_type);
//NPC����
void oss_role_npc_buy(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* pack, int point, int npcid, const StringType& npc_name, int is_buyback);
//NPC����
void oss_role_npc_sell(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* pack, int point, int npcid, const StringType& npc_name);
//ʰȡ��Ʒ
void oss_role_pickup(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* pack);
//ɾ����Ʒ
void oss_role_del_item(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* pack);
//�ϳ���Ʒ
void oss_role_compound_item(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* pack, GameLogic::PackOPRecordSet::PackOPRecords& list, int gp);
//��ֿ�
void oss_role_store(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item);
//ȡ�ֿ�
void oss_role_takeout(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item);
//p2p����
void oss_role_trade(LC_ServerPlayer* player, int64_t gp, GameLogic::LC_BackPackEntry items1[], int32_t itemssz1, LC_ServerPlayer* player2, int64_t gp2, GameLogic::LC_BackPackEntry items2[], int32_t itemssz2);
//װ����Ƕ
void oss_role_enchase_equip(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item, GameLogic::LC_BackPackEntry* socket_item);
//��ʯ���
void oss_role_pull_equip(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item, GameLogic::LC_BackPackEntry* socket_item, int gp);
//װ��ϴ��
void oss_role_item_refine(LC_ServerPlayer* player, int opType, int opParam, GameLogic::LC_BackPackEntry* aitem, GameLogic::LC_BackPackEntry* bitem, int pre, int post);
//װ���;öȱ仯
void oss_role_dur_equip(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item, int type, int dur, int gp);
//װ���ֽ�
void oss_role_decompund_equip(LC_ServerPlayer* player, GameLogic::LC_BackPackEntryPtrList& nitems, GameLogic::PackOPRecordSet::PackOPRecords& ditems);
//װ������
void oss_role_change_equip(LC_ServerPlayer* player, int on_id, int off_id);

void make_new_item_info(GameLogic::LC_BackPackEntryPtrList& new_items, LC_ServerPlayer* player);

//����
void oss_role_vehicle_up(LC_ServerPlayer* player, int32_t iUpType, int32_t iSubType, int32_t iPreLevel, int32_t iPostLevel, int64_t iCash, GameLogic::LC_BackPackEntry* item, int32_t res, int32_t iBPDelta, int32_t iUpCount, int32_t iSkillID);
//��������
void oss_role_monster_die(LC_ServerPlayer* player, LC_ServerNPC* npc, int exp);
//�������
void oss_role_monster_drop(LC_PlayerPtrList& players, LC_ServerNPC* npc, int gp, GameLogic::LC_BackPackEntryPtrList& items, int share);
//���븱��
void oss_role_copy_enter(LC_ServerPlayer* player, int nMapResId);
//�˳�����
void oss_role_copy_exit(LC_ServerPlayer* player, int score, int gp, int exp, int sp, GameLogic::LC_BackPackEntryPtrList& items);
//���䳡��ս
void oss_role_arena(LC_ServerPlayer* player, int rank, LC_ServerPlayer* player2, int rank2, int res, attr_value_type exp, MoneyType gp);
//���䳡ÿ�ս���
void oss_role_arena_reward(LC_ServerPlayer* player, int rank, attr_value_type exp, MoneyType gp);
//���̷���
void oss_role_counter_attack(LC_ServerPlayer* player, LC_ServerNPC* npc, int damage, int exp);
//�������
void oss_role_team(LC_ServerPlayer* player, int type);
//���Ѳ���
void oss_role_friend(LC_ServerPlayer* player, int type, const StringType& uid, unique_id_type cid, const StringType& cname, int job, int chartype, int level);
//�����ʼ�
void oss_role_send_mail(LC_ServerPlayer* player, const StringType& uid, unique_id_type cid, const StringType& cname, const StringType& title, const StringType& Context, int cost, int gp, int pay, GameLogic::LC_BackPackEntryPtrList& items);
void oss_role_send_mail(const unique_id_impl& charid, const std::string& title, const std::string& Context, int cost, int gp, int pay, PS_ItemAssetEntry* items, int32_t count);
//�����ʼ�
void oss_role_recv_mail(LC_ServerPlayer* player, const StringType& send_name, const StringType& title, const StringType& Context, int pay, int gp, int delta, GameLogic::LC_BackPackEntry* item);

//
//void oss_add_item(const StringType& eid, const StringType& tm, int type, LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* pack, int count);
//void oss_client_progress(LC_ServerPlayer* player, int progress);
//void oss_role_gm_item(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item, const StringType& op_name, int type);
//void oss_role_gm_other(LC_ServerPlayer* player, int type, const StringType& op_name, const StringType& p1, const StringType& p2);
//�ֽ���
StringType oss_role_moneyflow(LC_ServerPlayer* player, int64_t nBefore, int64_t nAfter, int64_t nChange, int nMoneyType, int nFlowType, int nAction, uint64_t nSrcID);
//��Ʒ��
//void oss_role_goodsflow(LC_ServerPlayer* player, int nOperate, GameLogic::LC_BackPackEntry * item, int identifier);
//��õ���
StringType oss_role_produceitem(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item, int nProductType, uint64_t nProductDetail, int nBfNum, int nEdNum, int iItemCostMoney);
//���ĵ���
StringType oss_role_consumeitem(LC_ServerPlayer* player, GameLogic::LC_BackPackEntry* item, int nConsumeType, uint64_t nConsumeDetail, int nBfNum, int nEdNum, int iItemCostMoney);
//�����¼
StringType oss_role_sendchat(LC_ServerPlayer* player, int nChannel, const StringType& strMessage, unique_id_type nTargetId, const StringType& strTargetName);
//�̳ǹ���
void oss_shop_buy(LC_ServerPlayer* player, int64_t yb, int64_t lijin, int itemid, int count);
//��̵깺��
void oss_act_shop_buy(LC_ServerPlayer* player, int32_t iType, int32_t iKey, const StringType& sRewards, const StringType& sConsumes, const StringType& sCond, int32_t iDrawCount);
//�Ṧ��־
void oss_jump(LC_ServerPlayer* player, int32_t skilltypeid);
//���־
void oss_role_common_stat(LC_ServerPlayer* player, int32_t type, int32_t key, uint32_t opcode, int32_t oppara);
//ս������־
void oss_role_combat_score(LC_ServerPlayer* player);
//GM��־
void oss_role_gm_command(LC_ServerPlayer* player, int64_t msgid, const StringType& cmd, const StringType& full, uint32_t rslt);
//�����ձ�
void oss_daily_data_report(const data_report& kDataReport);
//��ԴԤ��
void oss_resource_alert(LC_ServerPlayer* player, int32_t iTypeID, uint64_t iTotalCount, int64_t iLimit, uint32_t iAlertTime, const StringType& sSrcCount);
//LoginStep
void oss_login_step(int server, const StringType& uid, int acttype, uint16_t platformid, int rslt);
//��������Ϣ
void oss_server_info(int server, uint32_t starttime, uint32_t conflatetime);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//360ͳ��
#endif

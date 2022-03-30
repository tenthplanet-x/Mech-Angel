#ifndef OSS_360_DEFINE_H_
#define OSS_360_DEFINE_H_

#include "oss_log.h"
#include "ajson_mem.hpp"
#include "LC_ServerPlayer.h"

//��ʱ��ôд���ϰ�����ʽ�ĺ����
#define		YUANBAO_RMB_RATE		10

typedef enum 
{
	eOssLogTaskType_Finish,
	eOssLogTaskType_Cancel,
	eOssLogTaskType_Start,
} OSS_LOG_TASK_TYPE;

typedef enum 
{
	eOssLogSignType_Try,
	eOssLogSignType_Retry,
	eOssLogSignType_360MicroTry,
	eOssLogSignType_360MicroRetry,
} OSS_LOG_SIGN_TYPE;

typedef enum 
{
	eOssLogVIPType_Enable = 1,
	eOssLogVIPType_Pay,
	eOssLogVIPType_Upgrade,
} OSS_LOG_VIP_TYPE;

typedef enum 
{
	eOssLogGoldType_Pick,
	eOssLogGoldType_Monster,
	eOssLogGoldType_Task,
} OSS_LOG_GOLD_TYPE;

typedef enum 
{
	eOssLogGiftMoneyType_Pick,
	eOssLogGiftMoneyType_Monster,
	eOssLogGiftMoneyType_Task,
} OSS_LOG_GIFT_MONEY_TYPE;

#define		OSS_360_GID			9527

int get_world_id(LC_ServerPlayer * player);

//game_enter
//��¼�״μ���flash֮ǰ�������û�������������Ϸ�����棬��δ����ɫ���û���֮ǰ�Ѵ����򲻼�¼���������Ƿ񴴽ǳɹ���Ҫ��¼��
//�ֶ����� ���� ����
//channel varchar(255) ������ ��ȡ�������봫��0��.
//��Ȼ�û���������Ϊ-1
//poster varchar(255) ������ ��ȡ�������봫��0��
//site varchar(255) ��վid ��ȡ�������봫��0��
//ip varchar(255) �û���¼ip
void oss_360_log_game_enter(LC_ServerPlayer* player);

//��Ϸ�û���¼������ done
void oss_360_log_user_login(LC_ServerPlayer* player);
//��Ϸ�û��ǳ������� done
void oss_360_log_user_logout(LC_ServerPlayer* player);
// ͳ���������� done
void oss_360_log_online_count( uint32_t nCount );
//��ɫ���� done
void oss_360_log_user_upgrade( LC_ServerPlayer* pPlayer, int nReason );
//��ɫ���������� done
void oss_360_log_create_role( LC_ServerPlayer* pPlayer );
//��¼��Ϸ����ǰ���嵵��Ϊ
// noimpl
void oss_360_log_retreat( const char* szResult );
// ��¼��ҿͻ�����Ϣ���޲����΢�˾Ͳ���¼
// NOTE: ��Щ��Ϣgs�޷�ȡ����������ƽ̨�ϱ���������������ӿڣ���ʵ��
// noimpl
void oss_360_log_client_info( LC_ServerPlayer* pPlayer );

//��¼�û���������ĵ���Ϊ�������̳ǹ���������ǹ�������������Ϊ��
//�ֶ����� ���� ����
//balance int ������, �û�������ĵ���Ϊ֮���Ԫ�����
//itemid varchar(255) ��ƷID�����ߵ�ģ��ID��
//���޹����봫����;�����ɰ�����Ϸ���������д���г����ķǹ���������;��
//amount int �������� ���޹����봫��0��
//money int ����ʹ�õ��������
//level int ��ǰ�ȼ�
//map_id varchar(255) ��ͼid
// CT_PENTACLE
// done
void oss_360_log_giftmoney_buy( uint32_t nBanalce, uint32_t nItemId, uint32_t nAmount, uint32_t nMoney, LC_ServerPlayer* pPlayer );

//��¼��ȯ��õ���Ϊ
//�ֶ����� ���� ����
//money int ��õ��������, ��ȯ�����Ϊ֮������
//balance int ��ú���ȯ�����
//reason Varchar(255) ���ԭ�� �밴�������
//��
//1��ÿ�յ�½
//2��������� 
//level int ��ǰ�ȼ�
//map_id Varchar(255) ��ͼid
// done
void oss_360_log_giftmoney_income( uint32_t money, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer );

//��¼��ȯ���ĵ���Ϊ
//�ֶ����� ���� ����
//money int ��õ��������, ��ȯ������Ϊ֮������
//balance int ��ú���ȯ�����
//reason Varchar(255) ���ԭ�� �밴�������
//��
//1��ÿ�յ�½
//2��������� 
//level int ��ǰ�ȼ�
// done
void oss_360_log_giftmoney_expend( uint32_t money, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer );

//��¼�û����������Ϊ ��¼�����ϴ�Ľ������
//�ֶ����� ���� ����
//num int ��������ý����ֵΪ������¼������20000�����Ľ����ֵΪ���Ϊ��������-20000������ֵ����10000�ļ�¼
//status varchar(255) ����ԭ�� 
//1���� 2����� 3�������񣨰��������� 4�����˽��� 5��npc���� 6������
//map_id varchar(255) ǰ����ͼid
//balance int ������������Ľ�һ��ȡ��Һ�Ľ�ҵ�����
//type int ��¼��ҵ����ͣ�1���󶨽�ң�0���ǰ󶨽��
//Tips: status ��д��ȡԭ���Ӧ��id�б�ͬʱ��Ҫ��GM����ԭ��id��ԭ�����ƵĶ�Ӧ��ϵ��GM�ɶ�����ʾʹ�ã���
// done
void oss_360_log_gold(uint32_t num, uint32_t status, uint32_t balance, uint32_t type, LC_ServerPlayer* pPlayer);

//��¼�û���Ȩ�仯
//�ֶ����� ���� ����
//nAddVipLevel int ������Ȩ�ȼ�
//viplevel  int ��ǰ��Ȩ�ȼ�
//level int ��ǰ�ȼ�
// done
void oss_360_log_vip_privilege(uint32_t nAddVipLevel, LC_ServerPlayer* pPlayer);

//��¼�û�����Ϸ�ڽ��������Ϊ
//��ע������������Ϊ����NPC �����Ľ���ʱ�������ֶ�itemid��amount����Ϊ�գ�����������Ϊ��������Ϊʱ��itemid��amount��Ϊ�գ�����
//�����������
//�ֶ����� ���� ����
//balance int �������Ϸ�ڽ��������Ϊ֮��Ľ�����
//itemid varchar(255) ��ƷID(������Ʒ��ģ��ID)
//amount int ��������
//money int �������ĵĽ������
//level int ��ǰ�ȼ�
//map_id varchar(255) ��ͼid
// done
void oss_360_log_game_buy(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, LC_ServerPlayer* pPlayer);

//��¼�û�����Ϸ�ڳ�����Ϊ��ָ�� NPC �����Ľ�����Ϊ��
//�ֶ����� ���� ����
//balance int �������Ϸ�ڳ�����Ϊ֮��Ľ�����
//itemid varchar(255) ��ƷID
//amount int ��������
//money int ���γ��ۻ�õĽ��
//level int ��ǰ�ȼ�
//map_id varchar(255) ��ͼid
// done
void oss_360_log_game_sell(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, LC_ServerPlayer* pPlayer);


//��¼���ߵ�������Ϊ
//��ע��
//�˽ӿ�Ϊͳ�Ƶ������еĲ�������˻��ǰ���shop_buy,item_use�����ظ�����һ�����������繺�򣩣��ᷢ���������ݣ�һ��shop_buy,һ��
//item_log
//��Ϸ��������ṩ��������id��������͵�������Ӧ������ 1-���䣬2-����3-ʹ�õ�
//������Ӫ��Ҫ��ͬʱ��˵��������Ĵ�С������ֻ��¼��Ҫ���ߵ���־��(����Ҫ��ҩˮ��С����Ʒ����־ǿ�ҽ��鲻����¼)
//�ֶ����� ���� ����
//itemid varchar(255) ��ƷID
//opid varchar(255) ��������id
// ��Ϸ��������ṩ��������id��������͵�������Ӧ������ 1-���䣬2-����3-ʹ�õ�
//amount int ������Ʒ������������ʾ��ã�������ʾʧȥ��
// done
// note: opid ���չ����ֶ�
void oss_360_log_item_log(uint32_t itemid, uint32_t opid, uint32_t amount, LC_ServerPlayer* pPlayer );

//��¼�û�����������Ϊ
//�ֶ����� ���� ����
//taskid varchar(255) ����ID
//map_id varchar(255) ��ͼid
//result int 0 ���
//1 ����
//2 ����
//2.2.23. dupl
//��¼�û��򸱱�����Ϊ
//TIPS
//result ��д ���� 0,1������2 ����Ҫд���ġ���ȻӰ�쵽������ص�ͳ�Ƽ��㡣
// done
void oss_360_log_task(uint32_t taskid, uint32_t result, LC_ServerPlayer* pPlayer );

//��¼�û��򸱱�����Ϊ
//TIPS
//result ��д ���� 0,1������2 ����Ҫд���ġ���ȻӰ�쵽������ص�ͳ�Ƽ��㡣
//�ֶ����� ���� ����
//group_id int �����ʶ��
//type int ��������
//0 ����
//1 ���
//result int �������
//0 δ����
//1 ��ʤ
//2 ʧ��
//level int ��ǰ�ȼ�
//map_id varchar(255) ǰ����ͼid
//dlevel int ���������Ѷȵȼ�
//dscore varchar(16) ���������֣�����SSS,SS,S,A�ȣ�
//Tip: �����ʾ��Ϊ��Ӹ���ʱ����Ӧ����ӵ�id
// done
void oss_360_log_dupl(uint32_t group_id, int type, uint32_t result, uint32_t dlevel, uint32_t dscore, LC_ServerPlayer* pPlayer );

//vip��־
//��ע��
//�˽ӿ�Ϊvip��ز���,��־����������urlencode����
//�ֶ����� ���� ����
//optype int 1 ��ͨ
//2 ����
//3 ����
//viptype varchar �ƽ� ��ʯ ����
//urlencode��������֮һ
//buytime varchar ��Ч�ڣ����磺 10��
//��Ҫurlencode
//viplv int vip��ǰ�ȼ�
// done
void oss_360_log_vip(uint32_t optype, uint32_t viptype, uint32_t buytime, uint32_t viplv, LC_ServerPlayer* pPlayer );

//ǩ����־
//��ע��
//�ֶ����� ���� ����
//type int 0 ǩ��
//1 ��ǩ
// done
void oss_360_log_sign(uint32_t type, LC_ServerPlayer* pPlayer);

//��¼װ���Ĳ���
//��ע��
//�˽ӿ�Ϊװ����
//�ֶ����� ���� ����
//equipid varchar(255) װ��ID��guid��
//itemid varchar(255) ����id(��ƷID)
//opid varchar(255) ��������id ���� ��ת ���ٵȣ���Ҫid�б�
//amount int װ������
// todo
void oss_360_log_equip(uint32_t equipid, uint32_t itemid, uint32_t opid, uint32_t amount, LC_ServerPlayer* pPlayer );

//��¼�û��л���ͼ����Ϊ
//�ֶ����� ���� ����
//level int ��ǰ�ȼ�
//nonce_map varchar(255) ��ǰ��ͼid
//to_map varchar(255) ǰ����ͼid
// done
void oss_360_log_map( uint32_t level, uint32_t nonce_map, uint32_t to_map, LC_ServerPlayer* pPlayer );

//��¼�û���ս������־
//�ֶ����� ���� ����
//before int ���Ļ�����ǰ��ս��ֵ
//amount int �ñ�������ʾս���ı仯ֵ��amount Ϊ����ʾս�����ӣ�Ϊ����ʾս������
//after int ���Ļ����Ӻ��ս��ֵ
//type int ս���������͵�id,(��Ҫ�������ṩ����id���������Ķ�Ӧ��ϵ)
// noimpl: not such logic
void oss_360_log_warpot(uint32_t before, uint32_t amount, uint32_t after, uint32_t type, LC_ServerPlayer* pPlayer);

//recharge
//��¼�û���ֵ��Ϊ
//�ֶ����� ���� ����
//amount int ��ֵԪ����
//money int ��ֵRMB,ԪΪ��λ
//balance int Ԫ������¼��ֵ֮������
//level int ��ǰ�ȼ�
//order varchar(255) ��Ӫ������
//channel varchar(255) ����Ϊ�գ������ֵ����: GM
//legion varchar(255) ����ID��������ʱΪ��
// done
void oss_360_log_recharge(uint32_t amount, uint32_t money, uint32_t balance, const char*  order, const char* channel, LC_ServerPlayer* pPlayer);

//��¼����Ԫ������
//��ע�����г���ͨ��recharge��ֵ��Ϊ��õ�Ԫ��������¼�ڱ��ӿڣ��˽ӿ���recharge��ֵ�ӿں���һ�𣬾���ȫ����Ԫ������;��
//�ֶ����� ���� ����
//amount int ����Ԫ����
//balance int ����������������֮���������
//level int ��ǰ�ȼ�
//reason varchar(255) �����
//��ֵ����
//�밴����Ϸʵ����� ��ȫ�������������reason���ձ�������
//Tips: reason ��д��ȡԭ���Ӧ��id�б�ͬʱ��Ҫ��GM����ԭ��id��ԭ�����ƵĶ�Ӧ��ϵ��GM�ɶ�����ʾʹ�ã���
// done
// note: reason ���չ����ֶ�
void oss_360_log_yb_income(uint32_t amount, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer);

//��¼�û����̳ǵĹ�����Ϊ
//��ע����Ϸ��������ṩ��Ʒid����Ʒ�������۸�����ԵĶ�Ӧ��
//�ֶ����� ���� ����
//balance int Ԫ�����û����̳ǹ������֮���Ԫ�����
//itemid varchar(255) ��ƷID�������ǵ��ߵ�ģ��ID��
//amount int ��������
//money int ���ι�������Ԫ������
//level int ��ǰ�ȼ�
//remain int ʣ������
//map_id varchar(255) ��ͼid
//type int 0: ��ֵ�õ���Ԫ��
//1: ����;���õ���Ԫ��
//������Ԫ�����ĵ�;����shop_buy(�û����̳ǵĹ�����Ϊ)+yb_expend ��Ԫ���Ķ������ѣ�����һ��
//�����̳ǹ���shop_buy��+���ߵĶ�������(item_income)=����ʹ��(item_use)+����ʣ��(item_use��remain�ֶ� )
//type����Ϸ�̼�¼״������������ʵ��ֵԪ�����ĺͶ���Ԫ�������ģ�����һ����Ʒ�õ��˳�ֵԪ���Ͷ���Ԫ�������¼����
// done
// act shop?
void oss_360_log_shop_buy(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, uint32_t remain, uint32_t type, LC_ServerPlayer* pPlayer);

//Ԫ���Ķ�������
//��ע����¼����shop_buy(�û����̳ǵĹ�����Ϊ)֮�������Ԫ������;�������ӿ���shop_buy����һ��,����ȫ����Ԫ������;��
//�ֶ����� ���� ����
//amount int ����Ԫ����
//balance int Ԫ�����
//level int ��ǰ�ȼ�
//reason varchar(255) -����������
//-������ȴ
//�ɰ�����Ϸ���������д���г�ȫ����Ԫ����
//�˹����̳ǵ���֮�����������;��������
//reason���ձ�������
//type int 0: ��ֵ�õ���Ԫ��
//1: ����;���õ���Ԫ��
//������Ԫ�����ĵ�;����shop_buy(�û����̳ǵĹ�����Ϊ)+yb_expend ������Ԫ�����ѣ�����һ��
//type����Ϸ�̼�¼��������������ʵ��ֵԪ�����ĺͶ���Ԫ�������ģ���һ�������õ��˳�ֵԪ���Ͷ���Ԫ�������¼����
//Tips: reason ��дԪ������ԭ���Ӧ��id�б�ͬʱ��Ҫ��GM����ԭ��id��ԭ�����ƵĶ�Ӧ��ϵ��GM�ɶ�����ʾʹ�ã���
// done
// note: reason ���չ�������
void oss_360_log_yb_expend(uint32_t amount, uint32_t balance, uint32_t reason, uint32_t type,  LC_ServerPlayer* pPlayer);

//��¼�����������
//��ע����¼������shop_buy ���û����̳ǵĹ�����Ϊ��֮�⣬���RMB���ߵ�����;�������ӿ���shop_buy����һ���¼�����е��ߵ���Դ
//�ֶ����� ���� ����
//itemid varchar(255) ��ƷID
//amount int ��������
//level int ��ǰ�ȼ�
//reason varchar(255) �����
//��ֵ����
//����
//������Ϸ���������ʵ�ʵ��ߵĵõ������д������reason���ձ�������
//mode varchar(255) ��ʶ�õ����Ƿ�Ϊ��
//u �ǰ󶨵���
//b �󶨵���
//remain int �����������ʣ������
//�������̳ǹ���shop_buy��+���ߵĶ�������(item_income)=����ʹ��(item_use)+����ʣ��(item_use��remain�ֶΡ�
//Tips: reason ��д��ȡԭ���Ӧ��id�б�ͬʱ��Ҫ��GM����ԭ��id��ԭ�����ƵĶ�Ӧ��ϵ��GM�ɶ�����ʾʹ�ã���
// todo
void oss_360_log_item_income(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  LC_ServerPlayer* pPlayer);

//��¼�û�ʹ����Ʒ����Ϊ
//�ֶ����� ���� ����
//itemid varchar(255) ��ƷID
//amount int ʹ������
//remain int ʹ�ú�ʣ������
//type varchar(255) ���� 1 ������ 2 ��������
//level int ��ǰ�ȼ�
//map_id varchar(255) ��ͼid
//mode varchar(255) ��ʶ�õ����Ƿ�Ϊ��
//u -�ǰ󶨵���
//b -�󶨵���
//reason varchar(255) ����������
//������ȴ
//�ɰ��ո���Ϸ����ĵ��߱�ʹ�����������������������reason���ձ�
//�������̳ǹ���shop_buy��+���ߵĶ�������(item_income)=����ʹ��(item_use)+����ʣ��(item_use��remain�ֶΡ�
//Tips: reason ��д��ȡԭ���Ӧ��id�б�ͬʱ��Ҫ��GM����ԭ��id��ԭ�����ƵĶ�Ӧ��ϵ��GM�ɶ�����ʾʹ�ã���
// done
void oss_360_log_item_use(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  uint32_t type,  LC_ServerPlayer* pPlayer);



//��¼�û��޸��ǳƵ���Ϊ
// done
void oss_360_log_change_name(LC_ServerPlayer* pPlayer);

//��¼�û���������Ϊ
// done
void oss_360_log_level_up(int level, LC_ServerPlayer* pPlayer );

#endif


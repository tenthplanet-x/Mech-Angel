#ifndef OSS_360_DEFINE_H_
#define OSS_360_DEFINE_H_

#include "oss_log.h"
#include "ajson_mem.hpp"
#include "LC_ServerPlayer.h"

//临时这么写，上班用正式的宏代替
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
//记录首次加载flash之前的新增用户，即：进入游戏主界面，还未建角色的用户。之前已创角则不记录。（无论是否创角成功都要记录）
//字段名称 类型 含义
//channel varchar(255) 渠道号 如取不到，请传“0”.
//自然用户，渠道号为-1
//poster varchar(255) 海报号 如取不到，请传“0”
//site varchar(255) 子站id 如取不到，请传“0”
//ip varchar(255) 用户登录ip
void oss_360_log_game_enter(LC_ServerPlayer* player);

//游戏用户登录服务器 done
void oss_360_log_user_login(LC_ServerPlayer* player);
//游戏用户登出服务器 done
void oss_360_log_user_logout(LC_ServerPlayer* player);
// 统计在线人数 done
void oss_360_log_online_count( uint32_t nCount );
//角色升级 done
void oss_360_log_user_upgrade( LC_ServerPlayer* pPlayer, int nReason );
//角色创建，创角 done
void oss_360_log_create_role( LC_ServerPlayer* pPlayer );
//记录游戏开服前的清档行为
// noimpl
void oss_360_log_retreat( const char* szResult );
// 记录玩家客户端信息；无插件和微端就不记录
// NOTE: 这些信息gs无法取到，建议在平台上报处理，这里仅保留接口，不实现
// noimpl
void oss_360_log_client_info( LC_ServerPlayer* pPlayer );

//记录用户的礼金消耗的行为（包括商城购买和其他非购买但消耗礼金的行为）
//字段名称 类型 含义
//balance int 礼金余额, 用户礼金消耗的行为之后的元宝余额
//itemid varchar(255) 物品ID（道具的模板ID）
//如无购买，请传消耗途径，可按照游戏具体情况填写，列出礼金的非购买型消耗途径
//amount int 购买数量 如无购买，请传“0”
//money int 本次使用的礼金数量
//level int 当前等级
//map_id varchar(255) 地图id
// CT_PENTACLE
// done
void oss_360_log_giftmoney_buy( uint32_t nBanalce, uint32_t nItemId, uint32_t nAmount, uint32_t nMoney, LC_ServerPlayer* pPlayer );

//记录星券获得的行为
//字段名称 类型 含义
//money int 获得的礼金数量, 星券获得行为之后的余额
//balance int 获得后星券总余额
//reason Varchar(255) 获得原因 请按具体情况
//如
//1、每日登陆
//2、完成任务 
//level int 当前等级
//map_id Varchar(255) 地图id
// done
void oss_360_log_giftmoney_income( uint32_t money, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer );

//记录星券消耗的行为
//字段名称 类型 含义
//money int 获得的礼金数量, 星券消耗行为之后的余额
//balance int 获得后星券总余额
//reason Varchar(255) 获得原因 请按具体情况
//如
//1、每日登陆
//2、完成任务 
//level int 当前等级
// done
void oss_360_log_giftmoney_expend( uint32_t money, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer );

//记录用户金币所得行为 记录数量较大的金币所得
//字段名称 类型 含义
//num int 数量（获得金币数值为正常记录，例如20000。消耗金币数值为标记为负，例如-20000）。数值大于10000的记录
//status varchar(255) 产生原因 
//1、捡到 2、打怪 3、做任务（包括副本） 4、跟人交易 5、npc出售 6、其他
//map_id varchar(255) 前往地图id
//balance int 金币余量，消耗金币或获取金币后的金币的数量
//type int 记录金币的类型，1：绑定金币；0：非绑定金币
//Tips: status 填写获取原因对应的id列表，同时需要给GM返回原因id和原因名称的对应关系（GM可读性显示使用）。
// done
void oss_360_log_gold(uint32_t num, uint32_t status, uint32_t balance, uint32_t type, LC_ServerPlayer* pPlayer);

//记录用户特权变化
//字段名称 类型 含义
//nAddVipLevel int 增加特权等级
//viplevel  int 当前特权等级
//level int 当前等级
// done
void oss_360_log_vip_privilege(uint32_t nAddVipLevel, LC_ServerPlayer* pPlayer);

//记录用户在游戏内金币消耗行为
//备注：当该消耗行为是与NPC 发生的交易时，下列字段itemid及amount不能为空，当该消耗行为是其他行为时，itemid及amount可为空，根据
//具体情况而定
//字段名称 类型 含义
//balance int 金币余额，游戏内金币消耗行为之后的金币余额
//itemid varchar(255) 物品ID(道具物品的模板ID)
//amount int 购买数量
//money int 本次消耗的金币数量
//level int 当前等级
//map_id varchar(255) 地图id
// done
void oss_360_log_game_buy(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, LC_ServerPlayer* pPlayer);

//记录用户在游戏内出售行为（指与 NPC 发生的交易行为）
//字段名称 类型 含义
//balance int 金币余额，游戏内出售行为之后的金币余额
//itemid varchar(255) 物品ID
//amount int 出售数量
//money int 本次出售获得的金币
//level int 当前等级
//map_id varchar(255) 地图id
// done
void oss_360_log_game_sell(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, LC_ServerPlayer* pPlayer);


//记录道具的所有行为
//备注：
//此接口为统计道具所有的操作，因此会和前面的shop_buy,item_use等有重复，即一个操作（例如购买），会发送两条数据，一条shop_buy,一条
//item_log
//游戏方需额外提供操作类型id与操作类型的描述对应表，例如 1-掉落，2-购买，3-使用等
//满足运营需要的同时兼顾到数据量的大小，建议只记录重要道具的日志。(不重要的药水等小的物品的日志强烈建议不做记录)
//字段名称 类型 含义
//itemid varchar(255) 物品ID
//opid varchar(255) 操作类型id
// 游戏方需额外提供操作类型id与操作类型的描述对应表，例如 1-掉落，2-购买，3-使用等
//amount int 操作物品数量（正数表示获得，负数表示失去）
// done
// note: opid 参照公共字段
void oss_360_log_item_log(uint32_t itemid, uint32_t opid, uint32_t amount, LC_ServerPlayer* pPlayer );

//记录用户完成任务的行为
//字段名称 类型 含义
//taskid varchar(255) 任务ID
//map_id varchar(255) 地图id
//result int 0 完成
//1 放弃
//2 接受
//2.2.23. dupl
//记录用户打副本的行为
//TIPS
//result 填写 数字 0,1，或者2 ，不要写中文。不然影响到任务相关的统计计算。
// done
void oss_360_log_task(uint32_t taskid, uint32_t result, LC_ServerPlayer* pPlayer );

//记录用户打副本的行为
//TIPS
//result 填写 数字 0,1，或者2 ，不要写中文。不然影响到任务相关的统计计算。
//字段名称 类型 含义
//group_id int 分组标识符
//type int 副本类型
//0 单人
//1 组队
//result int 副本结果
//0 未进入
//1 获胜
//2 失败
//level int 当前等级
//map_id varchar(255) 前往地图id
//dlevel int 代表副本的难度等级
//dscore varchar(16) 副本的评分（比如SSS,SS,S,A等）
//Tip: 分组标示符为组队副本时，对应的组队的id
// done
void oss_360_log_dupl(uint32_t group_id, int type, uint32_t result, uint32_t dlevel, uint32_t dscore, LC_ServerPlayer* pPlayer );

//vip日志
//备注：
//此接口为vip相关操作,日志中中文请做urlencode处理
//字段名称 类型 含义
//optype int 1 开通
//2 续费
//3 升级
//viptype varchar 黄金 钻石 白银
//urlencode后传这三个之一
//buytime varchar 有效期，例如： 10天
//需要urlencode
//viplv int vip当前等级
// done
void oss_360_log_vip(uint32_t optype, uint32_t viptype, uint32_t buytime, uint32_t viplv, LC_ServerPlayer* pPlayer );

//签到日志
//备注：
//字段名称 类型 含义
//type int 0 签到
//1 补签
// done
void oss_360_log_sign(uint32_t type, LC_ServerPlayer* pPlayer);

//记录装备的产出
//备注：
//此接口为装备得
//字段名称 类型 含义
//equipid varchar(255) 装备ID（guid）
//itemid varchar(255) 道具id(物品ID)
//opid varchar(255) 操作类型id 产生 流转 销毁等，需要id列表
//amount int 装备数量
// todo
void oss_360_log_equip(uint32_t equipid, uint32_t itemid, uint32_t opid, uint32_t amount, LC_ServerPlayer* pPlayer );

//记录用户切换地图的行为
//字段名称 类型 含义
//level int 当前等级
//nonce_map varchar(255) 当前地图id
//to_map varchar(255) 前往地图id
// done
void oss_360_log_map( uint32_t level, uint32_t nonce_map, uint32_t to_map, LC_ServerPlayer* pPlayer );

//记录用户的战斗力日志
//字段名称 类型 含义
//before int 消耗或增加前的战力值
//amount int 该变量，表示战力的变化值。amount 为正表示战力增加，为负表示战力减少
//after int 消耗或增加后的战力值
//type int 战力消耗类型的id,(需要给建春提供类型id和类型名的对应关系)
// noimpl: not such logic
void oss_360_log_warpot(uint32_t before, uint32_t amount, uint32_t after, uint32_t type, LC_ServerPlayer* pPlayer);

//recharge
//记录用户充值行为
//字段名称 类型 含义
//amount int 充值元宝数
//money int 充值RMB,元为单位
//balance int 元宝余额。记录充值之后的余额
//level int 当前等级
//order varchar(255) 运营订单号
//channel varchar(255) 正常为空，虚拟充值则填: GM
//legion varchar(255) 公会ID，不存在时为空
// done
void oss_360_log_recharge(uint32_t amount, uint32_t money, uint32_t balance, const char*  order, const char* channel, LC_ServerPlayer* pPlayer);

//记录额外元宝收入
//备注：所有除了通过recharge充值行为获得的元宝，都记录在本接口；此接口与recharge充值接口合在一起，就是全部的元宝收入途径
//字段名称 类型 含义
//amount int 收入元宝数
//balance int 礼金余额。额外礼金收入之后的礼金余额
//level int 当前等级
//reason varchar(255) 活动赠送
//充值赠送
//请按照游戏实际情况 补全所有情况，并把reason对照表返给我们
//Tips: reason 填写获取原因对应的id列表，同时需要给GM返回原因id和原因名称的对应关系（GM可读性显示使用）。
// done
// note: reason 参照公共字段
void oss_360_log_yb_income(uint32_t amount, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer);

//记录用户在商城的购买行为
//备注：游戏方需额外提供物品id与物品描述、价格等属性的对应表
//字段名称 类型 含义
//balance int 元宝余额，用户在商城购买道具之后的元宝余额
//itemid varchar(255) 物品ID（这里是道具的模板ID）
//amount int 购买数量
//money int 本次购买消耗元宝数量
//level int 当前等级
//remain int 剩余数量
//map_id varchar(255) 地图id
//type int 0: 充值得到的元宝
//1: 其他途径得到的元宝
//【所有元宝消耗的途径，shop_buy(用户在商城的购买行为)+yb_expend （元宝的额外消费）合在一起；
//道具商城购买（shop_buy）+道具的额外收入(item_income)=道具使用(item_use)+道具剩余(item_use的remain字段 )
//type视游戏商记录状况定，区分真实充值元宝消耗和额外元宝的消耗，如买一个物品用到了充值元宝和额外元宝，则记录两条
// done
// act shop?
void oss_360_log_shop_buy(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, uint32_t remain, uint32_t type, LC_ServerPlayer* pPlayer);

//元宝的额外消费
//备注：记录除了shop_buy(用户在商城的购买行为)之外的所有元宝消耗途径，本接口与shop_buy合在一起,就是全部的元宝消费途径
//字段名称 类型 含义
//amount int 消费元宝数
//balance int 元宝余额
//level int 当前等级
//reason varchar(255) -背包开格子
//-经脉冷却
//可按照游戏具体情况填写，列出全部的元宝除
//了购买商城道具之外的所有消耗途径，并把
//reason对照表返给我们
//type int 0: 充值得到的元宝
//1: 其他途径得到的元宝
//【所有元宝消耗的途径，shop_buy(用户在商城的购买行为)+yb_expend （额外元宝消费）合在一起】
//type视游戏商记录能力定，区分真实充值元宝消耗和额外元宝的消耗，如一个操作用到了充值元宝和额外元宝，则记录两条
//Tips: reason 填写元宝消费原因对应的id列表，同时需要给GM返回原因id和原因名称的对应关系（GM可读性显示使用）。
// done
// note: reason 参照公共参数
void oss_360_log_yb_expend(uint32_t amount, uint32_t balance, uint32_t reason, uint32_t type,  LC_ServerPlayer* pPlayer);

//记录额外道具收入
//备注：记录除了在shop_buy （用户在商城的购买行为）之外，获得RMB道具的所有途径，本接口与shop_buy合在一起记录了所有道具的来源
//字段名称 类型 含义
//itemid varchar(255) 物品ID
//amount int 收入数量
//level int 当前等级
//reason varchar(255) 活动赠送
//充值赠送
//……
//按照游戏具体情况按实际道具的得到情况填写，并把reason对照表返给我们
//mode varchar(255) 标识该道具是否为绑定
//u 非绑定道具
//b 绑定道具
//remain int 道具收入后总剩余数量
//【道具商城购买（shop_buy）+道具的额外收入(item_income)=道具使用(item_use)+道具剩余(item_use的remain字段】
//Tips: reason 填写获取原因对应的id列表，同时需要给GM返回原因id和原因名称的对应关系（GM可读性显示使用）。
// todo
void oss_360_log_item_income(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  LC_ServerPlayer* pPlayer);

//记录用户使用物品的行为
//字段名称 类型 含义
//itemid varchar(255) 物品ID
//amount int 使用数量
//remain int 使用后剩余数量
//type varchar(255) 类型 1 可消耗 2 不可消耗
//level int 当前等级
//map_id varchar(255) 地图id
//mode varchar(255) 标识该道具是否为绑定
//u -非绑定道具
//b -绑定道具
//reason varchar(255) 背包开格子
//经脉冷却
//可按照各游戏具体的道具被使用情况列完整，并返给我们reason对照表
//【道具商城购买（shop_buy）+道具的额外收入(item_income)=道具使用(item_use)+道具剩余(item_use的remain字段】
//Tips: reason 填写获取原因对应的id列表，同时需要给GM返回原因id和原因名称的对应关系（GM可读性显示使用）。
// done
void oss_360_log_item_use(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  uint32_t type,  LC_ServerPlayer* pPlayer);



//记录用户修改昵称的行为
// done
void oss_360_log_change_name(LC_ServerPlayer* pPlayer);

//记录用户升级的行为
// done
void oss_360_log_level_up(int level, LC_ServerPlayer* pPlayer );

#endif


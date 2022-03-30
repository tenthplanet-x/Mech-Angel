#ifndef OSS_INTERNAL_DEFINE_H_
#define OSS_INTERNAL_DEFINE_H_

#include "oss_log.h"
#include "ajson_mem.hpp"
#include "LC_ServerPlayer.h"

int get_world_id(LC_ServerPlayer * player);

//游戏用户登录服务器
void oss_internal_log_user_server_login(LC_ServerPlayer* player);

//游戏用户登出
void oss_internal_log_user_server_logout(LC_ServerPlayer* player);

//用户充值
void oss_internal_log_recharge(LC_ServerPlayer* player, int RMB);

//创角
void oss_internal_log_create_role(LC_ServerPlayer* player);

void oss_internal_log_enter_state(LC_ServerPlayer* player);

#endif
#pragma once

class SC_ClientDebugInterface : public Memory::MM_BaseObject
{
public:
	SC_ClientDebugInterface();
	virtual ~SC_ClientDebugInterface();
	DECL_RTTI_NAME( SC_ClientDebugInterface )

	//Client Config
	void SetFogEnable( bool bEnable );
	void SetWaterRenderingEnable( bool bEnable );
	void SetSceneFxRenderingEnable( bool bEnable );
	void SetModelRenderingEnable( bool bEnable );
	void SetActorRenderingEnable( bool bEnable );
	void SetShadowRenderingEnable( bool bEnable );
	void SetTerrainRenderingEnable( bool bEnable );
	void SetGrassRenderingEnable( bool bEnable );
	void SetStatEnable( bool bEnable );
	void SetPathFindingEnable( bool bEnable );
	void SetCharacterUpdateEnable( bool bEnable );
	void SetFreeCameraModeEnalbe(bool bEnable);
	void SetTianlongCameraEnable(bool bEnable);
	bool GetTianlongCameraEnable();
	void SetTrackAnimTrackKey(int trackIdx, int keyIdx);
	int  GetTrackAnimTrackKeyCount(int trackIdx);
	void ModifyTrackAnimKey(int trackIdx, int keyIdx);
	void AddTrackAnimKey(int trackIdx,int keyIdx);
	void RemoveTrackAnimKey(int trackIdx,int keyIdx);
	void SaveTrackAnimToFile();
	void LoadTrackAnimFromFile();
	void SetTrackAnimTimeSpin(int trickIdx, float ti);
	float GetTrackAnimTimeSpin(int trickIdx);
	void SetTargetDebugInfo(bool bEnable);//Ŀ��ͷ�ϵ�����Ϣ�򿪹رգ�ID��
	void SetActorUpdateThread( unsigned int uiCount );
	void SetTempStringDump( bool bEnable , int nCount );
	void SetUseLastUIRender(bool bUse);
    void SetUIVisibility(bool bUIVisible);
	void SetUIDebugMode(bool bDebug);
	//CutScene
	void SaveFbx();
	void PlayCutScene();
	void PauseCutScene();
	void StopCutScene();
	void ReloadSkyDome();
    // Client Debug
	void ShowProfile( const StringType& name , bool bEnable );
	void ReloadLuaFile(const StringType& fileName);

	//��������
	void ChangeVehicle(int iLevel, int lCharTypeID);
	//����ESC����
	void ChangeESCVehicle(int iLevel, StringType sESCModRes);
	//���Խ�ɫ����Avatar
	void DebugCharacterAvatar(int cid, float dist, float x, float z);
	//����Boss��д��ͷ����
	void DebugBossDetailCamera(const StringType& sParams);
	//�޸ľ�ͷ
	void ChangeUIVehicleCamera(int charType, float dist);
	//�л���ս����
	void ChangeVehicleWeapon(int iLevel, int lItemID);

	int  GetBackPackTypeID();
	void SendToDebugConsole(const StringType &strTemp);
	void LogError(const StringType &strError);

	//CommandLine
	//AI
	StringType GetAiInformation();
	void SetAIStatus();

	//SKILL
	StringType GetSkillInfo();
	void SetSkillStatus();

	StringType FindOwerType(object_id_type nID);
	StringType GetAniName(object_id_type nID, const StringType &strOwerType);

	void	SendDebugCommandToGameServer(const StringType& rkCommand);
	void    TestEffentForRepeat();
	//ReloadUIScript
	void ReloadUIScript();

	//����load csv file
	void ReloadCSV(const StringType& rkCSVFile);

	//OpenMapList
	void OpenMapList();

	//ReloadSkillScript
	void ReloadSkillScript();
	//��ȡ����UIͼƬ�ڴ�����С
	int GetAllUIImgAllocSize();

	//����
	void SetCurrentLocation(int iMapLogicID,int nLoactionX, int nLoactionY);

	//�߼�����
	void ChangeCurrentLocation(int iMapInOutType,int iMapLogicType,int iMapLogicID,int iMapResID,int iX,int iY);

	//��ת������ֱ�Ӵ���
	void SetCurrentLocationDirectly(int iMapLogicID,int nLoactionX, int nLoactionY);

	//AcceptTask
	bool AcceptTask(long lTaskID);

	//FinishTask
	bool FinishTask(long lTaskID);

	//DeleteTask
	bool DeleteTask(long lTaskID);

	void SaveActiveMemLog();

	//ItemEnhance
	bool ItemEnhance(long lKeepsakeID, int nItemEntryIndex);

	void Transform(long lCharTypeID);

	//��޵�����Ϣ
	void ReqActorAIInfo(long lActorID);

	//���ùһ��ű�
	void ReloadControlScript(bool bUseScript);

	//�������𲥷�ʱ�����
	void SetSupremacyGfxPlayInterval(float fTime);

	//���ƽ̨�ϱ�
	void SignUpTeamPlatform(long lMapLogicType);

    void LogCurrentFxList(void);

	//
	void SetHooking(bool bFlag);

	void SetHookingParam(const StringType &info, long lParam);

	void ParseCommand(const StringType &commandLine, uint32_t uiPri);

	StringType GuessCommand(StringType cmdchip);
	StringType GuessCmdFile(StringType filechip);

	StringType MD5String(const StringType& src);

	void GetMyPos();

	void DumpScore();

	void GetACInfo();

	void GetAllUnits();

	void GetAllFX();

	void WingRot(float x, float y, float z);
	void WingXYZ(float x, float y, float z);
	void WingLevel(int llevel);

	void SetSkillDeadForce(int charType, int hForce, int VForce);


	void SetCameraType(int type);

	void FBUpdate(int lLevel);

	void LCUpdate(int lLevel);

	void LCWeaponUpdate(int lLevel);

	void LCFollowLoad();   

	void AQUpdate(int iLevel);

	void SetCounter(int flag);
	void SetSuiteCounter(int flag);

	StringType GetServerVersionInfo();
};
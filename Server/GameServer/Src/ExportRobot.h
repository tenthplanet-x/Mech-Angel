/********************************************************************
	created:	2014/10/20
	filename: 	ExportRobot.h
	file path:	GameServer\Src
	file base:	ExportRobot
	file ext:	h
	
	purpose:导入机器人
*********************************************************************/
#ifndef _EXPORT_ROBOT_H
#define _EXPORT_ROBOT_H

class ExportRobot:public  Memory::MM_BaseObject,public Utility::UT_Singleton<ExportRobot>
{
public:
	ExportRobot():m_Operator(0), m_Step(0), m_StartTime(0), m_nServerID(0){}
	~ExportRobot(){}

	void Reset(void);

	void Update(float fCurrentTime);
	
	void Export2LoginSvrd(int beginindex);
	void Export2LoginSvrdResp(int32_t result,int beginindex,std::vector<std::string>& name);
	
	void ClearAllPlayer();
	void ExportResult(bool result);
	void ExportStep();

	void InitExportRobotToGroup(int32_t exportType, int32_t robotType);

	inline void SetOperator(const unique_id_impl& Operator){m_Operator=Operator;}

private:
	void Export2GroupSvrd(int32_t exportType, uint32_t robotType, mem::vector<uint32_t>& robotID);
private:
	unique_id_impl m_Operator;
	int m_Step;
	float m_StartTime;
	int m_nServerID;
};

#endif
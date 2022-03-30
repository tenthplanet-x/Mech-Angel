
inline void TS_AtomOperate::SetResultID(uint32_t iErrorID)
{
	m_iResultID = iErrorID;
}

inline void TS_AtomOperate::SetUserID(uint32_t iUserID)
{
	m_uiUserID = iUserID;
}

inline void TS_AtomOperate::SetCharID(const unique_id_impl& iCharID)
{
	m_uiCharID = iCharID;
}

inline void TS_AtomOperate::SetContentFlag(bool bFlag)
{
	m_bContentFlag = bFlag;
}

inline uint32_t TS_AtomOperate::GetResultID() const
{
	return m_iResultID;
}

inline uint32_t TS_AtomOperate::GetUserID() const
{
	return m_uiUserID;
}

inline const unique_id_impl& TS_AtomOperate::GetCharID() const
{
	return m_uiCharID;
}

inline bool TS_AtomOperate::GetContentFlag() const
{
	return m_bContentFlag;
}

//////////////////////////////////////////////////////////////////////////

inline void TS_TranBase::SetID(uint32_t uiID)
{
	m_uiID = uiID;
}

inline void TS_TranBase::SetStep(uint32_t uiStep)
{
	m_uiStep = uiStep;
}

inline uint32_t TS_TranBase::GetID() const
{
	return m_uiID;
}

inline uint32_t TS_TranBase::GetStep() const
{
	return m_uiStep;
}

inline void TS_TranBase::RegPostCommit(TS_PostCommit* pkPostCommit)
{
	m_pPostCommit = pkPostCommit;
}
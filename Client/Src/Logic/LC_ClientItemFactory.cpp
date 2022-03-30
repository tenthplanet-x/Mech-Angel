#include "ClientPCH.h"
#include "LC_ClientItemFactory.h"
#include "LC_ClientItem.h"

#include "CF_ItemList.h"

using namespace GameLogic;
using namespace CSVFile;
//------------------------------------------------------
LC_ClientItemFactory::LC_ClientItemFactory()
: LC_ItemFactoryBase()
{
	
}
//------------------------------------------------------
LC_ClientItemFactory::~LC_ClientItemFactory()
{
	m_kActiveSkillItemIDs.clear();
}
//------------------------------------------------------
bool LC_ClientItemFactory::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	LC_ItemFactoryBase::Init(pkLoader, pkCSVs);
	
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntryMapExternIterator kIter = pkItemList->GetIterator();
	
	while(kIter.HasMoreElements())
	{
		const CF_ItemList::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		long lItemID = pkEntry->_iID;
		LC_ClientItem* pkItem = T_NEW_D LC_ClientItem;
		pkItem->SetItemTypeID(lItemID);
		pkItem->InitPropertyFromCSV(pkEntry);

		m_kItemIDPtrMap[lItemID] = pkItem;

		if(pkEntry->_lActivateSkillID > 0)
			m_kActiveSkillItemIDs.push_back(lItemID);

		kIter.MoveNext();
	}

	return true;
}
//------------------------------------------------------
bool LC_ClientItemFactory::UnInit()
{
	LC_ItemFactoryBase::UnInit();

	
	LC_ItemIDPtrMapIter Iter;
	TRAV(m_kItemIDPtrMap,Iter)
	{
		LC_ItemBase* pkItem = Iter->second;
		T_SAFE_DELETE(pkItem);
	}
	return true;
}

int LC_ClientItemFactory::GetActiveSkillItemID(int iSkillID)
{
	for(size_t i=0; i<m_kActiveSkillItemIDs.size(); ++i)
	{
		const CF_ItemList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(m_kActiveSkillItemIDs[i]);
		if(pkEntry != NULL && pkEntry->_lActivateSkillID == iSkillID)
			return m_kActiveSkillItemIDs[i];
	}

	return 0;
}
#pragma once

#include "GD_ModelBase.h"
#include "GD_ModelContentManager.h"
#include "GD_MaterialManager.h"

//场景物件模型
class GD_ClientBreakModel : public Data::GD_ModelBase
{
public:
	GD_ClientBreakModel(void);

protected:
	bool _doLoad( const StringType& strModFileName, int iActiveModelIndex );
	void         _updateActorShader();

private:
	virtual Data::ModelContentType _doGetModelContentType(void) { return Data::MCT_CHARACTER; }
	virtual Data::GD_MaterialManager::MaterialType _doGetMaterialType(void) { return Data::GD_MaterialManager::MT_CHARACTER; }
};


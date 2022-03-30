
#include "ClientPCH.h"
#include "GD_ClientBreakModel.h"
#include "PF_GroundMeshConverter.h"


GD_ClientBreakModel::GD_ClientBreakModel(void)
: GD_ModelBase(false, false)
{
}


bool GD_ClientBreakModel::_doLoad(const StringType& strModFileName, int iActiveModelIndex)
{
	if (!GD_ModelBase::_doLoad(strModFileName, iActiveModelIndex))
	{
		return false;
	}
	_updateActorShader();
	return true;
}

void GD_ClientBreakModel::_updateActorShader()
{
	NiShaderPtr spSceneShader = GfxGetWorldConfig().GetNPCWeaponShader(false);
	GfxSetShader(m_spNiRootNode, spSceneShader, NULL, false , false);
}


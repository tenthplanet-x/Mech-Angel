#ifndef _LC_VISIBLE_OBJECT_INTERFACE_H
#define _LC_VISIBLE_OBJECT_INTERFACE_H


#include "GD_ClientActorModel.h"

class LC_VisibleObjectInterface
{
public:
		
	virtual void					CreateVisibleData() = 0;
	virtual void					DestoryVisibleData() = 0;
	virtual Utility::UT_Vec3Int		GetVisibleLocation() = 0;
	virtual void					SetVisibleLocation(const Utility::UT_Vec3Int& rkVisibleLocation) = 0;
	virtual Base::BS_IMInterface*	GetVisibleData() = 0;
	virtual void	 			    SetVisibleData(Base::BS_IMInterface* pkVisibleData) = 0;
	virtual void					PlayAnimation(const StringType& rkAnim,bool bForce = false,float fSequency = 1.0f) = 0;
};

#endif
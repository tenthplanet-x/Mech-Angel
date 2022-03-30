#pragma once


namespace Utility
{
	class UT_Vec3Int;
}
class NiPoint3;


namespace CA_CameraHelp
{
	namespace GamePlayer
	{
		bool			GetPosition(Utility::UT_Vec3Int & pos);
		bool			GetNiPosition(NiPoint3 & pos);
		bool			SetAIControl(bool b);
	};
};

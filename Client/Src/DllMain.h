#pragma once

#ifdef CLIENT_DLL

extern "C"
{
	void __declspec( dllexport ) jzldCreate( void* window, unsigned long mode );
	void __declspec( dllexport ) jzldDestroy( );
	void __declspec( dllexport ) jzldSetup( const wchar_t* name, const wchar_t* value );
	void __declspec( dllexport ) jzldUpdate( unsigned long elapse );
	void __declspec( dllexport ) jzldRender( );
	unsigned int __declspec( dllexport )  jzldMessage( unsigned long msgid, unsigned long wparam, unsigned long lparam );
	float __declspec( dllexport ) jzldGetProgress( );
}

#endif
#include "tc_interface.h"


#ifdef USE_TC_MALLOC

void tcmalloc_clearcache()
{
	MallocExtension::instance()->ReleaseFreeMemory();
}

#endif

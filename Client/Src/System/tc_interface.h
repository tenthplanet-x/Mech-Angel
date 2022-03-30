#pragma once


#ifdef USE_TC_MALLOC

#	include "google/tcmalloc.h"
#	include "gperftools/malloc_extension.h"


void tcmalloc_clearcache();

#endif

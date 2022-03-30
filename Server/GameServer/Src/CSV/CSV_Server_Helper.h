#ifndef CSV_SERVER_HELPER_H_DFGD23535
#define CSV_SERVER_HELPER_H_DFGD23535

#include "CSVDataSource.h"
#include "CSVSchedule.h"

bool Init_Server_CSV_File();
void UnInit_Server_CSV_File();
bool Reload_Server_CSV_File(CSVFile::CSVDataLoader* pkReloadOP);

#endif

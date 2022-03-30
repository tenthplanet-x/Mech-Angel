#ifndef _BS_CLIENT_MODEL_CONTENT_MANAGER_H
#define _BS_CLIENT_MODEL_CONTENT_MANAGER_H

#include "GD_ModelContent.h"
#include "GD_ModelContentManager.h"

class BS_ClientModelContentManager : public Data::GD_ModelContentManager
{
private:
    virtual bool _doGetMainThreadPermission(void);
};

#endif // _BS_CLIENT_MODEL_CONTENT_MANAGER_H

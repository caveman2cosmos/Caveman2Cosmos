#include "BuildingInfo.h"
#include "../Interface/IBuildingInfo.h"


void __RPC_USER hIBuildingInfo_t_rundown(hIBuildingInfo_t hBuilding)
{
   IBuildingInfo_delete(hBuilding);
}


hIBuildingInfo_t IBuildingInfo_create(handle_t)
{
	BuildingInfo* pContext = new BuildingInfo();
	hIBuildingInfo_t hContext = pContext;
	return hContext;
}

void IBuildingInfo_delete(hIBuildingInfo_t hBuilding)
{
	delete static_cast<BuildingInfo*>(hBuilding);
}


boolean IBuildingInfo_isNoLimit(hIBuildingInfo_t hBuilding)
{
	return static_cast<BuildingInfo*>(hBuilding)->isNoLimit();
}

void IBuildingInfo_setNoLimit(hIBuildingInfo_t hBuilding, boolean b)
{
	static_cast<BuildingInfo*>(hBuilding)->setNoLimit(b);
}



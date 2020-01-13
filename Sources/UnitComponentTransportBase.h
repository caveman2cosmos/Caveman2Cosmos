#pragma once

#include "UnitComponentBase.h"

struct UnitComponentTransportBase : UnitComponentBase
{
	SpecialUnitTypes specialCargo() const; // Exposed to Python
	SpecialUnitTypes SMspecialCargo() const;
	SpecialUnitTypes SMnotSpecialCargo() const;
	DomainTypes domainCargo() const; // Exposed to Python

	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const; // Exposed to Python
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const; // Exposed to Python
	int SMcargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const; // Exposed to Python
	bool canCargoAllMove() const; // Exposed to Python
	bool canCargoEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage) const;
	int getUnitAICargo(UnitAITypes eUnitAI) const; // Exposed to Python

	int getCargo() const;
	int SMgetCargo() const; // Exposed to Python					
	void changeCargo(int iChange); // Exposed to Python					
	void SMchangeCargo(int iChange);
	void getCargoUnits(std::vector<CvUnit*>& aUnits) const;
	void validateCargoUnits();

	DomainTypes getDomainCargo() const;
	void setNewDomainCargo(DomainTypes eDomain);
	SpecialUnitTypes getSpecialCargo() const;
	void setNewSpecialCargo(SpecialUnitTypes eSpecialUnit);
	SpecialUnitTypes getSMSpecialCargo() const;
	void setNewSMSpecialCargo(SpecialUnitTypes eSpecialUnit);
	SpecialUnitTypes getSMNotSpecialCargo() const;
	void setNewSMNotSpecialCargo(SpecialUnitTypes eSpecialUnit);

	void changeSMCargoSpace(int iChange);
	int SMcargoSpace() const;
	int SMcargoSpaceFilter() const;
	int SMcargoCapacityPreCheck() const;
	int getSMCargoCapacity() const;
	void setSMCargoCapacity();
	
	int getCargoCapacitybyType(int iValue) const;

private:
	DomainTypes m_eNewDomainCargo;
	SpecialUnitTypes m_eNewSpecialCargo;
	SpecialUnitTypes m_eNewSMSpecialCargo;
	SpecialUnitTypes m_eNewSMNotSpecialCargo;

	int m_iCargo;
	int m_iCargoCapacity;
	int m_iSMCargo;
	int m_iSMCargoCapacity;
};

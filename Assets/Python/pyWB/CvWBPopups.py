## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import Popup as PyPopup

GC = CyGlobalContext()

# I don't think this class is used by the game, will have to look into why or possibly where it is used.
class CvWBPopups:
	"World Builder App Popups"

	# These are popup types, not popup context IDs
	WBPopupEditCity		= WorldBuilderPopupTypes.WBPOPUP_CITY
	WBPopupEditUnit		= WorldBuilderPopupTypes.WBPOPUP_UNIT
	WBPopupEditPlot		= WorldBuilderPopupTypes.WBPOPUP_PLOT
	WBPopupEditPlayer	= WorldBuilderPopupTypes.WBPOPUP_PLAYER

	def __init__(self):
		self.WBPopupDictionary = {self.WBPopupEditCity: {'Init': self.initEditCity, 'Apply': self.applyEditCity}}

	def initWB(self, argsList):
		"called from Interface - use the PopupDictionary for quick access"
		context = argsList[0]
		entry = self.WBPopupDictionary.get(context)

		if entry:
			init = entry.get('Init')
			return init(argsList)
		return 0

	def applyWB(self, argsList):
		"Apply the appropriate Popup using the PopupDictionary"
		context = argsList[0]
		entry = self.WBPopupDictionary.get(context - 200 + WorldBuilderPopupTypes.WBPOPUP_START)

		if entry:
			apply = entry.get('Apply')
			return apply(argsList)
		return 0


	############################################
	# Initializes World Builder City Edit Popup
	def initEditCity(self, argsList):
		local = CyTranslator()
		px, py = argsList
		CyCity = CyMap().plot(px,py).getPlotCity()
		iOwner = CyCity.getOwner()
		iID = CyCity.getID()
		userData = (iOwner, iID)

		# create popup
		popup = PyPopup.PyPopup(5001, EventContextTypes.EVENTCONTEXT_ALL)
		popup.setSize(600, 600)
		popup.setPosition(16, 64)
		popup.setUserData(userData)
		popup.setHeaderString(local.getText("TXT_KEY_WB_HEADER_CITY_EDIT", ()))
		# City Name - Editbox 0
		popup.createEditBox(CyCity.getName(), 0)
		# Population - Editbox 1
		popup.createEditBox('0', 1)
		popup.setBodyString(local.getText("TXT_KEY_WB_CITY_EDIT_POP", ()))
		# Culture - Editbox 2
		popup.createEditBox('0', 2)
		popup.setBodyString(local.getText("TXT_KEY_WB_CITY_EDIT_CULTURE", (CyCity.getCulture(iOwner),)))
		# Buildings - Listboxes Group
		popup.createListBox(0)
		iNumBuildings = GC.getNumBuildingInfos()
		lBuildings = []
		for i in range(iNumBuildings):
			# ('Library', iIndex)
			lBuildings.append((GC.getBuildingInfo(i).getDescription(), i))
		lBuildings.sort()

		popup.addListBoxString(local.getText("TXT_KEY_WB_CITY_NOTHING", ()), -1)
		popup.addListBoxString(local.getText("TXT_KEY_WB_CITY_ALL", ()), iNumBuildings) #for adding/removing every building
		for i in range(len(lBuildings)):
			entry = lBuildings[i]
			popup.addListBoxString(entry[0], entry[1])

		# Add/Remove - Pulldown Group 0
		popup.createPullDown(0)
		popup.addPullDownString(local.getText("TXT_KEY_WB_CITY_ADD", ()), 1)
		popup.addPullDownString(local.getText("TXT_KEY_WB_CITY_NOCHANGE", ()), 0) #for clean exit
		popup.addPullDownString(local.getText("TXT_KEY_WB_CITY_REMOVE", ()), 2)
		popup.setBodyString(local.getText("TXT_KEY_WB_CITY_ADD_REMOVE", ()))

		# Launch Popup
		popup.launch()
		return 0

	def applyEditCity(self, argsList):
		popupReturn, userData = argsList
		iOwner, iID = userData
		CyCity = GC.getPlayer(iOwner).getCity(iID)

		if CyCity.isNone():
			return 0
		# Name
		newName = str(popupReturn.getEditBoxString(0))
		currName = CyCity.getName()
		if newName != currName:
			CyCity.setName(newName, False)

		# Population
		PopModifier = int(popupReturn.getEditBoxString(1))
		if PopModifier:
			CyCity.changePopulation(PopModifier)

		iCultureModifier = int(popupReturn.getEditBoxString(2))
		if iCultureModifier:
			CyCity.setCulture(iOwner, iCultureModifier, True)

		# Buildings
		iNumBuildings = GC.getNumBuildingInfos()
		if popupReturn.getSelectedListBoxValue(0) >= 0 and popupReturn.getSelectedPullDownValue(0) > 0:
			iBuilding = popupReturn.getSelectedListBoxValue(0)
			if iBuilding == iNumBuildings + 1:
				return 0
			AddRemoveIdx = popupReturn.getSelectedPullDownValue(0)
			# Toggles buildings in cities
			add = AddRemoveIdx == 1
			if iBuilding != -1:
				if iBuilding == iNumBuildings:
					for i in range(iBuilding):
						CyCity.setNumRealBuilding(i, add)
				else:
					CyCity.setNumRealBuilding(iBuilding, add)
		return 0
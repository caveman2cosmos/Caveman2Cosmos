from CvPythonExtensions import CyGlobalContext
import BugOptionsTab

class GameOptionsTab(BugOptionsTab.BugOptionsTab):

	def __init__(self):
		BugOptionsTab.BugOptionsTab.__init__(self, "GameOptions", "Game Options")

	def create(self, screen):
		GC = CyGlobalContext()
		GAME = GC.getGame()
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)
		left, right = self.addTwoColumnLayout(screen, column, "Options", True)
		column = left
		for i in range(GC.getNumGameOptionInfos()):
			info = GC.getGameOptionInfo(i)
			szType = info.getType()
			control = szType + "Check"
			screen.attachCheckBox(column, control, info.getDescription(), "BUG_OptionsCB_IF", "handleGameOptionChange", szType, GAME.isOption(i))
			screen.setToolTip(control, info.getHelp())
			if column is left:
				column = right
			else:
				column = left

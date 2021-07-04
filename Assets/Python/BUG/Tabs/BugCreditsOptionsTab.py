## BugCreditsOptionsTab
##
## Tab for the BUG Credits.
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

credits = [
			"-", "Caveman2Cosmos",
			"StrategyOnly - C2C Mod Author, Prehistoric Era, Galactic Era",
			"Hydromancerx - Building Designer, Artwork",
			"DancingHoskuld - Designer, Subdue Animals, Religions",
			"Koshling - C++ Coding, C2C SDK Designer",
			"alberts2 - C++ Coding, C2C SDK Designer",
			"AIAndy - Property System, SDK Designer",
			"Thunderbrd - Lead Designer, SDK Designer",
			"Yudishtira - Feedback, Unitnologist",
			"Sparth - Graphics Re-Skinner, Culture Units",
			"ori - Designer for Events XML",
			"MrAzure - XML building, Artwork",
			"CIVPlayer8 - Civicologist",
			"Sgtslick - LH Traitologist",
			"rightfuture - Ideas, PR person",
			"Faustmouse - XML Assistant, Feedback",
			"ls612 - Designer, Feedback",
			"Nimek - C2C Combat Mod ideas developer, Feedback",
			"RidetheSpiral23 - C2C Maps, Feedback",
			"DRJ - Ideas, Feedback",
			"Anq - C++ Coding",
			"Toffer90 - Designer, Python, PPIO Modmod",
			"aztur - Ideas, Feedback",
			"dsma_bell - German translator",
			"Harrier123 - C2C GEM Map Team, Feedback",
			"Acularius - C2C GEM Map Team, Feedback",
			"hx2762 - Ideas, Feedback",
			"JosEPh_II - All round Nuisance, Game Play balance",
			"KaTiON_PT - Text and C++ Coding Assistant",
			"magillicutty - Graphics, Ideas, Feedback",
			"makotech222 - Ideas, Feedback",
			"pepper2000 - C2C Late Game Designer",
			"praetyre - Ideas, Feedback",
			"raledon - Ideas, Feedback",
			"raxo2222 - C2C Clean up Specialist",
			"sargonthegreat2 - Ideas, Feedback",
			"-",
			"Civ Fuehrer - Advanced Nukes Mod, Artwork",
			"Vincentz - Trade Improvements, Guilds, Corporations",
			"Afforess - AND Developer, AND Designer, AND Lead Programmer",
			"Zappara - Creator & Lead Design of Rise of Mankind",
			"RevolutionDCM Team - RevolutionDCM Mod",
			"BUG Team - BUG Mod",
			"Better AI Team - Better AI",
			"Dexy - Fixed Borders, Surround And Destroy",
			"KillmePlease - Occasional Promotions, Great Commanders, Retreat",
			"Jean Eclard - Culturally Linked Starts",
			"Kael - Flexible Difficulty, Assimilation, Increasing Difficulty, Challenge Game Options",
			"GeneralStaff - Civic Buildings, Early Buildings",
			"Jooyo - Larger Cities, Sea Tunnels",
			"The HoTK Team - The CAR Mod",
			"Nevermind - 'Mountains Back To Service'",
			"Shqpye - Vicinity Prerequisites",
			"Phunny Pharmer - Realisitic Culture Spread",
			"Mechaerik - Artwork",
			"Hrochland - Artwork",
			"Kathy - Artwork",
			"Nitram15 - Artwork",
			"Arian - Event Images",
			"Walter Hawkwood - Artwork",
			"Darkstyx - Artwork",
			"Vokarya - Ideas, Feedback",
			"12padams - Ideas, Feedback",
			"platyping - Python modcomps",
			"-", "REVOLUTIONDCM_TEAM|RevDCM Team",
			"jdog5000 - Designer, Lead Programmer",
			"glider1 - Producer, Programmer",
			"phungus420 - Coding, testing",
			"Moctezuma - Coding, IDW",
			"Orion Veteran - Coding, Inquisitions",
			"Trojan Sheep - Coding, Super Spies",
			"Dale - Coding, DCM",
			"Dresden - Coding, Testing",
			"-",
			"-", "BUG_TEAM|BUG Team",
			"Alerum68 - Release, Documentation",
			"Cammagno - Documentation",
			"Dresden - Coding, Testing",
			"EmperorFool - Coding, Testing",
			"NikNaks - Graphics",
			"Ruff_Hi - Coding, Testing",
			"-",
			"TRANSLATORS|Translators",
			"Cammagno - Italian (Game and Documentation)",
			"Falc - French (Game)RoM/AND",
			"The Doc - German (Game and Documentation)",
			"Snofru1 - German Revolutions translations",
			"Heini - German Revolutions translations",
			"calvitix - French Translations C2C",
			"Hale_9204 - Italian Translations C2C"
			"ramzay1945 - Russian info",
			"-",
			"MAP_SCRIPTS|Map Scripts",
			"Tamudjin - Scenario loading script fix",
			"Cephalo - PerfectWorld2",
			"LunarMongoose - (PW3 Civ4 port and some SDK code) - PerfectWorld2",
			"Sephi - (some SDK code)",
			"Doug McCreary - SmartMap",
			"LDiCesare - Tectonics",
			"low - Random Map",
			"Nercury - Planet Generator",
			"Ruff_Hi - Ring World",
			"Terkhen - The Minidump mod",

		    ]

class BugCreditsOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG Credits Options Screen Tab"

	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "Credits", "Credits")

	def create(self, screen):
		self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)

		labelNum = 0
		sepNum = 0
		boxNum = 0
		first = True
		for line in credits:
			if line == "-":
				pass
			else:
				pos = line.find(" - ")
				if pos == -1:
					# Header
					if not first:
						label = "CreditsSpacerLabel%d" % labelNum
						screen.attachLabel(column, label, " ")
						labelNum += 1
					else:
						first = False
					pos = line.find("|")
					if pos != -1:
						label = line[:pos]
						text = line[pos+1:]
						self.addLabel(screen, column, label, text)
					else:
						label = "CreditsHeaderLabel%d" % labelNum
						self.addLabel(screen, column, label, line)
					#screen.setLayoutFlag(label, "LAYOUT_CENTER")
					#screen.setLayoutFlag(label, "LAYOUT_SIZE_HPREFERREDEXPANDING")
					labelNum += 1
					screen.attachHSeparator(column, column + "Sep%d" % sepNum)
					sepNum += 1
					box = "CreditsBox%d" % boxNum
					left, right = self.addTwoColumnLayout(screen, column, box)
					screen.setLayoutFlag(box + "HBox", "LAYOUT_CENTER")
					boxNum += 1
				else:
					# Person - Task
					leftLabel = "CreditsLabelLeft%d" % labelNum
					rightLabel = "CreditsLabelRight%d" % labelNum
					leftText = line[:pos] + "   "
					rightText = line[pos+3:]
					screen.attachLabel(left, leftLabel, leftText)
					screen.setLayoutFlag(leftLabel, "LAYOUT_RIGHT")
					#screen.setLayoutFlag(leftLabel, "LAYOUT_SIZE_HPREFERREDEXPANDING")
					screen.attachLabel(right, rightLabel, rightText)
					labelNum += 1

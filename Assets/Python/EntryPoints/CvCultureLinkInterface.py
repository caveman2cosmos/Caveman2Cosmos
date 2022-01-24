from __future__ import division
from CvPythonExtensions import *
from copy import copy, deepcopy
from math import *

DMAX_EARTH = 20038	# max possible distance between two points on earth (WGS-84)

def InitCoordinatesDictionary():
	coords = []
	GC = CyGlobalContext()

	#Ensure that the Civilization exists first

	##Afforess Note to Modders:
	# Asserts are hidden for the following getInfoTypeForString calls
	# because some of these civs may exist in modules/mod mods.

	if (GC.getInfoTypeForString("CIVILIZATION_ARABIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ARABIA", 21.423, 39.826))
	if (GC.getInfoTypeForString("CIVILIZATION_ABORIGINES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ABORIGINES", -25.215, 130.282))
	if (GC.getInfoTypeForString("CIVILIZATION_AUSTRALIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_AUSTRALIA", -25.585, 134.504))
	if (GC.getInfoTypeForString("CIVILIZATION_NEWZEALAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NEWZEALAND", -44.056, 70.354))
	if (GC.getInfoTypeForString("CIVILIZATION_MAORI", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MAORI", -35.442, 173.820))
	if (GC.getInfoTypeForString("CIVILIZATION_POLYNESIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_POLYNESIA", -14.209, -169.553))
	if (GC.getInfoTypeForString("CIVILIZATION_DENE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_DENE", 39.898, 5.040))
	if (GC.getInfoTypeForString("CIVILIZATION_UPAAJUT", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_UPAAJUT", 63.747, -68.522))
	if (GC.getInfoTypeForString("CIVILIZATION_CANADA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CANADA", 45.389, -75.681))
	if (GC.getInfoTypeForString("CIVILIZATION_TILLIKUM", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_TILLIKUM", 53.265, -131.990))
	if (GC.getInfoTypeForString("CIVILIZATION_UNITED_STATES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_UNITED_STATES", 38.895, -77.037))
	if (GC.getInfoTypeForString("CIVILIZATION_CUBA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CUBA", 23.132, -82.367))
	if (GC.getInfoTypeForString("CIVILIZATION_HAITI", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_HAITI", 39.893, 5.035))
	if (GC.getInfoTypeForString("CIVILIZATION_MEXICO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MEXICO", 19.431, -99.133))
	if (GC.getInfoTypeForString("CIVILIZATION_OLMEC", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_OLMEC", 37.680, -122.127))
	if (GC.getInfoTypeForString("CIVILIZATION_PALEO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PALEO", 33.425, -111.937))
	if (GC.getInfoTypeForString("CIVILIZATION_WESTINDIES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_WESTINDIES", 14.525, -75.818))
	if (GC.getInfoTypeForString("CIVILIZATION_CHILE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CHILE", -35.786, -71.674))
	if (GC.getInfoTypeForString("CIVILIZATION_VENEZUELA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_VENEZUELA", 7.665, -66.145))
	if (GC.getInfoTypeForString("CIVILIZATION_BRAZIL", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BRAZIL", -22.908, -43.196))
	if (GC.getInfoTypeForString("CIVILIZATION_HONDURAS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_HONDURAS", 14.975, -86.264))
	if (GC.getInfoTypeForString("CIVILIZATION_BOLIVARIAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BOLIVARIAN", 10.487, -66.886))
	if (GC.getInfoTypeForString("CIVILIZATION_ARGENTINA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ARGENTINA", -34.608, -58.373))
	if (GC.getInfoTypeForString("CIVILIZATION_AZTEC", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_AZTEC", 19.808, -96.917))
	if (GC.getInfoTypeForString("CIVILIZATION_TUPI", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_TUPI", -7.833, -36.366))
	if (GC.getInfoTypeForString("CIVILIZATION_NAZCA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NAZCA", -14.880, -74.994))
	if (GC.getInfoTypeForString("CIVILIZATION_ZAPOTEC", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ZAPOTEC", 17.036, -96.784))
	if (GC.getInfoTypeForString("CIVILIZATION_NATIVE_AMERICA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NATIVE_AMERICA", 39.883, 5.025))
	if (GC.getInfoTypeForString("CIVILIZATION_SIOUX", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SIOUX", 43.75, -102.5))
	if (GC.getInfoTypeForString("CIVILIZATION_LOWER_SIOUX", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_LOWER_SIOUX", 44.550, -94.987))
	if (GC.getInfoTypeForString("CIVILIZATION_AMERICAN_CONFEDERACY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_AMERICAN_CONFEDERACY", 37.540, -77.433))
	if (GC.getInfoTypeForString("CIVILIZATION_CHIPPEWA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CHIPPEWA", 46.759, -92.613))
	if (GC.getInfoTypeForString("CIVILIZATION_GREAT_PLAINS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GREAT_PLAINS", 47.086, -109.282))
	if (GC.getInfoTypeForString("CIVILIZATION_MIDWEST", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MIDWEST", 41.884, -87.632))
	if (GC.getInfoTypeForString("CIVILIZATION_IROQUOIS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_IROQUOIS", 43.080, -79.071))
	if (GC.getInfoTypeForString("CIVILIZATION_INCA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_INCA", -13.508, -71.972))
	if (GC.getInfoTypeForString("CIVILIZATION_MAYA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MAYA", 14.632, -90.549))
	if (GC.getInfoTypeForString("CIVILIZATION_JIVARO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_JIVARO", 20.468, -89.213))
	if (GC.getInfoTypeForString("CIVILIZATION_APACHE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_APACHE", 32.769, -108.275))
	if (GC.getInfoTypeForString("CIVILIZATION_CHEROKEE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CHEROKEE", 34.679, -83.325))
	if (GC.getInfoTypeForString("CIVILIZATION_NAVAJO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NAVAJO", 35.399, -110.321))
	if (GC.getInfoTypeForString("CIVILIZATION_ANASAZI", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ANASAZI", 36.060, -107.970))
	if (GC.getInfoTypeForString("CIVILIZATION_COMANCHE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_COMANCHE", 34.605, -98.389))
	if (GC.getInfoTypeForString("CIVILIZATION_PIRATES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PIRATES", 17.938, -76.840))
	if (GC.getInfoTypeForString("CIVILIZATION_NAZI_GERMANY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NAZI_GERMANY", 52.516, 13.376))
	if (GC.getInfoTypeForString("CIVILIZATION_FINLAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_FINLAND", 60.186,  24.931))
	if (GC.getInfoTypeForString("CIVILIZATION_GERMANY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GERMANY", 50.112, 8.683))
	if (GC.getInfoTypeForString("CIVILIZATION_AUSTRIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_AUSTRIA", 48.243, 16.375))
	if (GC.getInfoTypeForString("CIVILIZATION_POLAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_POLAND", 52.228, 21.006))
	if (GC.getInfoTypeForString("CIVILIZATION_GOTHS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GOTHS", 58.361, 15.780))
	if (GC.getInfoTypeForString("CIVILIZATION_NORWAY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NORWAY", 59.924, 10.752))
	if (GC.getInfoTypeForString("CIVILIZATION_DENMARK", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_DENMARK", 55.690, 12.571))
	if (GC.getInfoTypeForString("CIVILIZATION_NETHERLANDS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NETHERLANDS", 52.330, 4.864))
	if (GC.getInfoTypeForString("CIVILIZATION_ICELAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ICELAND", 49.554, 20.125))
	if (GC.getInfoTypeForString("CIVILIZATION_IRELAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_IRELAND", 49.553, 20.126))
	if (GC.getInfoTypeForString("CIVILIZATION_WALES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_WALES", 49.552, 20.127))
	if (GC.getInfoTypeForString("CIVILIZATION_SCOTLAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SCOTLAND", 49.551, 20.128))
	if (GC.getInfoTypeForString("CIVILIZATION_ENGLAND", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ENGLAND", 51.508, -0.128))
	if (GC.getInfoTypeForString("CIVILIZATION_BELGIUM", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BELGIUM", 49.549, 20.130))
	if (GC.getInfoTypeForString("CIVILIZATION_BULGARIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BULGARIA", 49.548, 20.131))
	if (GC.getInfoTypeForString("CIVILIZATION_SWEDEN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SWEDEN", 49.547, 20.132))
	if (GC.getInfoTypeForString("CIVILIZATION_SWISS_CONFEDERACY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SWISS_CONFEDERACY", 49.546, 20.133))
	if (GC.getInfoTypeForString("CIVILIZATION_LITHUANIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_LITHUANIA", 49.545, 20.134))
	if (GC.getInfoTypeForString("CIVILIZATION_LATVIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_LATVIA", 49.544, 20.135))
	if (GC.getInfoTypeForString("CIVILIZATION_MAGYAR", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MAGYAR", 49.543, 20.136))
	if (GC.getInfoTypeForString("CIVILIZATION_FRANCE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_FRANCE", 48.856, 2.341))
	if (GC.getInfoTypeForString("CIVILIZATION_SPAIN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SPAIN", 40.413, -3.704))
	if (GC.getInfoTypeForString("CIVILIZATION_PORTUGAL", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PORTUGAL", 38.717, -9.167))
	if (GC.getInfoTypeForString("CIVILIZATION_ITALY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ITALY", 41.895, 12.476))
	if (GC.getInfoTypeForString("CIVILIZATION_PAPAL", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PAPAL", 49.538, 20.145))
	if (GC.getInfoTypeForString("CIVILIZATION_GREECE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GREECE", 37.731, 22.756))
	if (GC.getInfoTypeForString("CIVILIZATION_MINOANS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MINOANS", 35.299, 25.170))
	if (GC.getInfoTypeForString("CIVILIZATION_MICROSTATESEUR", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MICROSTATESEUR", 49.536, 20.146))
	if (GC.getInfoTypeForString("CIVILIZATION_ROMANIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ROMANIA", 49.535, 20.147))
	if (GC.getInfoTypeForString("CIVILIZATION_CROATIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CROATIA", 49.534, 20.148))
	if (GC.getInfoTypeForString("CIVILIZATION_SLOVENIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SLOVENIA", 49.533, 20.149))
	if (GC.getInfoTypeForString("CIVILIZATION_CZECH", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CZECH", 49.532, 20.150))
	if (GC.getInfoTypeForString("CIVILIZATION_SLOVAKIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SLOVAKIA", 48.149, 17.106))
	if (GC.getInfoTypeForString("CIVILIZATION_BIH", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BIH", 49.563, 20.151))
	if (GC.getInfoTypeForString("CIVILIZATION_SERBIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SERBIA", 49.564, 20.152))
	if (GC.getInfoTypeForString("CIVILIZATION_ARMENIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ARMENIA", 40.207, 44.556))
	if (GC.getInfoTypeForString("CIVILIZATION_KOSOVO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KOSOVO", 49.565, 20.153))
	if (GC.getInfoTypeForString("CIVILIZATION_UKRAINE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_UKRAINE", 49.566, 20.154))
	if (GC.getInfoTypeForString("CIVILIZATION_RUSSIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_RUSSIA", 55.756, 37.615))
	if (GC.getInfoTypeForString("CIVILIZATION_GEORGIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GEORGIA", 49.568, 20.156))
	if (GC.getInfoTypeForString("CIVILIZATION_CHECHNYA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CHECHNYA", 49.569, 20.157))
	if (GC.getInfoTypeForString("CIVILIZATION_ROME", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ROME", 41.883, 12.483))
	if (GC.getInfoTypeForString("CIVILIZATION_BAKTRIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BAKTRIA", 49.529, 20.159))
	if (GC.getInfoTypeForString("CIVILIZATION_KAZAKH", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KAZAKH", 45.046, 45.752))
	if (GC.getInfoTypeForString("CIVILIZATION_UZBEK", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_UZBEK", 41.320, 69.306))
	if (GC.getInfoTypeForString("CIVILIZATION_XIONGNU", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_XIONGNU", 41.775, 84.244))
	if (GC.getInfoTypeForString("CIVILIZATION_YANGSHAO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_YANGSHAO", 36.042, 114.983))
	if (GC.getInfoTypeForString("CIVILIZATION_MANCHURIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MANCHURIA", 41.787, 123.404))
	if (GC.getInfoTypeForString("CIVILIZATION_AFGHANISTAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_AFGHANISTAN", 45.044, 45.754))
	if (GC.getInfoTypeForString("CIVILIZATION_CENTRALASUN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CENTRALASUN", 45.043, 45.755))
	if (GC.getInfoTypeForString("CIVILIZATION_UYGHUR", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_UYGHUR", 45.042, 45.756))
	if (GC.getInfoTypeForString("CIVILIZATION_MONGOL", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MONGOL", 47.198, 102.821))
	if (GC.getInfoTypeForString("CIVILIZATION_HUNS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_HUNS", 45.041, 45.758))
	if (GC.getInfoTypeForString("CIVILIZATION_JAPAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_JAPAN", 35.017, 135.767))
	if (GC.getInfoTypeForString("CIVILIZATION_AINU", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_AINU", 45.040, 45.771))
	if (GC.getInfoTypeForString("CIVILIZATION_KOREA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KOREA", 39.019, 125.738))
	if (GC.getInfoTypeForString("CIVILIZATION_CHINA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CHINA", 34.455, 113.025))
	if (GC.getInfoTypeForString("CIVILIZATION_TIBET", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_TIBET", 29.650, 91.100))
	if (GC.getInfoTypeForString("CIVILIZATION_VIETNAM", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_VIETNAM", 21.033, 105.850))
	if (GC.getInfoTypeForString("CIVILIZATION_KHMER", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KHMER", 13.433, 103.833))
	if (GC.getInfoTypeForString("CIVILIZATION_BURMA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BURMA", 45.034, 45.761))
	if (GC.getInfoTypeForString("CIVILIZATION_BANGLADESH", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BANGLADESH", 45.033, 45.760))
	if (GC.getInfoTypeForString("CIVILIZATION_INDIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_INDIA", 25.611, 85.144))
	if (GC.getInfoTypeForString("CIVILIZATION_PAKISTAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PAKISTAN", 45.031, 45.758))
	if (GC.getInfoTypeForString("CIVILIZATION_SIAM", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SIAM", 17.021, 99.704))
	if (GC.getInfoTypeForString("CIVILIZATION_MALAYSIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MALAYSIA", 45.029, 45.768))
	if (GC.getInfoTypeForString("CIVILIZATION_MICROSTATESA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MICROSTATESA", 45.028, 45.769))
	if (GC.getInfoTypeForString("CIVILIZATION_INDONESIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_INDONESIA", 45.027, 45.770))
	if (GC.getInfoTypeForString("CIVILIZATION_PHILIPPINES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PHILIPPINES", 45.026, 45.771))
	if (GC.getInfoTypeForString("CIVILIZATION_ASSYRIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ASSYRIA", 35.457, 43.263))
	if (GC.getInfoTypeForString("CIVILIZATION_ARMENIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ARMENIA", 21.433, 39.837))
	if (GC.getInfoTypeForString("CIVILIZATION_ALBANIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ALBANIA", 21.432, 39.836))
	if (GC.getInfoTypeForString("CIVILIZATION_OTTOMAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_OTTOMAN", 21.431, 39.835))
	if (GC.getInfoTypeForString("CIVILIZATION_LEBANON", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_LEBANON", 21.430, 39.834))
	if (GC.getInfoTypeForString("CIVILIZATION_ISRAEL", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ISRAEL", 31.783, 35.216))
	if (GC.getInfoTypeForString("CIVILIZATION_PALMYRA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PALMYRA", 21.428, 39.832))
	if (GC.getInfoTypeForString("CIVILIZATION_IRAQ", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_IRAQ", 21.427, 39.831))
	if (GC.getInfoTypeForString("CIVILIZATION_IRAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_IRAN", 21.426, 39.830))
	if (GC.getInfoTypeForString("CIVILIZATION_SOMALIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SOMALIA", 2.033, 45.333))
	if (GC.getInfoTypeForString("CIVILIZATION_YEMEN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_YEMEN", 21.424, 39.828))
	if (GC.getInfoTypeForString("CIVILIZATION_ETHIOPIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ETHIOPIA", 14.117, 38.733))
	if (GC.getInfoTypeForString("CIVILIZATION_EGYPT", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_EGYPT", 26.333, 31.900))
	if (GC.getInfoTypeForString("CIVILIZATION_PHOENICIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PHOENICIA", 35.602, 35.782))
	if (GC.getInfoTypeForString("CIVILIZATION_SUDAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SUDAN", 21.420, 39.824))
	if (GC.getInfoTypeForString("CIVILIZATION_GARAMANTES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GARAMANTES", 21.419, 39.823))
	if (GC.getInfoTypeForString("CIVILIZATION_ABYSSINIANS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ABYSSINIANS", 21.418, 39.822))
	if (GC.getInfoTypeForString("CIVILIZATION_MORROCO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MORROCO", 21.417, 39.821))
	if (GC.getInfoTypeForString("CIVILIZATION_KANEMBORNU", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KANEMBORNU", 10.418, 29.825))
	if (GC.getInfoTypeForString("CIVILIZATION_MALI", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MALI", 13.890, -4.540))
	if (GC.getInfoTypeForString("CIVILIZATION_BENIN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BENIN", 10.416, 29.823))
	if (GC.getInfoTypeForString("CIVILIZATION_NIGERIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NIGERIA", 10.415, 29.822))
	if (GC.getInfoTypeForString("CIVILIZATION_MANDE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MANDE", 10.414, 29.821))
	if (GC.getInfoTypeForString("CIVILIZATION_GHANA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_GHANA", 10.413, 29.820))
	if (GC.getInfoTypeForString("CIVILIZATION_CONGO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CONGO", -6.267, 14.250))
	if (GC.getInfoTypeForString("CIVILIZATION_TOGO", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_TOGO", 10.411, 29.818))
	if (GC.getInfoTypeForString("CIVILIZATION_KITARA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KITARA", 10.410, 29.817))
	if (GC.getInfoTypeForString("CIVILIZATION_MAASAI", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MAASAI", 10.409, 29.816))
	if (GC.getInfoTypeForString("CIVILIZATION_MUTAPA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MUTAPA", 10.408, 29.815))
	if (GC.getInfoTypeForString("CIVILIZATION_SOUTH_AFRICA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SOUTH_AFRICA", 10.407, 29.814))
	if (GC.getInfoTypeForString("CIVILIZATION_KHOISAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_KHOISAN", 10.406, 29.813))
	if (GC.getInfoTypeForString("CIVILIZATION_MALAGASY", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_MALAGASY", 10.405, 29.812))
	if (GC.getInfoTypeForString("CIVILIZATION_ZULU", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ZULU", -28.317, 31.417))
	if (GC.getInfoTypeForString("CIVILIZATION_BABYLON", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BABYLON", 32.536, 44.421))
	if (GC.getInfoTypeForString("CIVILIZATION_BYZANTIUM", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_BYZANTIUM", 41.009, 28.976))
	if (GC.getInfoTypeForString("CIVILIZATION_CARTHAGE", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CARTHAGE", 36.887, 10.315))
	if (GC.getInfoTypeForString("CIVILIZATION_CELT", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_CELT", 46.923, 4.038))
	if (GC.getInfoTypeForString("CIVILIZATION_PICTS", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PICTS", 46.923, 4.037))
	if (GC.getInfoTypeForString("CIVILIZATION_HITTITES", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_HITTITES", 41.016, 28.966))
	if (GC.getInfoTypeForString("CIVILIZATION_HOLY_ROMAN", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_HOLY_ROMAN", 50.775, 6.084))
	if (GC.getInfoTypeForString("CIVILIZATION_PERSIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_PERSIA", 30.012, 52.408))
	if (GC.getInfoTypeForString("CIVILIZATION_SUMERIA", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_SUMERIA", 31.322, 45.636))
	if (GC.getInfoTypeForString("CIVILIZATION_VIKING", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_VIKING", 63.420, 10.393))
	if (GC.getInfoTypeForString("CIVILIZATION_NEANDERTHAL", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_NEANDERTHAL", 51.227, 6.951))
	if (GC.getInfoTypeForString("CIVILIZATION_ARAWAK", True) > -1):
		coords.append(GeographicalCoordinate("CIVILIZATION_ARAWAK", 14.53, -75.82))

	global CoordinatesDictionary
	CoordinatesDictionary = dict(zip([coord.civ for coord in coords], coords))


def assignCulturallyLinkedStarts():
	print "Culturally Linked Starts Enabled."
	InitCoordinatesDictionary()
	CultureLink().assignStartingPlots()
	global CoordinatesDictionary
	del CoordinatesDictionary


class CultureLink:

	pStartingPlotsList = []
	pRWCoordinatesList = []

	def __init__(self):
		CultureLink.pStartingPlotsList = []
		self.__initStartingPlotsList()
		CultureLink.pRWCoordinatesList = []
		self.__initRWCoordinatesList()

	def __initStartingPlotsList(self):
		GC = CyGlobalContext()
		iNumPlayers = GC.getGame().countCivPlayersEverAlive()
		for iPlayer in range(iNumPlayers):
			pPlayer = GC.getPlayer(iPlayer)
			pStartingPlot = pPlayer.getStartingPlot()
			CultureLink.pStartingPlotsList.append(pStartingPlot)

	def __initRWCoordinatesList(self):
		GC = CyGlobalContext()
		iNumPlayers = GC.getGame().countCivPlayersEverAlive()
		for iPlayer in range(iNumPlayers):
			pPlayer = GC.getPlayer(iPlayer)
			eCivilization = pPlayer.getCivilizationType()
			if eCivilization in CoordinatesDictionary:
				pCoordinate = CoordinatesDictionary[eCivilization]
			else:
				print "[ERROR] Culturally Linked Starts: civilization %s (%d) not defined:" % (GC.getCivilizationInfo(eCivilization).getDescription(), eCivilization)
				pCoordinate = CoordinatesDictionary[eCivilization]
			CultureLink.pRWCoordinatesList.append(pCoordinate)

	def assignStartingPlots(self):

		iNumPlayers = CyGame().countCivPlayersEverAlive()
		iPlayersList = range(iNumPlayers)

		iSPDistanceMatrix = ScaleMatrix(self.__computeSPDistanceMatrix())
		print FormatMatrix(iSPDistanceMatrix, "Starting Plots Distance Matrix:")
		iRWDistanceMatrix = ScaleMatrix(self.__computeRWDistanceMatrix())
		print FormatMatrix(iRWDistanceMatrix, "Real World Distance Matrix:")

		def runBruteForceSearch(permutation = [], depth = 0, best = (None, 'inf')):
			if depth < iNumPlayers:
				for i in iPlayersList:
					if not i in permutation:
						permutation.append(i)
						best = runBruteForceSearch(permutation, depth + 1, best)
						permutation.pop()
			else:
				error = evaluatePermutation(permutation)
				if error < best[1]:
					best = (copy(permutation), error)
					print "%s %.4f" % (best[0], best[1])
			return best

		def runAntColonyOptimization():
			# constants
			# NUM_RUNS = 50
			# NUM_ANTS = 250
			# NUM_BEST_ANTS = 5
			# PHEROMON_UPDATE = 0.025
			NUM_ANTS = iNumPlayers
			NUM_BEST_ANTS = int(iNumPlayers / 10)
			NUM_RUNS = iNumPlayers * 25
			PHEROMON_UPDATE = 0.34 / iNumPlayers
			# the best ant (permutation, error) we know
			TheBestAnt = (None, 'inf')
			# uniformly distributed pheromon at the beginning
			fPheromonMatrix = SquareMatrix(iNumPlayers, 1 / iNumPlayers)
			# the actual ACO:
			for iRun in xrange(NUM_RUNS):
				ants = {}
				# get some random ants:
				for i in xrange(NUM_ANTS):
					permutation = randomList(iPlayersList, fPheromonMatrix)
					error = evaluatePermutation(permutation)
					ants[error] = permutation
				bestants = []
				# get the x best ants (smallest error):
				for error in sorted(ants)[:NUM_BEST_ANTS]:
					ant = (ants[error], error)
					bestants.append(ant)
				# check if we have a new TheBestAnt:
				if bestants[0][1] < TheBestAnt[1]:
					TheBestAnt = bestants[0]
					print "%s %.8f (%d)" % (TheBestAnt[0], TheBestAnt[1], iRun)
				# let the x best ants update the pheromon matrix:
				for i, pos in enumerate(fPheromonMatrix):
						for ant in bestants:
							value = ant[0][i]
							fPheromonMatrix[i][value] = pos[value] + PHEROMON_UPDATE
						# total probability for each pos has to be one:
						fPheromonMatrix[i] = ScaleList(fPheromonMatrix[i])
			return TheBestAnt

		def evaluatePermutation(permutation):
			fPermRWMatrix = []
			for i in permutation:
				row = [iRWDistanceMatrix[i][j] for j in permutation]
				fPermRWMatrix.append(row)
			fError = 0.0
			for i in iPlayersList:
				for j in iPlayersList:
					if i > j:
						# square to make it more robust against small errors
						fError += abs(iSPDistanceMatrix[i][j] - fPermRWMatrix[i][j]) ** 1.3
			return fError

		if iNumPlayers <= 9: # brute force
			iBestPermutation = runBruteForceSearch()[0]
		else: # ants where brute force is impossible
			iBestPermutation = runAntColonyOptimization()[0]

		GC = CyGlobalContext()
		# assign the best found permutation:
		pStartingPlots = CultureLink.pStartingPlotsList
		for iPlayer, iStartingPlot in zip(iBestPermutation, range(len(pStartingPlots))):
			GC.getPlayer(iPlayer).setStartingPlot(pStartingPlots[iStartingPlot], True)

	def __computeRWDistanceMatrix(self):
		pCoordinates = CultureLink.pRWCoordinatesList
		fRWDistanceMatrix = SquareMatrix(len(pCoordinates), 0.0)
		for i, pCoordinateA in enumerate(pCoordinates):
			for j, pCoordinateB in enumerate(pCoordinates):
				if j > i:
					fRWDistance = RealWorldDistance(pCoordinateA, pCoordinateB) / DMAX_EARTH
					fRWDistanceMatrix[i][j] = fRWDistance
					fRWDistanceMatrix[j][i] = fRWDistance
		return fRWDistanceMatrix

	def __computeSPDistanceMatrix(self):
		fSPDistanceMatrix = SquareMatrix(len(CultureLink.pStartingPlotsList), 0.0)
		# fill the starting plot distance matrix with normalized step distances:
		for i, pStartingPlotA in enumerate(CultureLink.pStartingPlotsList):
			for j, pStartingPlotB in enumerate(CultureLink.pStartingPlotsList):
				if i > j:
					fSPDistance = StepDistance(pStartingPlotA, pStartingPlotB) / MaxPossibleStepDistance()
					if pStartingPlotA.getArea() != pStartingPlotB.getArea():
						#print "Area A: %s, Area B: %s" % (pStartingPlotA.getArea(), pStartingPlotB.getArea())
						fSPDistance = fSPDistance * 2
					fSPDistanceMatrix[i][j] = fSPDistance
					fSPDistanceMatrix[j][i] = fSPDistance
		return fSPDistanceMatrix


class GeographicalCoordinate:

	def __init__(self, sCivilizationType, dLatitude, dLongitude):
		self.civ = CyGlobalContext().getInfoTypeForString(sCivilizationType)
		self.lat = dLatitude
		self.lon = dLongitude

	def __str__(self):
		return "%s:\t%8.4f   %8.4f" % (CyGlobalContext().getCivilizationInfo(self.civ).getCityNames(0), self.lat, self.lon)


## GLOBAL HELPER FUNCTIONS:

def RealWorldDistance(pCoordA, pCoordB):
	# equator radius and earth flattening (WGS-84)
	SEMI_MAJOR_AXIS = 6378.137
	INVERSE_FLATTENING = 1/298.257223563
	# some variables to reduce redundancy
	F = pi * (pCoordA.lat + pCoordB.lat) / 360
	G = pi * (pCoordA.lat - pCoordB.lat) / 360
	l = pi * (pCoordA.lon - pCoordB.lon) / 360
	# calculate the rough distance
	S = sin(G)**2 * cos(l)**2 + cos(F)**2 * sin(l)**2
	C = cos(G)**2 * cos(l)**2 + sin(F)**2 * sin(l)**2
	w = atan(sqrt(S/C))
	D = 2 * w * SEMI_MAJOR_AXIS
	# flattening correction
	R = sqrt(S * C) / w
	H1 = INVERSE_FLATTENING * (3 * R - 1) / (2 * C)
	H2 = INVERSE_FLATTENING * (3 * R + 1) / (2 * S)
	return D * (1 + H1 * sin(F)**2 * cos(G)**2 - H2 * cos(F)**2 * sin(G)**2)

def StepDistance(pPlotA, pPlotB):
	return stepDistance(pPlotA.getX(), pPlotA.getY(), pPlotB.getX(), pPlotB.getY())

def MaxPossibleStepDistance():
	if CyMap().getGridWidth() > CyMap().getGridHeight():
		if CyMap().isWrapX():
			return (CyMap().getGridWidth() + 1) // 2
		return CyMap().getGridWidth()
	if CyMap().isWrapY():
		return (CyMap().getGridHeigth() + 1) // 2
	return CyMap().getGridHeight()

def ScaleList(xList):
	return [xElement / sum(xList) for xElement in xList]

def SquareMatrix(iSize, xInitWith = None):
	return [[xInitWith] * iSize for i in range(iSize)]

def ScaleMatrix(matrix, absmin = None, absmax = None):
	minValue = absmin
	maxValue = absmax
	if minValue == None:
		minValue = min([min(row) for row in matrix])
	if maxValue == None:
		maxValue = max([max(row) for row in matrix])
	if minValue == maxValue:
		return "geht nicht"
	return [map(lambda x: (x - minValue) / (maxValue - minValue), row) for row in matrix]

def FormatMatrix(matrix, description = None, rownames = None):
	if len(matrix) > 0:
		s = ""
		if description != None:
			s += description + 2 * "\n"
		s += "["
		for r in xrange(len(matrix)):
			if r > 0:
				s += " "
			s += "["
			for c in xrange(len(matrix[0])):
				if matrix[r][c] != None:
					s += "%8.4f" % matrix[r][c]
				else:
					s += "%8s" % "None"
				if c != len(matrix[0]) - 1:
					s+= ","
			if r == len(matrix) - 1:
				s += "]]"
			else:
				s += "],"
			if rownames != None:
				s += 3 * " " + rownames[r]
			s += "\n"
		return s
	return("Error while creating formated matrix string.")


'''
RANDOMLIST:
Returns a permutation of the items in xList. The probability for each element to appear at position x of
the permutation is defined by row x of the probabilities matrix. Element y in row x is the probability
for element y in xList to appear at position x in the permutation.
In other words: randmomList is a more general form of shuffling with non-uniform probabilities.
'''

def randomList(xList, fProbabilitiesMatrix):
	# make a copy so we can change the values
	fPrMxCopy = deepcopy(fProbabilitiesMatrix)
	iNumElements = len(fPrMxCopy)
	xRandomList = [None] * iNumElements
	# fill xRandomList in random order to prevent bias
	xShuffledList = [0]*iNumElements
	shuffleList(iNumElements, CyGame().getMapRand(), xShuffledList)
	for i in xShuffledList:
		for j in xRandomList:
			if j != None:
				fPrMxCopy[i][j] = 0.0
		fPrMxCopy[i] = ScaleList(fPrMxCopy[i])
		# get a random element from xList not in xRandomList
		xRandomList[i] = randomListElement(xList, fPrMxCopy[i])
	return xRandomList

def randomListElement(xList, fProbabilitiesList):
	maxsoren = 2 ** 16 - 1

	r = CyGame().getSorenRand().get(maxsoren, "CultureLink: randomListElement") / maxsoren
	fCumulatedProbabilities = 0.0
	for i, fProbability in enumerate(fProbabilitiesList):
		fCumulatedProbabilities += fProbability
		if r < fCumulatedProbabilities:
			return xList[i]
	return xList[-1:][0]

## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil

"""
NOTES ABOUT THE MAP UTILITIES

generatePlotTypes(), generateTerrainTypes(), and addFeatures() are mandatory functions for all map scripts.

FractalWorld, HintedWorld, and MultilayeredFractal classes are different ways to generatePlotTypes. (Fractal
world is Soren's baby. HintedWorld is Andy's baby. MultilayeredFractal is Bob's baby.) There is no C++ default
for the plot generation process. Each map script must handle the process on its own, typically by calling
one of these three classes (or subclassing them).

TerrainGenerator is the only primary method for generating terrain types. Call, subclass, or replace it to
set terrain types. FeatureGenerator is the primary method for adding Features to the map.

The minor functions at the end are either children of HintedWorld or, in the case of findStartingPlot, an
alternative method to the default process for placing the starting units for each civ.

- Bob Thomas	September 23, 2005
"""

class FractalWorld:
	def __init__(self, fracXExp=CyFractal.FracVals.DEFAULT_FRAC_X_EXP, fracYExp=CyFractal.FracVals.DEFAULT_FRAC_Y_EXP):
		self.GC = CyGlobalContext()
		self.map = self.GC.getMap()
		self.iNumPlotsX = self.map.getGridWidth()
		self.iNumPlotsY = self.map.getGridHeight()
		self.mapRand = self.GC.getGame().getMapRand()
		self.iFlags = self.map.getMapFractalFlags()
		self.plotTypes = [PlotTypes.PLOT_OCEAN] * (self.iNumPlotsX*self.iNumPlotsY)
		self.fracXExp = fracXExp
		self.fracYExp = fracYExp
		self.continentsFrac = CyFractal()
		self.hillsFrac = CyFractal()
		self.peaksFrac = CyFractal()
		# init User Input variances
		self.seaLevelChange = self.GC.getSeaLevelInfo(self.map.getSeaLevel()).getSeaLevelChange()
		self.seaLevelMax = 100
		self.seaLevelMin = 0
		self.hillGroupOneRange = self.GC.getClimateInfo(self.map.getClimate()).getHillRange()
		self.hillGroupOneBase = 25
		self.hillGroupTwoRange = self.GC.getClimateInfo(self.map.getClimate()).getHillRange()
		self.hillGroupTwoBase = 75
		self.peakPercent = self.GC.getClimateInfo(self.map.getClimate()).getPeakPercent()
		self.stripRadius = 15

	def checkForOverrideDefaultUserInputVariances(self):
		# Subclass and override this function to customize/alter/nullify
		# the XML defaults for user selections on Sea Level, Climate, etc.
		return

	def initFractal(self, continent_grain = 2, rift_grain = 2, has_center_rift = True, invert_heights = False, polar = False):
		"For no rifts, use rift_grain = -1"
		iFlags = self.iFlags
		if invert_heights:
			iFlags += CyFractal.FracVals.FRAC_INVERT_HEIGHTS
		if polar:
			iFlags += CyFractal.FracVals.FRAC_POLAR
		if rift_grain >= 0:
			self.riftsFrac = CyFractal()
			self.riftsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, rift_grain, self.mapRand, 0, self.fracXExp, self.fracYExp)
			if has_center_rift:
				iFlags += CyFractal.FracVals.FRAC_CENTER_RIFT
			self.continentsFrac.fracInitRifts(self.iNumPlotsX, self.iNumPlotsY, continent_grain, self.mapRand, iFlags, self.riftsFrac, self.fracXExp, self.fracYExp)
		else:
			self.continentsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, continent_grain, self.mapRand, iFlags, self.fracXExp, self.fracYExp)

	def shiftPlotTypes(self):
		stripRadius = self.stripRadius
		best_split_x, best_split_y = 0,0

		if self.map.isWrapX():
			best_split_x = self.findBestSplitX(stripRadius)
		if self.map.isWrapY():
			best_split_y = self.findBestSplitY(stripRadius)

		self.shiftPlotTypesBy(best_split_x, best_split_y)

	def shiftPlotTypesBy(self, xshift, yshift):
		if xshift > 0 or yshift > 0:
			iWH = self.iNumPlotsX * self.iNumPlotsY
			buf = self.plotTypes[:]
			for iDestY in range(self.iNumPlotsY):
				for iDestX in range(self.iNumPlotsX):
					iDestI = self.iNumPlotsX*iDestY + iDestX
					iSourceX = iDestX + xshift
					iSourceY = iDestY + yshift
					iSourceX %= self.iNumPlotsX
					iSourceY %= self.iNumPlotsY

					iSourceI = self.iNumPlotsX*iSourceY + iSourceX
					self.plotTypes[iDestI] = buf[iSourceI]

	def findBestSplitY(self, stripRadius):
		stripSize = 2*stripRadius
		if stripSize > self.iNumPlotsX:
			return 0

		numPlots = self.iNumPlotsX * self.iNumPlotsY
		stripCenterIndex = stripRadius
		piLandWeights = self.calcWeights(stripRadius)

		scores = [0]*self.iNumPlotsY
		for y in range(self.iNumPlotsY):
			landScore = 0
			bFoundLand = False
			for x in range(self.iNumPlotsX):
				i = y*self.iNumPlotsX + x
				assert (i >= 0 and i < numPlots)
				if self.plotTypes[i] == PlotTypes.PLOT_LAND:
					landScore += 1
					bFoundLand = True
			if bFoundLand:
				landScore += 30 # the first land is worth about 10 plots of land

			for i in range(stripSize):
				yy = y + i - stripCenterIndex
				yy %= self.iNumPlotsY
				scores[yy] += landScore * piLandWeights[i]

		best_split_y, lowest_score = argmin(scores)
		return best_split_y

	def findBestSplitX(self, stripRadius):
		stripSize = 2*stripRadius
		if stripSize > self.iNumPlotsX:
			return 0

		numPlots = self.iNumPlotsX * self.iNumPlotsY
		stripCenterIndex = stripRadius
		piLandWeights = self.calcWeights(stripRadius)

		scores = [0]*self.iNumPlotsX
		for x in range(self.iNumPlotsX):
			landScore = 0
			bFoundLand = False
			for y in range(self.iNumPlotsY):
				i = y*self.iNumPlotsX + x
				assert (i >= 0 and i < numPlots)
				if self.plotTypes[i] == PlotTypes.PLOT_LAND:
					landScore += 1
					bFoundLand = True
			if bFoundLand:
				landScore += 30 # the first land is worth about 10 plots of land

			for i in range(stripSize):
				xx = x + i - stripCenterIndex
				xx %= self.iNumPlotsX
				scores[xx] += landScore * piLandWeights[i]

		best_split_x, lowest_score = argmin(scores)
		return best_split_x

	def calcWeights(self, stripRadius):
		stripSize = 2*stripRadius
		landWeights = [0]*stripSize
		for i in range(stripSize):
			distFromStart = i+1
			distFromEnd = stripSize-i
			distFromEdge = min(distFromStart, distFromEnd)
			landWeight = distFromEdge
			distFromCenter = stripRadius - distFromEdge
			if distFromCenter <= 1:
				landWeight *= stripRadius
			if distFromCenter == 0:
				landWeight *= 2
			landWeights[i] = landWeight
		return landWeights

	def generatePlotTypes(self, water_percent=78, shift_plot_types=True, grain_amount=3):
		# Check for changes to User Input variances.
		self.checkForOverrideDefaultUserInputVariances()

		self.hillsFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.peaksFrac.fracInit(self.iNumPlotsX, self.iNumPlotsY, grain_amount+1, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		water_percent += self.seaLevelChange
		water_percent = min(water_percent, self.seaLevelMax)
		water_percent = max(water_percent, self.seaLevelMin)

		iWaterThreshold = self.continentsFrac.getHeightFromPercent(water_percent)
		iHillsBottom1 = self.hillsFrac.getHeightFromPercent(max((self.hillGroupOneBase - self.hillGroupOneRange), 0))
		iHillsTop1 = self.hillsFrac.getHeightFromPercent(min((self.hillGroupOneBase + self.hillGroupOneRange), 100))
		iHillsBottom2 = self.hillsFrac.getHeightFromPercent(max((self.hillGroupTwoBase - self.hillGroupTwoRange), 0))
		iHillsTop2 = self.hillsFrac.getHeightFromPercent(min((self.hillGroupTwoBase + self.hillGroupTwoRange), 100))
		iPeakThreshold = self.peaksFrac.getHeightFromPercent(self.peakPercent)

		for x in range(self.iNumPlotsX):
			for y in range(self.iNumPlotsY):
				i = y*self.iNumPlotsX + x
				val = self.continentsFrac.getHeight(x,y)
				if val <= iWaterThreshold:
					self.plotTypes[i] = PlotTypes.PLOT_OCEAN
				else:
					hillVal = self.hillsFrac.getHeight(x,y)
					if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)):
						peakVal = self.peaksFrac.getHeight(x,y)
						if (peakVal <= iPeakThreshold):
							self.plotTypes[i] = PlotTypes.PLOT_PEAK
						else:
							self.plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						self.plotTypes[i] = PlotTypes.PLOT_LAND

		if shift_plot_types:
			self.shiftPlotTypes()

		return self.plotTypes

cardinal_directions = (1,0), (0,1), (-1,0), (0, -1)

class HintedWorld(FractalWorld):
	def __init__(self, w=16, h=8, fracXExp=CyFractal.FracVals.DEFAULT_FRAC_X_EXP,
	             fracYExp=CyFractal.FracVals.DEFAULT_FRAC_Y_EXP):
		FractalWorld.__init__(self, fracXExp, fracYExp)

		self.plotsPerBlockX = self.iNumPlotsX/w
		self.plotsPerBlockY = self.iNumPlotsY/h

		if not self.iFlags & CyFractal.FracVals.FRAC_WRAP_X:
			w += 1
		if not self.iFlags & CyFractal.FracVals.FRAC_WRAP_Y:
			h += 1

		self.w, self.h = w,h # the map is divided into 'w' blocks by 'h' blocks
		self.data = [None]*(w*h)
		self.mapRand = CyGlobalContext().getGame().getMapRand()
		self.continents = []

	def normalizeBlock(self, x, y):
		map = CyMap()
		if map.isWrapX():
			x = x % self.w
		if map.isWrapY():
			y = y % self.h
		return x,y

	def setValue(self, x, y, val):
		x,y = self.normalizeBlock(x,y)

		if self.inBounds(x,y):
			self.data[self.w*y + x] = val
			return True
		else:
			return False

	def getValue(self, x, y):
		x,y = self.normalizeBlock(x,y)
		if self.inBounds(x,y):
			return self.data[self.w*y + x]
		else:
			return None

	def blockToPlot(self, blockx, blocky):
		scalex, scaley = self.plotsPerBlockX, self.plotsPerBlockY
		plotx, ploty = scalex*(blockx), scaley*(blocky)
		return (int(plotx), int(ploty))

	# nested class to describe a continent in the hinted world
	class Continent:
		def __init__(self, world, numBlocks, x, y, maxradius):
			self.world = world
			self.centerx = x
			self.centery = y
			self.targetNumBlocks = numBlocks
			self.maxradius = maxradius
			self.blocks = [(x,y)] # (x,y) coords of blocks that compose the continent
			self.rects = [] # one (x,y,w,h) rect. of plots for each (x,y) block'

			if numBlocks <= 1:
				self.done = True
			else:
				self.done = False

		def addBlock(self, x, y):
			self.blocks.append((x,y))
			scalex, scaley = self.world.plotsPerBlockX, self.world.plotsPerBlockY
			rect = int(x*scalex), int(y*scaley), int(1*scalex), int(1*scaley)
			self.rects.append(rect)
			if len(self.blocks) >= self.targetNumBlocks:
				self.done = True

		def recalculateRects(self):
			scalex, scaley = self.world.plotsPerBlockX, self.world.plotsPerBlockY
			self.rects = []
			for (x,y) in self.blocks:
				rect = int(x*scalex), int(y*scaley), int(1*scalex), int(1*scaley)
				self.rects.append(rect)

		def containsPlot(self, x, y): # could add bRemoveParentRect here
			point = (x,y)
			for rect in self.rects:
				if pointInRect(point, rect):
					return True
			return False

		def getCenterPlot(self):
			scalex, scaley = self.world.plotsPerBlockX, self.world.plotsPerBlockY
			x = scalex*(self.centerx+0.5)
			y = scaley*(self.centery+0.5)
			return x,y

		def findStartingPlot(self, playerID):
			validFn = lambda playerID, x, y: self.containsPlot(x,y)
			return findStartingPlot(playerID, validFn) # call global fn

	def addContinent(self, numBlocks, x=-1, y=-1, maxDist=-1, maxRadius=-1):
		if (x == -1):
			x = self.mapRand.get(self.w, "Add Continent Width PYTHON")
		if (y == -1):
			y = self.mapRand.get(self.h, "Add Continent Height PYTHON")

		foundx, foundy = self.findValid(x,y, maxDist)
		if (foundx == -1 and foundy == -1):
			return None
		else:
			return self.__addContinentAt(numBlocks, foundx, foundy, maxRadius)

	def __addContinentAt(self, numBlocks, x, y, maxradius=-1):
		land_value = 192 + self.mapRand.get(64, "Add Continent At PYTHON")
		self.setValue(x,y, land_value)
		cont = HintedWorld.Continent(self,numBlocks,x,y,maxradius)
		self.continents.append(cont)
		return cont

	def expandContinentBy(self, cont, numBlocks):
		# this plot is not valid; choose an in-bounds plot adjacent to an existing plot and try again:
		#print "expand continent by", numBlocks
		blockOrder = CvUtil.shuffle(len(cont.blocks), self.mapRand)
		for blockIndex in blockOrder:
			x,y = cont.blocks[blockIndex]
			dirOrder = CvUtil.shuffle(len(cardinal_directions), self.mapRand)
			for dirIndex in dirOrder:
				dx, dy = cardinal_directions[dirIndex]
				if self.isValid(x+dx,y+dy, cont):
					cont.addBlock(x+dx,y+dy)
					land_value = 208 + self.mapRand.get(48, "Expand Continent PYTHON")
					self.setValue(x+dx, y+dy, land_value)
					#print "\tadded block", x+dx, y+dy
					if (numBlocks > 1):
						return self.expandContinentBy(cont, numBlocks-1)
					else:
						return True

		print "\tcould not expand continent:"
		printMap(self.data, self.w, self.h, cont.centerx, cont.centery)
		cont.done = True
		return False

	def buildAllContinents(self):
		all_done = False
		while not all_done:
			all_done = True
			for cont in self.continents:
				if not cont.done:
					self.expandContinentBy(cont, 1) #expand by 1 block
					all_done = False

	def shiftHintsToMap(self):
		map = CyMap()
		wrapX = map.isWrapX()
		wrapY = map.isWrapY()

		splitx, splity = 0,0
		#self.printHints()
		if (wrapX):
			splitx = self.bestHintsSplitX()
		if (wrapY):
			splity = self.bestHintsSplitY()
		self.shiftHintsBy(splitx, splity)
		#self.printHints()

	def bestHintsSplitX(self):
		scores = [0]*self.w
		for x in range(self.w):
			for y in range(self.h):
				if self.getValue(x, y) >= 192: scores[x] += 1
				if self.getValue(x-1, y) >= 192: scores[x] += 1
		best_split, best_score = argmin(scores)
		return best_split

	def bestHintsSplitY(self):
		scores = [0]*self.h
		for x in range(self.w):
			for y in range(self.h):
				if self.getValue(x, y) >= 192: scores[y] += 1
				if self.getValue(x, y-1) >= 192: scores[y] += 1
		best_split, best_score = argmin(scores)
		return best_split

	def shiftHintsBy(self, splitx, splity):
		print "shifting hints by ", splitx, splity
		if splitx != 0 or splity != 0:
			buf = self.data[:]
			# shift the values in self.data left by best_split
			for x in range(self.w):
				for y in range(self.h):
					i = y*self.w + x
					self.setValue(x-splitx, y-splity, buf[i])

			# shift all continents' blocks left by best_split
			for cont in self.continents:
				cont.blocks = [self.normalizeBlock(x-splitx, y-splity) for (x,y) in cont.blocks]
				cont.recalculateRects()

	# self.data must represent a rect where w = 2*h,
	# and where both w and h are exponents of 2
	def __doInitFractal(self):
		self.shiftHintsToMap()

		# don't call base method, this overrides it.
		size = len(self.data)
		minExp = min(self.fracXExp, self.fracYExp)
		iGrain = None
		for i in range(minExp):
			width = (1 << (self.fracXExp - minExp + i))
			height = (1 << (self.fracYExp - minExp + i))
			if not self.iFlags & CyFractal.FracVals.FRAC_WRAP_X:
				width += 1
			if not self.iFlags & CyFractal.FracVals.FRAC_WRAP_Y:
				height += 1
			if size == width*height:
				iGrain = i
		assert(iGrain != None)
		iFlags = self.map.getMapFractalFlags()
		self.continentsFrac.fracInitHints(self.iNumPlotsX, self.iNumPlotsY, iGrain, self.mapRand, iFlags, self.data, self.fracXExp, self.fracYExp)

	def isValid(self, x, y, cont=None):
		if not self.inBounds(x, y):
			return False
		if cont and cont.maxradius > 0:
			if abs(x - cont.centerx) + abs(y - cont.centery) > cont.maxradius:
				return False
		val = self.getValue(x,y)
		if val != None:
			return False
		for dx in range(-1,2):
			for dy in range(-1,2):
				val = self.getValue(x+dx, y+dy)
				if val != None and val >= 192 and ((not cont) or (x+dx, y+dy) not in cont.blocks):
					return False
		return True


	def findValid(self, x, y, dist=-1):
		if (dist == -1):
			dist = max(self.w, self.h)

		if (dist > 0):
			foundx, foundy = self.findValid(x, y, dist-1)
			if (foundx != -1 and foundy != -1):
				return foundx, foundy

		plots = []
		for dx in range(-dist, dist+1):
			for dy in range(-dist, dist+1):
				if max(abs(dx), abs(dy)) == dist:
					plots.append((x+dx, y+dy))

		plotOrder = CvUtil.shuffle(len(plots), self.mapRand)
		for plotIndex in plotOrder:
			tryx, tryy = plots[plotIndex]
			if self.isValid(tryx, tryy): return tryx, tryy

		return -1, -1

	def printHints(self, markerx=-1, markery=-1):
		printMap(self.data, self.w, self.h, markerx, markery)

	def inBounds(self, x, y):
		x,y = self.normalizeBlock(x,y)
		return (0 <= x < self.w and 0 <= y < self.h)

	def generatePlotTypes(self, water_percent=-1, shift_plot_types=False):
		for i in range(len(self.data)):
			if self.data[i] == None:
				self.data[i] = self.mapRand.get(48, "Generate Plot Types PYTHON")

		self.__doInitFractal()
		if (water_percent == -1):
			numPlots = len(self.data)
			numWaterPlots = 0
			for val in self.data:
				if val < 192: # XXX what is this???
					numWaterPlots += 1
			water_percent = int(100*numWaterPlots/numPlots)

		return FractalWorld.generatePlotTypes(self, water_percent, shift_plot_types) # call superclass

def printMap(data, w, h, markerx=-1, markery=-1):
	print "-"*(w+2)
	hrange = range(h)
	hrange.reverse()
	for y in hrange:
		str = "|"
		for x in range(w):
			val = data[y*w + x]
			if (x,y) == (markerx, markery):
				str += "O"
			elif val != 0:
				str += "X"
			else:
				str += " "
		str += "|"
		print str
	print "-"*(w+2)

'''
SIRIAN's "MULTILAYERED FRACTAL" INSTRUCTIONS

Since some map scripting concepts demanded the ability to use more than one
fractal instance to generate plot types, I set out to create a new class that
would use multiple "regional fractals" to assemble a complex map.

MultilayeredFractal duplicates the effects of FractalWorld for each layer
in turn. GeneratePlotsByRegion is the controlling function. You will need to
customize this function for each usage, but the rest of the class will stand
as written unless your needs fall outside the normal intended usage.

I've included an enormous amount of power over the layers, but this does mean
a long list of parameters that you must understand and organize for each layer.

Each layer must be passed this list of arguments:

Regional Variables Key:

iWaterPercent,
iRegionWidth, iRegionHeight,
iRegionWestX, iRegionSouthY,
iRegionGrain, iRegionHillsGrain,
iRegionPlotFlags, iRegionTerrainFlags,
iRegionFracXExp, iRegionFracYExp,
bShift, iStrip,
rift_grain, has_center_rift,
invert_heights

Most of these should be self-explanatory, but I'll discuss the rest.

-------------------------------------------------
Grain is the density of land vs sea. Higher numbers generate more and smaller land masses.

HillsGrain is the density of highlands vs flatlands.
Peaks are included in highlands and work off the same density numbers.

Flags are special variables to pass to the fractal generator.
* FRAC_POLAR will eliminate straight edges along the border of your region.
* FRAC_WRAP_X will "spread out" the fractal horizontally and cancel FRAC_POLAR's vertical component.
* FRAC_WRAP_Y will "spread out" the fractal vertically and cancel FRAC_POLAR's horizontal component.

The Polar flag causes a maximum "height value" to be returned for any coordinates
with a zero component. (0,0 or 0,15 or 71,0 - for instance.) This can cause
problems for terrain and features on maps that put land plots in the zero row
or column. This will also cause a problem for any fractal region you generate.
I've included shortcuts for typical uses, but you may need to customize the flags
for some purposes. PlotFlags and TerrainFlags give you full control.

FracXExp is the width of the source fractal.
FracYExp is the height of the source fractal.
These exponents are raised to powers of two. So a value of FracXExp = 7
means 2^7, which would be 128 units wide. FracXExp = 6 would be only 64
units wide. FracYExp works the same way.

Default values are 7 for FracXExp and 6 for FracYExp, or 128x64 matrix.

I've poked around with the fractals quite a bit. Values lower than 5 seem to
distort the fractal's definition too much, so I don't recommend them even for
use with very small regions. 6x5 proved to be the smallest that I trust. Higher
exponents will generate more defined and refined fractal outputs, but at the
cost of increased calculation times. I would not recommend using exponents
higher than 9. (Larger than 512 even in only one direction is hopeless for Civ4's
True Pathfinding processes, anyway. The game would be unplayably slow!) So I
recommend sticking with 7 as maximum exponent unless your map will be more than
32 (4x4) plot blocks (128 plots) in at least one dimension. Sticking between the
ranges of 6 and 8 for whole maps, and 5 and 7 for regions, is recommended.

Shift is a boolean flag as to whether or not to shift plots in that region.

Strip value has to do with "shifting" the plots to reduce rough edges. This
process overlaps with the Polar flags, though, so if you are using Polar flags,
shifting won't do anything for you along the edges that are Polar-shifted.
The strip size needs to scale appropriately to the size of the region being
shifted. As of this writing, I have not yet determined all the best values to
fit with certain sizes of regions. (And these are a moving target based on map
sizes!) I will try to figure this out and update these notes again before release.

rift_grain has to do with forced strips of water running in a mostly vertical
direction. They simulate the Atlantic and Pacific Oceans separating Earth's
two primary land regions. You can turn off the Atlantic rift by setting
has_center_rift to false. You can turn off all rifts by setting rift_grain
to -1. For most regional fractals, you will probably want to disable rifts.

invert_heights is not a function I have needed, but it seems clear enough. It
has to do with results returned by the fractal generator and could be used
instead of adjusting the height values, in some cases. I always adjust the
height values, though, so it has seemed like a redundant option. It's there
in case somebody wanted to use it, though.
-------------------------------------------------

GeneratePlotsInRegion is a fully automated process. If you want to layer land
plots, all you need to do is call this function over and over from the
controlling function: GeneratePlotsByRegion

Each region needs to be defined by the map scripter, then organized in the
controlling function. Pass in the necessary arguments to generatePlotsInRegion
and get back a region of land, already "layered" on to the global plot array.

The global plot array begins as all water. Each layer of fractalized plots is
applied in turn, overwriting the previous layer. Water plots in each layer are
ignored. Land plots of any type are assigned to the applicable plot. The water
"left over" at the end of the process will be whatever plots went untouched by
any of the regional layers' land plots. If regions overlap, landforms may overlap,
too. This allows both separate-distinct regional use, and layering over a single
area with as many passes as the scripter selects.


For most uses, you can use a new subclass to override GeneratePlotsByRegion
and not have to mess with the rest of the class. GeneratePlotsByRegion is the
controlling function and must be customized for each applicable map script.

- Bob Thomas   July 13, 2005
'''

# This class can be called instead of FractalWorld or HintedWorld.
# MultilayeredFractal enables multiple fractals to be
# layered over a single map, to generate plot types.
# Use GeneratePlotsByRegion to organize your fractal layers.

class MultilayeredFractal:
	def __init__(self, fracXExp=CyFractal.FracVals.DEFAULT_FRAC_X_EXP,
	             fracYExp=CyFractal.FracVals.DEFAULT_FRAC_Y_EXP):
		self.GC = CyGlobalContext()
		self.map = self.GC.getMap()
		self.iW = self.map.getGridWidth()
		self.iH = self.map.getGridHeight()
		self.dice = self.GC.getGame().getMapRand()
		self.iFlags = self.map.getMapFractalFlags() # Defaults for that map type.
		self.iTerrainFlags = self.map.getMapFractalFlags() # Defaults for that map type.
		self.iHorzFlags = CyFractal.FracVals.FRAC_WRAP_X + CyFractal.FracVals.FRAC_POLAR # Use to prevent flat edges to north or south.
		self.iVertFlags = CyFractal.FracVals.FRAC_WRAP_Y + CyFractal.FracVals.FRAC_POLAR # Use to prevent flat edges to east or west.
		self.iRoundFlags = CyFractal.FracVals.FRAC_POLAR # Use to prevent flat edges on all sides.
		self.plotTypes = [] # Regional array
		self.wholeworldPlotTypes = [PlotTypes.PLOT_OCEAN] * (self.iW*self.iH) # Global
		self.fracXExp = fracXExp
		self.fracYExp = fracYExp
		# Note: there is no checkForOverrideDefaultUserInputVariances()
		# function for MultilayeredFractal. You should control any
		# user input variances per region, in your region definitions.

	def shiftRegionPlots(self, iRegionWidth, iRegionHeight, iStrip=15):
		# Minimizes land plots along the region's edges by shifting the coordinates.
		stripRadius = min(15, iStrip)
		stripRadius = max(3, iStrip)
		best_split_x, best_split_y = 0,0
		best_split_x = self.findBestRegionSplitX(iRegionWidth, iRegionHeight, stripRadius)
		best_split_y = self.findBestRegionSplitY(iRegionWidth, iRegionHeight, stripRadius)
		self.shiftRegionPlotsBy(best_split_x, best_split_y, iRegionWidth, iRegionHeight)

	def shiftRegionPlotsBy(self, xshift, yshift, iRegionWidth, iRegionHeight):
		if xshift > 0 or yshift > 0:
			iWH = iRegionWidth * iRegionHeight
			buf = self.plotTypes[:]
			for iDestY in range(iRegionHeight):
				for iDestX in range(iRegionWidth):
					iDestI = iRegionWidth*iDestY + iDestX
					iSourceX = iDestX + xshift
					iSourceY = iDestY + yshift
					iSourceX %= iRegionWidth # normalize
					iSourceY %= iRegionHeight

					iSourceI = iRegionWidth*iSourceY + iSourceX
					self.plotTypes[iDestI] = buf[iSourceI]

	def findBestRegionSplitY(self, iRegionWidth, iRegionHeight, stripRadius):
		stripSize = 2*stripRadius
		if stripSize > iRegionWidth:
			return 0

		numPlots = iRegionWidth * iRegionHeight
		stripCenterIndex = stripRadius
		piLandWeights = self.calcWeights(stripRadius)

		scores = [0]*iRegionHeight
		for y in range(iRegionHeight):
			landScore = 0
			bFoundLand = False
			for x in range(iRegionWidth):
				i = y*iRegionWidth + x
				assert (i >= 0 and i < numPlots)
				if self.plotTypes[i] == PlotTypes.PLOT_LAND:
					landScore += 1
					bFoundLand = True
			if bFoundLand:
				landScore += 30 # the first land is worth about 10 plots of land

			for i in range(stripSize):
				yy = y + i - stripCenterIndex
				yy %= iRegionHeight
				scores[yy] += landScore * piLandWeights[i]

		best_split_y, lowest_score = argmin(scores)
		return best_split_y

	def findBestRegionSplitX(self, iRegionWidth, iRegionHeight, stripRadius):
		stripSize = 2*stripRadius
		if stripSize > iRegionWidth:
			return 0

		numPlots = iRegionWidth * iRegionHeight
		stripCenterIndex = stripRadius
		piLandWeights = self.calcWeights(stripRadius)

		scores = [0]*iRegionWidth
		for x in range(iRegionWidth):
			landScore = 0
			bFoundLand = False
			for y in range(iRegionHeight):
				i = y*iRegionWidth + x
				assert (i >= 0 and i < numPlots)
				if self.plotTypes[i] == PlotTypes.PLOT_LAND:
					landScore += 1
					bFoundLand = True
			if bFoundLand:
				landScore += 30 # the first land is worth about 10 plots of land

			for i in range(stripSize):
				xx = x + i - stripCenterIndex
				xx %= iRegionWidth
				scores[xx] += landScore * piLandWeights[i]

		best_split_x, lowest_score = argmin(scores)
		return best_split_x

	def calcWeights(self, stripRadius):
		stripSize = 2*stripRadius
		landWeights = [0]*stripSize
		for i in range(stripSize):
			distFromStart = i+1
			distFromEnd = stripSize-i
			distFromEdge = min(distFromStart, distFromEnd)
			landWeight = distFromEdge
			distFromCenter = stripRadius - distFromEdge
			if distFromCenter <= 1:
				landWeight *= stripRadius
			if distFromCenter == 0:
				landWeight *= 2
			landWeights[i] = landWeight
		return landWeights

	def generatePlotsInRegion(self, iWaterPercent,
	                          iRegionWidth, iRegionHeight,
	                          iRegionWestX, iRegionSouthY,
	                          iRegionGrain, iRegionHillsGrain,
	                          iRegionPlotFlags, iRegionTerrainFlags,
	                          iRegionFracXExp = -1, iRegionFracYExp = -1,
	                          bShift = True, iStrip = 15,
	                          rift_grain = -1, has_center_rift = False,
	                          invert_heights = False):
		# This is the code to generate each fractal.
		# Determine and pass in the appropriate arguments from the controlling function.
		#
		# Init local variables
		water = iWaterPercent
		iWestX = iRegionWestX
		# Note: if you pass bad regional dimensions so that iEastX > self.iW, BOOM! So don't do that. I could close out that possibility, but better that I not, so that you get an error to warn you of erroneous regional parameters. - Sirian
		iSouthY = iRegionSouthY

		# Init the plot types array and the regional fractals
		self.plotTypes = [] # reinit the array for each pass
		self.plotTypes = [PlotTypes.PLOT_OCEAN] * (iRegionWidth*iRegionHeight)
		regionContinentsFrac = CyFractal()
		regionHillsFrac = CyFractal()
		regionPeaksFrac = CyFractal()
		regionContinentsFrac.fracInit(iRegionWidth, iRegionHeight, iRegionGrain, self.dice, iRegionPlotFlags, iRegionFracXExp, iRegionFracYExp)
		regionHillsFrac.fracInit(iRegionWidth, iRegionHeight, iRegionHillsGrain, self.dice, iRegionTerrainFlags, iRegionFracXExp, iRegionFracYExp)
		regionPeaksFrac.fracInit(iRegionWidth, iRegionHeight, iRegionHillsGrain+1, self.dice, iRegionTerrainFlags, iRegionFracXExp, iRegionFracYExp)

		iWaterThreshold = regionContinentsFrac.getHeightFromPercent(water)
		iHillsBottom1 = regionHillsFrac.getHeightFromPercent(max((25 - self.GC.getClimateInfo(self.map.getClimate()).getHillRange()), 0))
		iHillsTop1 = regionHillsFrac.getHeightFromPercent(min((25 + self.GC.getClimateInfo(self.map.getClimate()).getHillRange()), 100))
		iHillsBottom2 = regionHillsFrac.getHeightFromPercent(max((75 - self.GC.getClimateInfo(self.map.getClimate()).getHillRange()), 0))
		iHillsTop2 = regionHillsFrac.getHeightFromPercent(min((75 + self.GC.getClimateInfo(self.map.getClimate()).getHillRange()), 100))
		iPeakThreshold = regionPeaksFrac.getHeightFromPercent(self.GC.getClimateInfo(self.map.getClimate()).getPeakPercent())

		# Loop through the region's plots
		for x in range(iRegionWidth):
			for y in range(iRegionHeight):
				i = y*iRegionWidth + x
				val = regionContinentsFrac.getHeight(x,y)
				if val <= iWaterThreshold: pass
				else:
					hillVal = regionHillsFrac.getHeight(x,y)
					if ((hillVal >= iHillsBottom1 and hillVal <= iHillsTop1) or (hillVal >= iHillsBottom2 and hillVal <= iHillsTop2)):
						peakVal = regionPeaksFrac.getHeight(x,y)
						if (peakVal <= iPeakThreshold):
							self.plotTypes[i] = PlotTypes.PLOT_PEAK
						else:
							self.plotTypes[i] = PlotTypes.PLOT_HILLS
					else:
						self.plotTypes[i] = PlotTypes.PLOT_LAND

		if bShift:
			# Shift plots to obtain a more natural shape.
			self.shiftRegionPlots(iRegionWidth, iRegionHeight, iStrip)

		# Once the plot types for the region have been generated, they must be
		# applied to the global plot array.
		#
		# Default approach is to ignore water and layer the lands over one another.
		# If you want to layer the water, too, or some other combination, then
		# create a subclass and override this function. Customize in your override.
		#
		# Apply the region's plots to the global plot array.
		for x in range(iRegionWidth):
			wholeworldX = x + iWestX
			for y in range(iRegionHeight):
				i = y*iRegionWidth + x
				if self.plotTypes[i] == PlotTypes.PLOT_OCEAN: continue
				wholeworldY = y + iSouthY
				iWorld = wholeworldY*self.iW + wholeworldX
				self.wholeworldPlotTypes[iWorld] = self.plotTypes[i]

		# This region is done.
		return

	def generatePlotsByRegion(self):
		# Sirian's MultilayeredFractal class, controlling function.
		# You -MUST- customize this function for each use of the class.
		#
		# The rest of this function from CvMapGeneratorUtil.py is provided
		# to you as a template. You will have to build your own version for
		# use with your map scripts, according to your designs.
		#
		# The following object indexes custom grain amounts per world size.
		# Add a new column for each desired global or regional grain setting.
		# (Grains are used to control fractal results. Larger grains create
		# smaller patches of similar values. Small grains create large patches.)
		#
		# Here is an example of obtaining grain sizes to fit with map sizes.

		# Rise of Mankind MOD EDIT START --------------------------------------
		sizekey = self.map.getWorldSize()
		sizevalues = {
			WorldSizeTypes.WORLDSIZE_DUEL:      (3,2,1,2),
			WorldSizeTypes.WORLDSIZE_TINY:      (3,2,1,2),
			WorldSizeTypes.WORLDSIZE_SMALL:     (3,2,1,2),
			WorldSizeTypes.WORLDSIZE_STANDARD:  (4,2,1,2),
			WorldSizeTypes.WORLDSIZE_LARGE:     (4,2,1,2),
			WorldSizeTypes.WORLDSIZE_HUGE:      (5,2,1,2),
			WorldSizeTypes.WORLDSIZE_GIANT:     (5,2,1,2),
			WorldSizeTypes.WORLDSIZE_GIGANTIC:  (6,2,1,2)
			}
		# Rise of Mankind MOD EDIT END ----------------------------------------

		# You can add as many grain entries as you like.
		# Seed them all from the matrix using the following type of line:
		(iGrainOne, iGrainTwo, iGrainThree, iGrainFour) = sizevalues[sizekey]
		# The example is for four grain values. You may not need that many.
		# Check scripts that use MultilayeredFractal for more examples.

		# Define the regions (necessary to any use of generatePlotsByRegion)
		# Start by initializing regional definitions.
		# All regions must be rectangular. (The fractal only feeds on these!)
		# Obtain region width and height by any method you care to design.
		# Obtain WestX and EastX, NorthY and SouthY, to define the boundaries.
		#
		# Note that Lat and Lon as used here are different from the use for
		# the generation of terrain types and features. Sorry for the ambiguity!
		#
		# Latitude and Longitude are values between 0.0 and 1.0
		# Latitude South to North is 0.0 to 1.0
		# Longitude West to East is 0.0 to 1.0
		# Plots are indexed by X,Y with 0,0 in SW corner.
		#
		# Here is an example set of definitions
		regiononeWestLon = 0.05
		regiononeEastLon = 0.35
		regiontwoWestLon = 0.45
		regiontwoEastLon = 0.95
		regiontwoNorthLat = 0.95
		regiontwoSouthLat = 0.45
		subcontinentLargeHorz = 0.2
		subcontinentLargeVert = 0.32
		subcontinentLargeNorthLat = 0.6
		subcontinentLargeSouthLat = 0.28
		subcontinentSmallDimension = 0.125
		subcontinentSmallNorthLat = 0.525
		subcontinentSmallSouthLat = 0.4
		# You can then use these longitudes and latitudes crossed with grid sizes
		# to enable one definition to fit any map size, map width, map height.

		# Define your first region here.
		NiTextOut("Generating Region One (Python Map_Script_Name) ...")
		# Set dimensions of your region. (Below is an example).
		regiononeWestX = int(self.iW * regiononeWestLon)
		regiononeEastX = int(self.iW * regiononeEastLon)
		regiononeNorthY = int(self.iH * regiononeNorthLat)
		regiononeSouthY = int(self.iH * regiononeSouthLat)
		regiononeWidth = regiononeEastX - regiononeWestX + 1
		regiononeHeight = regiononeNorthY - regiononeSouthY + 1
		regiononeWater = 70

		# With all of your parameters set, pass them in to the plot generator.
		self.generatePlotsInRegion(regiononeWater,
		                           regiononeWidth, regiononeHeight,
		                           regiononeWestX, regiononeSouthY,
		                           regiononeGrain, iGrainOne,
		                           self.iFlags, self.iTerrainFlags,
		                           -1, -1,
		                           True, 15,
		                           2, False,
		                           False
		                           )

		# Define additional regions.
		# Regions can overlap or add on to other existing regions.
		# Example of a subcontinent region appended to region one from above:
		NiTextOut("Generating subcontinent for Region One (Python Map_Script_Name) ...")
		scLargeWidth = int(subcontinentLargeHorz * self.iW)
		scLargeHeight = int(subcontinentLargeVert * self.iH)
		scRoll = self.dice.get((regiononeWidth - scLargeWidth), "Large Subcontinent Placement - Map_Script_Name PYTHON")
		scWestX = regiononeWestX + scRoll
		scEastX = scWestX + scLargeWidth
		scNorthY = int(self.iH * subcontinentLargeNorthLat)
		scSouthY = int(self.iH * subcontinentLargeSouthLat)

		# Clever use of dice rolls can inject some randomization in to definitions.
		scShape = self.dice.get(3, "Large Subcontinent Shape - Map_Script_Name PYTHON")
		if scShape == 2: # Massive subcontinent!
			scWater = 55; scGrain = 1; scRift = 2
		elif scShape == 1: # Standard subcontinent.
			scWater = 66; scGrain = 2; scRift = 2
		else: # scShape == 0, Archipelago subcontinent.
			scWater = 77; scGrain = archGrain; scRift = -1

		# Each regional fractal needs its own uniquely defined parameters.
		# With proper settings, there's almost no limit to what can be done.
		self.generatePlotsInRegion(scWater,
		                           scLargeWidth, scLargeHeight,
		                           scWestX, scSouthY,
		                           scGrain, iGrainOne,
		                           self.iRoundFlags, self.iTerrainFlags,
		                           6, 6,
		                           False, 9,
		                           scRift, False,
		                           False
		                           )

		# Once all your of your fractal regions (and other regions! You do not have
		# to make every region a fractal-based region) have been processed, and
		# your plot generation is complete, return the global plot array.
		#
		# All regions have been processed. Plot Type generation completed.
		return self.wholeworldPlotTypes

'''
Regional Variables Key:

iWaterPercent,
iRegionWidth, iRegionHeight,
iRegionWestX, iRegionSouthY,
iRegionGrain, iRegionHillsGrain,
iRegionPlotFlags, iRegionTerrainFlags,
iRegionFracXExp, iRegionFracYExp,
bShift, iStrip,
rift_grain, has_center_rift,
invert_heights
'''

class TerrainGenerator:
	"If iDesertPercent=35, then about 35% of all land will be desert. Plains is similar. \
	Note that all percentages are approximate, as values have to be roughened to achieve a natural look."
	# Rise of Mankind 2.5 start
	def __init__(self, iDesertPercent=20, iPlainsPercent=15, iMarshPercent=10,
	             fIceLatitude=0.7, fTundraLatitude=0.6,
	             fGrassLatitude=0.1, fDesertBottomLatitude=0.2,
	             fDesertTopLatitude=0.5, fracXExp=-1,
	             fracYExp=-1, grain_amount=4):
	# Rise of Mankind 2.5 end
		self.GC = CyGlobalContext()
		self.map = CyMap()

		grain_amount += self.GC.getWorldInfo(self.map.getWorldSize()).getTerrainGrainChange()

		self.grain_amount = grain_amount

		self.iWidth = self.map.getGridWidth()
		self.iHeight = self.map.getGridHeight()

		self.mapRand = self.GC.getGame().getMapRand()

		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
		if self.map.isWrapX(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_X
		if self.map.isWrapY(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_Y

		#C2C
		self.rainmap=CyFractal()
#		self.plains=CyFractal()
#		self.marsh=CyFractal()
		self.variation=CyFractal()

		iDesertPercent += self.GC.getClimateInfo(self.map.getClimate()).getDesertPercentChange()
		iDesertPercent = min(iDesertPercent, 50)
		iDesertPercent = max(iDesertPercent, 0)

		self.DesertPercent = iDesertPercent * 3 / 4
		self.PlainsPercent = iPlainsPercent + iDesertPercent
		self.GrasslandPercent = 100 - iMarshPercent - iPlainsPercent / 4
		self.SaltFlatsPercent = iDesertPercent / 4
		self.DunesPercent = iDesertPercent / 2
		self.ScrubPercent = iDesertPercent
		self.RockyPercent = iDesertPercent + iPlainsPercent / 5
		self.BarrenPercent = iDesertPercent + iPlainsPercent * 2 / 5
		self.LushPercent = 100 - iMarshPercent
		self.MuddyPercent = 100 - iMarshPercent / 2

		#self.iDesertTopPercent = 100
		#self.iDesertBottomPercent = max(0,int(100-iDesertPercent))
		# Rise of Mankind 2.5 start
		# self.iMarshTopPercent = 100
		# self.iMarshBottomPercent = max(0,int(100-iDesertPercent-iMarshPercent))
		# self.iPlainsTopPercent = 100
		# self.iPlainsBottomPercent = max(0,int(100-iDesertPercent-iMarshPercent-iPlainsPercent))
		#self.iPlainsTopPercent = 100
		#self.iPlainsBottomPercent = max(0,int(100-iPlainsPercent))
		#self.iMarshTopPercent = 100
		#self.iMarshBottomPercent = max(0,int(100-iDesertPercent-iPlainsPercent-iMarshPercent))
		# Rise of Mankind 2.5 end
		self.iMountainTopPercent = 75
		self.iMountainBottomPercent = 60

		fIceLatitude += self.GC.getClimateInfo(self.map.getClimate()).getSnowLatitudeChange()
		fIceLatitude = min(fIceLatitude, 1.0)
		fIceLatitude = max(fIceLatitude, 0.0)
		self.fIceLatitude = fIceLatitude

		fTundraLatitude += self.GC.getClimateInfo(self.map.getClimate()).getTundraLatitudeChange()
		fTundraLatitude = min(fTundraLatitude, 1.0)
		fTundraLatitude = max(fTundraLatitude, 0.0)
		self.fTundraLatitude = fTundraLatitude

		self.fTaigaLatitude = fTundraLatitude - (fIceLatitude - fTundraLatitude) / 2.0
		self.fPermafrostLatitude = fTundraLatitude + (fIceLatitude - fTundraLatitude) / 3.0

		#fGrassLatitude += self.GC.getClimateInfo(self.map.getClimate()).getGrassLatitudeChange()
		#fGrassLatitude = min(fGrassLatitude, 1.0)
		#fGrassLatitude = max(fGrassLatitude, 0.0)
		#self.fGrassLatitude = fGrassLatitude

		#fDesertBottomLatitude += self.GC.getClimateInfo(self.map.getClimate()).getDesertBottomLatitudeChange()
		#fDesertBottomLatitude = min(fDesertBottomLatitude, 1.0)
		#fDesertBottomLatitude = max(fDesertBottomLatitude, 0.0)
		#self.fDesertBottomLatitude = fDesertBottomLatitude

		#fDesertTopLatitude += self.GC.getClimateInfo(self.map.getClimate()).getDesertTopLatitudeChange()
		#fDesertTopLatitude = min(fDesertTopLatitude, 1.0)
		#fDesertTopLatitude = max(fDesertTopLatitude, 0.0)
		#self.fDesertTopLatitude = fDesertTopLatitude

		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.initFractals()

	def initFractals(self):

		#C2C
		self.rainmap.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		self.desertThreshold = self.rainmap.getHeightFromPercent(self.DesertPercent)
		self.saltflatsThreshold = self.rainmap.getHeightFromPercent(self.SaltFlatsPercent)
		self.dunesThreshold = self.rainmap.getHeightFromPercent(self.DunesPercent)
		self.scrubThreshold = self.rainmap.getHeightFromPercent(self.ScrubPercent)
		self.plainsThreshold = self.rainmap.getHeightFromPercent(self.PlainsPercent)
		self.rockyThreshold = self.rainmap.getHeightFromPercent(self.RockyPercent)
		self.barrenThreshold = self.rainmap.getHeightFromPercent(self.BarrenPercent)
		self.grasslandThreshold = self.rainmap.getHeightFromPercent(self.GrasslandPercent)
		self.lushThreshold = self.rainmap.getHeightFromPercent(self.LushPercent)
		self.muddyThreshold = self.rainmap.getHeightFromPercent(self.MuddyPercent)

#		self.plains.fracInit(self.iWidth, self.iHeight, self.grain_amount+1, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
#		self.iPlainsTop = self.plains.getHeightFromPercent(self.iPlainsTopPercent)
#		self.iPlainsBottom = self.plains.getHeightFromPercent(self.iPlainsBottomPercent)

#		self.marsh.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
#		self.iMarshTop = self.marsh.getHeightFromPercent(self.iMarshTopPercent)
#		self.iMarshBottom = self.marsh.getHeightFromPercent(self.iMarshBottomPercent)

		self.variation.fracInit(self.iWidth, self.iHeight, self.grain_amount, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		self.terrainIce = self.GC.getInfoTypeForString("TERRAIN_ICE")
		self.terrainPermafrost = self.GC.getInfoTypeForString("TERRAIN_PERMAFROST")
		self.terrainTundra = self.GC.getInfoTypeForString("TERRAIN_TUNDRA")
		self.terrainTaiga = self.GC.getInfoTypeForString("TERRAIN_TAIGA")
		self.terrainRockyCold = self.GC.getInfoTypeForString("TERRAIN_JAGGED")
		self.terrainRocky = self.GC.getInfoTypeForString("TERRAIN_ROCKY")
		self.terrainRockyArid = self.GC.getInfoTypeForString("TERRAIN_BADLAND")
		self.terrainDesert = self.GC.getTERRAIN_DESERT()
		self.terrainPlains = self.GC.getInfoTypeForString("TERRAIN_PLAINS")
		self.terrainGrass = self.GC.getInfoTypeForString("TERRAIN_GRASSLAND")
		self.terrainMarsh = self.GC.getInfoTypeForString("TERRAIN_MARSH")
		self.terrainSaltFlats = self.GC.getInfoTypeForString("TERRAIN_SALT_FLATS")
		self.terrainDunes = self.GC.getInfoTypeForString("TERRAIN_DUNES")
		self.terrainScrub = self.GC.getInfoTypeForString("TERRAIN_SCRUB")
		self.terrainBarren = self.GC.getInfoTypeForString("TERRAIN_BARREN")
		self.terrainLush = self.GC.getInfoTypeForString("TERRAIN_LUSH")
		self.terrainMuddy = self.GC.getInfoTypeForString("TERRAIN_MUDDY")

	def getLatitudeAtPlot(self, iX, iY):
		"""given a point (iX,iY) such that (0,0) is in the NW,
		returns a value between 0.0 (tropical) and 1.0 (polar).
		This function can be overridden to change the latitudes; for example,
		to make an entire map have temperate terrain, or to make terrain change from east to west
		instead of from north to south"""
		lat = abs((self.iHeight / 2) - iY)/float(self.iHeight/2) # 0.0 = equator, 1.0 = pole

		# Adjust latitude using self.variation fractal, to mix things up:
		lat += (128 - self.variation.getHeight(iX, iY))/(255.0 * 5.0)

		# Limit to the range [0, 1]:
		if lat < 0:
			lat = 0.0
		if lat > 1:
			lat = 1.0

		return lat

	def generateTerrain(self):
		terrainData = [0]*(self.iWidth*self.iHeight)
		for x in range(self.iWidth):
			for y in range(self.iHeight):
				iI = y*self.iWidth + x
				terrain = self.generateTerrainAtPlot(x, y)
				terrainData[iI] = terrain

		#remove marsh next to desert
		#for x in range(self.iWidth):
		#	for y in range(self.iHeight):
		#		iIndex = y * self.iWidth + x#

		#		for iDirection in range(CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES):
		#			pPlot = plotCardinalDirection(x, y, CardinalDirectionTypes(iDirection))

		#			if not pPlot.isNone():
		#				iOtherIndex = pPlot.getY() * self.iWidth + pPlot.getX()

		#				if ((terrainData[iIndex] == self.terrainDesert) and (terrainData[iOtherIndex] == self.terrainMarsh)) or ((terrainData[iIndex] == self.terrainMarsh) and (terrainData[iOtherIndex] == self.terrainDesert)):
		#					terrainData[iIndex] = self.terrainPlains
		#					break

		return terrainData

	def generateTerrainAtPlot(self,iX,iY):
		lat = self.getLatitudeAtPlot(iX,iY)

		plot = self.map.plot(iX, iY)

		if (plot.isWater()):
			return self.map.plot(iX, iY).getTerrainType()

		#C2C
		if lat >= self.fIceLatitude:
			terrainVal = self.terrainIce
		elif lat >= self.fPermafrostLatitude:
			terrainVal = self.terrainPermafrost
		elif lat >= self.fTundraLatitude:
			if plot.isHills() and 24 > self.mapRand.get(100, "HillTundraRocky"):
				terrainVal = self.terrainRockyCold
			elif 8 > self.mapRand.get(100, "TundraRocky"):
				terrainVal = self.terrainRockyCold
			else:
				terrainVal = self.terrainTundra
		elif lat >= self.fTaigaLatitude:
			if plot.isHills() and 24 > self.mapRand.get(100, "HillTaigaRocky"):
				terrainVal = self.terrainRockyCold
			elif 8 > self.mapRand.get(100, "TaigaRocky"):
				terrainVal = self.terrainRockyCold
			else:
				terrainVal = self.terrainTaiga
		else:
			rainmapVal = self.rainmap.getHeight(iX, iY)
			#plainsVal = self.plains.getHeight(iX, iY)
			#marshVal = self.marsh.getHeight(iX, iY)
			if rainmapVal < self.saltflatsThreshold:
				terrainVal = self.terrainSaltFlats
			elif rainmapVal < self.dunesThreshold:
				if plot.isHills() and 24 > self.mapRand.get(100, "HillDuneRocky"):
					terrainVal = self.terrainRockyArid
				elif 8 > self.mapRand.get(100, "DuneRocky"):
					terrainVal = self.terrainRockyArid
				else:
					terrainVal = self.terrainDunes
			elif rainmapVal < self.desertThreshold:
				if plot.isHills() and 24 > self.mapRand.get(100, "HillDesertRocky"):
					terrainVal = self.terrainRockyArid
				elif 8 > self.mapRand.get(100, "DesertRocky"):
					terrainVal = self.terrainRockyArid
				else:
					terrainVal = self.terrainDesert
			elif rainmapVal < self.scrubThreshold:
				terrainVal = self.terrainScrub
			elif rainmapVal < self.rockyThreshold:
				terrainVal = self.terrainRocky
			elif rainmapVal < self.barrenThreshold:
				terrainVal = self.terrainBarren
			elif rainmapVal < self.plainsThreshold:
				terrainVal = self.terrainPlains
			elif rainmapVal < self.grasslandThreshold:
				terrainVal = self.terrainGrass
			elif rainmapVal < self.lushThreshold:
				terrainVal = self.terrainLush
			elif rainmapVal < self.muddyThreshold:
				terrainVal = self.terrainMuddy
			else:
				terrainVal = self.terrainMarsh


			#if ((desertVal >= self.iDesertBottom) and (desertVal <= self.iDesertTop) and (lat >= self.fDesertBottomLatitude) and (lat < self.fDesertTopLatitude)):
			#	terrainVal = self.terrainDesert
			#elif ((marshVal >= self.iMarshBottom) and (marshVal <= self.iMarshTop) and plot.isFlatlands() and (lat >= self.fDesertBottomLatitude) and (lat < self.fDesertTopLatitude)):
			#	terrainVal = self.terrainMarsh
			#elif ((plainsVal >= self.iPlainsBottom) and (plainsVal <= self.iPlainsTop)):
			#	terrainVal = self.terrainPlains

		if (terrainVal == TerrainTypes.NO_TERRAIN):
			return self.map.plot(iX, iY).getTerrainType()

		return terrainVal

class FeatureGenerator:
	def __init__(self, iJunglePercent=80, iForestPercent=60, jungle_grain=5, forest_grain=6,  fracXExp=-1, fracYExp=-1):

		self.GC = GC = CyGlobalContext()
		self.map = MAP = GC.getMap()
		self.mapRand = GC.getGame().getMapRand()
		self.jungles = CyFractal()
		self.forests = CyFractal()

		self.iFlags = 0  # Disallow FRAC_POLAR flag, to prevent "zero row" problems.
		if MAP.isWrapX(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_X
		if MAP.isWrapY(): self.iFlags += CyFractal.FracVals.FRAC_WRAP_Y

		self.iGridW = MAP.getGridWidth()
		self.iGridH = MAP.getGridHeight()

		self.iJunglePercent = iJunglePercent
		self.iForestPercent = iForestPercent

		iFeatureGrain = GC.getWorldInfo(MAP.getWorldSize()).getFeatureGrainChange()

		self.jungle_grain = jungle_grain + iFeatureGrain
		self.forest_grain = forest_grain + iFeatureGrain

		self.fracXExp = fracXExp
		self.fracYExp = fracYExp

		self.__initFractals()
		self.__initFeatureTypes()

	def __initFractals(self):
		self.jungles.fracInit(self.iGridW, self.iGridH, self.jungle_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)
		self.forests.fracInit(self.iGridW, self.iGridH, self.forest_grain, self.mapRand, self.iFlags, self.fracXExp, self.fracYExp)

		self.iJungleBottom = self.jungles.getHeightFromPercent((100 - self.iJunglePercent)/2)
		self.iJungleTop = self.jungles.getHeightFromPercent((100 + self.iJunglePercent)/2)
		self.iForestLevel = self.forests.getHeightFromPercent(self.iForestPercent)

	def __initFeatureTypes(self):
		self.featureIce = self.GC.getInfoTypeForString("FEATURE_ICE")
		self.featureJungle = self.GC.getFEATURE_JUNGLE()
		self.featureForest = self.GC.getFEATURE_FOREST()

	def addFeatures(self):
		"adds features to all plots as appropriate"
		for iX in range(self.iGridW):
			for iY in range(self.iGridH):
				self.addFeaturesAtPlot(iX, iY)

	def getLatitudeAtPlot(self, iX, iY):
		"returns a value in the range of 0.0 (tropical) to 1.0 (polar)"
		return abs((self.iGridH/2) - iY)/float(self.iGridH/2) # 0.0 = equator, 1.0 = pole

	def addFeaturesAtPlot(self, iX, iY):
		"adds any appropriate features at the plot (iX, iY) where (0,0) is in the SW"
		lat = self.getLatitudeAtPlot(iX, iY)
		pPlot = self.map.sPlot(iX, iY)

		for iI in range(self.GC.getNumFeatureInfos()):
#			print self.GC.getFeatureInfo(iI).getDescription()
			if pPlot.canHaveFeature(iI):
#				print "Can have feature with probability: %d" % self.GC.getFeatureInfo(iI).getAppearanceProbability()
				if self.mapRand.get(10000, "Add Feature PYTHON") < self.GC.getFeatureInfo(iI).getAppearanceProbability():
#					print "Setting feature"
					variety = -1
					varietynum = self.GC.getFeatureInfo(iI).getNumVarieties()
					if varietynum > 1:
						variety = self.mapRand.get(varietynum, "Add Feature PYTHON")
						#print "Variety choice: %d" % varietynum
					pPlot.setFeatureType(iI, variety)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addIceAtPlot(pPlot, iX, iY, lat)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addJunglesAtPlot(pPlot, iX, iY, lat)

		if (pPlot.getFeatureType() == FeatureTypes.NO_FEATURE):
			self.addForestsAtPlot(pPlot, iX, iY, lat)

	def addIceAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureIce):
			if (self.map.isWrapX() and not self.map.isWrapY()) and (iY == 0 or iY == self.iGridH - 1):
				pPlot.setFeatureType(self.featureIce, -1)
			elif (self.map.isWrapY() and not self.map.isWrapX()) and (iX == 0 or iX == self.iGridW - 1):
				pPlot.setFeatureType(self.featureIce, -1)
			else:
				rand = self.mapRand.get(100, "Add Ice PYTHON")/100.0
				if rand < 8 * (lat - (1.0 - (self.GC.getClimateInfo(self.map.getClimate()).getRandIceLatitude() / 2.0))):
					pPlot.setFeatureType(self.featureIce, -1)
				elif rand < 4 * (lat - (1.0 - self.GC.getClimateInfo(self.map.getClimate()).getRandIceLatitude())):
					pPlot.setFeatureType(self.featureIce, -1)

	def addJunglesAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureJungle):
			iJungleHeight = self.jungles.getHeight(iX, iY)
			if self.iJungleTop >= iJungleHeight >= self.iJungleBottom + (self.iJungleTop - self.iJungleBottom)*self.GC.getClimateInfo(self.map.getClimate()).getJungleLatitude()*lat:
				pPlot.setFeatureType(self.featureJungle, -1)

	def addForestsAtPlot(self, pPlot, iX, iY, lat):
		if pPlot.canHaveFeature(self.featureForest):
			if self.forests.getHeight(iX, iY) >= self.iForestLevel:
				pPlot.setFeatureType(self.featureForest, -1)

def findStartingPlot(playerID, validFn = None):
	GC = CyGlobalContext()
	MAP = GC.getMap()
	player = GC.getPlayer(playerID)

	player.AI_updateFoundValues(True)

	iRange = player.startingPlotRange()
	iPass = 0

	while (True):
		iBestValue = 0
		pBestPlot = None
		for iX in range(MAP.getGridWidth()):
			for iY in range(MAP.getGridHeight()):
				if validFn != None and not validFn(playerID, iX, iY):
					continue
				pLoopPlot = MAP.plot(iX, iY)
				val = pLoopPlot.getFoundValue(playerID)
				if val > iBestValue:
					iBestValue = val
					pBestPlot = pLoopPlot

		if pBestPlot != None:
			return MAP.plotNum(pBestPlot.getX(), pBestPlot.getY())

		print ("player", playerID, "pass", iPass, "failed")

		iPass += 1

	return -1

def argmin(list):
	best = None
	best_index = None
	for i in range(len(list)):
		val = list[i]
		if (best == None) or (val < best):
			best_index = i
			best = val
	return (best_index, best)

def pointInRect(point, rect):
	x,y=point
	rectx,recty,rectw,recth = rect
	if rectx <= x < rectx + rectw:
		if recty <= y < recty + recth:
			return True
	return False


class BonusBalancer:
	def __init__(self):
		self.GC = CyGlobalContext()
		self.map = CyMap()

		# Rise of Mankind mod start
		self.resourcesToBalance = ('BONUS_BAUXITE_ORE', 'BONUS_COAL', 'BONUS_COPPER_ORE', 'BONUS_HORSE', 'BONUS_IRON_ORE', 'BONUS_OIL', 'BONUS_URANIUM', 'BONUS_SULPHUR', 'BONUS_RUBBER')
		# Rise of Mankind mod end

		self.resourcesToEliminate = ('BONUS_MARBLE', )

	def isSkipBonus(self, iBonusType):
		type_string = self.GC.getBonusInfo(iBonusType).getType()

		return ((type_string in self.resourcesToBalance) or (type_string in self.resourcesToEliminate))


	def isBonusValid(self, eBonus, pPlot, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent):
		"Returns True if we can place a bonus here"

		iX, iY = pPlot.getX(), pPlot.getY()

		if (not bIgnoreOneArea) and self.GC.getBonusInfo(eBonus).isOneArea():
			if self.map.getNumBonuses(eBonus) > 0:
				if self.map.getArea(pPlot.getArea()).getNumBonuses(eBonus) == 0:
					return False

		if not bIgnoreAdjacent:
			for iI in range(DirectionTypes.NUM_DIRECTION_TYPES):
				pLoopPlot = plotDirection(iX, iY, DirectionTypes(iI))
				if not pLoopPlot.isNone():
					if (pLoopPlot.getBonusType(-1) != -1) and (pLoopPlot.getBonusType(-1) != eBonus):
						return False

		if not bIgnoreUniqueRange:
			uniqueRange = self.GC.getBonusInfo(eBonus).getUniqueRange()
			for iDX in range(-uniqueRange, uniqueRange+1):
				for iDY in range(-uniqueRange, uniqueRange+1):
					pLoopPlot = plotXY(iX, iY, iDX, iDY)
					if not pLoopPlot.isNone() and pLoopPlot.getBonusType(-1) == eBonus:
						return False

		return True

	def normalizeAddExtras(self):

		for i in range(self.GC.getMAX_PC_PLAYERS()):
			if (self.GC.getPlayer(i).isAlive()):
				start_plot = self.GC.getPlayer(i).getStartingPlot() # returns a CyPlot
				startx, starty = start_plot.getX(), start_plot.getY()

				plots = [] # build a list of the plots near the starting plot
				for dx in range(-5,6):
					for dy in range(-5,6):
						x,y = startx+dx, starty+dy
						pLoopPlot = self.map.plot(x,y)
						if not pLoopPlot.isNone():
							plots.append(pLoopPlot)

				resources_placed = []
				for pass_num in range(4):
					bIgnoreUniqueRange  = pass_num >= 1
					bIgnoreOneArea 		= pass_num >= 2
					bIgnoreAdjacent 	= pass_num >= 3

					for bonus in range(self.GC.getNumBonusInfos()):
						type_string = self.GC.getBonusInfo(bonus).getType()
						if (type_string not in resources_placed) and (type_string in self.resourcesToBalance):
							for (pLoopPlot) in plots:
								if (pLoopPlot.canHaveBonus(bonus, True)):
									if self.isBonusValid(bonus, pLoopPlot, bIgnoreUniqueRange, bIgnoreOneArea, bIgnoreAdjacent):
										pLoopPlot.setBonusType(bonus)
										resources_placed.append(type_string)
										#print "placed", type_string, "on pass", pass_num
										break # go to the next bonus


###############################################################################
## Functions specific for C2C
###############################################################################
# Check that each plot around this plot is sea/coast
def isSinglePlotIsland(pPlot):
	iPlotX = pPlot.getX()
	iPlotY = pPlot.getY()
	MAP = CyMap()

	iX = iPlotX - 1
	while iX < iPlotX + 2:
		iY = iPlotY - 1
		while iY < iPlotY + 2:
			if (iX != iPlotX or iY != iPlotY) and not MAP.plot(iX, iY).isWater():
				return False
			iY += 1
		iX += 1
	return True


def placeSealOrWalrus(iPlot, iSeaLion, iWalrus, bNorth):
	mapRand = CyGlobalContext().getGame().getMapRand()
	if bNorth and mapRand.get(3, 'SealionResource') == 0:
		iPlot.setBonusType(iWalrus)
	else:
		iPlot.setBonusType(iSeaLion)
	return


def c2CMapReport(sWhen):
	countPlotType = [0, 0, 0, 0]
	countTerrain = {}
	countFeature = {}
	countResource = {}
	countImprovement = {}
	map = CyMap()
	GC = CyGlobalContext()
	iNumPlots = map.numPlots()

	iMapWidth = map.getGridWidth()
	iEquatorPlot = map.getGridHeight()/2
	iWestRightEdge = iMapWidth/3
	iCentralRightEdge = iMapWidth - iWestRightEdge

	for i in range(iNumPlots):
		iPlot = map.plotByIndex(i)
		iPlotType = iPlot.getPlotType()

		# Identify where on the map the plot is
		iLatitude = iPlot.getLatitude()
		iLongitude = iPlot.getLongitude()
		iPlotX = iPlot.getX()
		iPlotY = iPlot.getY()
		if iPlotY > iEquatorPlot:
			if iPlotX > iCentralRightEdge:
				iHemisphere = "Asian"
			elif iPlotX > iWestRightEdge:
				iHemisphere = "European"
			else:
				iHemisphere = "North American"
		else:
			if iPlotX > iCentralRightEdge:
				iHemisphere = "Oceania"
			elif iPlotX > iWestRightEdge:
				iHemisphere = "African"
			else:
				iHemisphere = "South American"

		if iPlotType == PlotTypes.PLOT_OCEAN:
			countPlotType[0] += 1
		elif iPlotType == PlotTypes.PLOT_LAND:
			countPlotType[1] += 1
		elif iPlotType == PlotTypes.PLOT_HILLS:
			countPlotType[2] += 1
		elif iPlotType == PlotTypes.PLOT_PEAK:
			countPlotType[3] += 1

		iTerrain = iPlot.getTerrainType()
		if iTerrain in countTerrain:
			countTerrain[iTerrain] += 1
		else:
			countTerrain[iTerrain] = 1
		iFeature = iPlot.getFeatureType()
		if iFeature > -1:
			if iFeature in countFeature:
				countFeature[iFeature] += 1
			else:
				countFeature[iFeature] = 1
		iResource = iPlot.getBonusType(-1)
		if iResource > -1:
			if (iHemisphere,iResource) in countResource:
				countResource[iHemisphere,iResource] += 1
			else:
				countResource[iHemisphere,iResource] = 1
		iImprovement = iPlot.getImprovementType()
		if iImprovement > -1:
			if iImprovement in countImprovement:
				countImprovement[iImprovement] += 1
			else:
				countImprovement[iImprovement] = 1

	print "[MST] ####################################################################### C2C:Map Statistics ###"
	sprint = "[MST] %s" % (sWhen) + "\n\n"
	sprint += "[MST]   Statistics of actual Terrain, Features etc. \n"
	sprint += "[MST]   ------------------------------------------- \n"
	sprint += "[MST]   %i Width x %i Height = %i Plots,  %2i Players" % (map.getGridWidth(), map.getGridHeight(), iNumPlots, GC.getGame().countCivPlayersEverAlive() ) + "\n\n"
	f1 = (countPlotType[0]*100.0) / iNumPlots
	f2 = (countPlotType[1]*100.0) / iNumPlots
	f3 = (countPlotType[2]*100.0) / iNumPlots
	f4 = (countPlotType[3]*100.0) / iNumPlots
	f5 = (countPlotType[1]+countPlotType[2])*100.0 / iNumPlots
	sprint += "[MST]   Plots: #0 - PLOT_OCEAN ( %4i )  = %4.1f%s Water" 		% (countPlotType[0], f1, '%%') + "\n"
	sprint += "[MST]   Plots: #1 - PLOT_LAND  ( %4i )  = %4.1f%s Land   }" 	% (countPlotType[1], f2, '%%') + "\n"
	sprint += "[MST]   Plots: #2 - PLOT_HILLS ( %4i )  = %4.1f%s Hills  }= %4.1f%s Usable Land" % (countPlotType[2], f3, '%%', f5, '%%') + "\n"
	sprint += "[MST]   Plots: #3 - PLOT_PEAK  ( %4i )  = %4.1f%s Peaks" 		% (countPlotType[3], f4, '%%') + "\n\n"

	# Display actual Mod-Terrain
	for ter in countTerrain:
		type_string = GC.getTerrainInfo(ter).getType()
		sprint += "[MST]   Terrain: #%2i - %s ( %i ) \n" % (ter, type_string, countTerrain[ter])
	sprint += "\n"

	# Display actual Mod-Features
	for feat in countFeature:
		type_string = GC.getFeatureInfo(feat).getType()
		sprint += "[MST]   Feature: #%2i - %s ( %i ) \n" % (feat, type_string, countFeature[feat])
	sprint += "\n"

	# Display actual Mod-Resources
	# countResource.sort()
	boni = 0
	dontReportBonusClass = [GC.getInfoTypeForString("BONUSCLASS_CULTURE"), GC.getInfoTypeForString("BONUSCLASS_MANUFACTURED"), GC.getInfoTypeForString("BONUSCLASS_GENMODS"), GC.getInfoTypeForString("BONUSCLASS_WONDER")]
	while boni < GC.getNumBonusInfos():
		type_string = GC.getBonusInfo(boni).getType()
		class_int = GC.getBonusInfo(boni).getBonusClassType()
		if class_int not in dontReportBonusClass:
			region = "North American"
			if (region,boni) in countResource:
				sprint += "[MST]   Bonus: #%2i - %s ( %s - %i ) " % (boni, type_string, region, countResource[region,boni])
			else:
				sprint += "[MST]   Bonus: #%2i - %s ( %s - %i ) " % (boni, type_string, region, 0)
			region = "European"
			if (region,boni) in countResource:
				sprint += "( %s - %i ) " % (region, countResource[region,boni])
			else:
				sprint += "( %s - %i ) " % (region, 0)
			region = "Asian"
			if (region,boni) in countResource:
				sprint += "( %s - %i ) " % (region, countResource[region,boni])
			else:
				sprint += "( %s - %i ) " % (region, 0)
			region = "South American"
			if (region,boni) in countResource:
				sprint += "( %s - %i ) " % (region, countResource[region,boni])
			else:
				sprint += "( %s - %i ) " % (region, 0)
			region = "African"
			if (region,boni) in countResource:
				sprint += "( %s - %i ) " % (region, countResource[region,boni])
			else:
				sprint += "( %s - %i ) " % (region, 0)
			region = "Oceania"
			if (region,boni) in countResource:
				sprint += "( %s - %i ) \n" % (region, countResource[region,boni])
			else:
				sprint += "( %s - %i ) \n" % (region, 0)

		boni = boni + 1

	sprint += "\n"

	# Display actual Mod-Improvements
	for imp in countImprovement:
		type_string = GC.getImprovementInfo(imp).getType()
		sprint += "[MST]   Improvement: #%3i  %s ( %i ) \n" % (imp, type_string, countImprovement[imp])
	sprint += "\n"

	print sprint
	print "[MST] ####################################################################### C2C:MapStatistics ###"

	return

def placeC2CBonuses():
	GC = CyGlobalContext()
	MAP = GC.getMap()
	GAME = GC.getGame()
	mapRand = GAME.getMapRand()

	bGoodyHuts = not GAME.isOption(GameOptionTypes.GAMEOPTION_NO_GOODY_HUTS)

	c2CMapReport("Before adjustment")

	cNORTH = "NORTHERN HEMISPHERE"
	cSOUTH = "SOUTHERN HEMISPHERE"
	cWEST = "WESTERN HEMISPHERE"
	cCENTRAL = "CENTRAL HEMISPHERE"
	cEAST = "EASTERN HEMISPHERE"

	iMapWidth = MAP.getGridWidth()
	iEquatorPlot = MAP.getGridHeight()/2
	iWestRightEdge = iMapWidth/3
	iCentralRightEdge = iMapWidth - iWestRightEdge

	iSeaLion = GC.getInfoTypeForString("BONUS_SEA_LION_AND_SEAL")
	iWalrus = GC.getInfoTypeForString("BONUS_WALRUS")
	iKangaroo = GC.getInfoTypeForString("BONUS_KANGAROO")
	iLlama = GC.getInfoTypeForString("BONUS_LLAMA")
	iDeer = GC.getInfoTypeForString("BONUS_DEER")
	iBison = GC.getInfoTypeForString("BONUS_BISON")
	iRabbit = GC.getInfoTypeForString("BONUS_RABBIT")
	iGuineaPig = GC.getInfoTypeForString("BONUS_GUINEA_PIGS")
	iElephant = GC.getInfoTypeForString("BONUS_ELEPHANTS")
	aNotInNewWorld = [
		GC.getInfoTypeForString("BONUS_HORSE"),
		GC.getInfoTypeForString("BONUS_DONKEY"),
		GC.getInfoTypeForString("BONUS_CAMEL"),
		GC.getInfoTypeForString("BONUS_COW"),
		iElephant
	]

	terrainCoast = GC.getInfoTypeForString("TERRAIN_COAST")
	terrainSea = GC.getInfoTypeForString("TERRAIN_SEA")
	terrainOcean = GC.getInfoTypeForString("TERRAIN_OCEAN")

	terrainShore = GC.getInfoTypeForString("TERRAIN_LAKE_SHORE")
	terrainLake = GC.getInfoTypeForString("TERRAIN_LAKE")

	terrainPolarCoast = GC.getInfoTypeForString("TERRAIN_COAST_POLAR")
	terrainPolarSea = GC.getInfoTypeForString("TERRAIN_SEA_POLAR")
	terrainPolarOcean = GC.getInfoTypeForString("TERRAIN_OCEAN_POLAR")
	terrainTropicalCoast = GC.getInfoTypeForString("TERRAIN_COAST_TROPICAL")
	terrainTropicalSea = GC.getInfoTypeForString("TERRAIN_SEA_TROPICAL")
	terrainTropicalOcean = GC.getInfoTypeForString("TERRAIN_OCEAN_TROPICAL")

	aCoastTerrains = [terrainCoast, terrainPolarCoast, terrainTropicalCoast]

	terrainMarsh = GC.getInfoTypeForString("TERRAIN_MARSH")

	featureMangrove = GC.getInfoTypeForString("FEATURE_MANGROVE")

	improvementGoodyIsland = GC.getInfoTypeForString("IMPROVEMENT_GOODY_ISLAND")

	for i in range(MAP.numPlots()):
		plot = MAP.plotByIndex(i)

		# Identify where on the map the plot is
		iLatitude = plot.getLatitude()
		iLongitude = plot.getLongitude()
		x = plot.getX()
		y = plot.getY()
		if y > iEquatorPlot:
			iNS_Hemisphere = cNORTH
		else:
			iNS_Hemisphere = cSOUTH
		if x > iCentralRightEdge:
			iEW_Hemisphere = cEAST
		elif x > iWestRightEdge:
			iEW_Hemisphere = cCENTRAL
		else:
			iEW_Hemisphere = cWEST

		# Get plot terrain, feature and bonus
		iTerrain = plot.getTerrainType()
		iFeature = plot.getFeatureType()
		iResource = plot.getBonusType(-1)

		if plot.isLake():
			if iTerrain == terrainCoast:
				plot.setTerrainType(terrainShore, False, False)
			elif iTerrain in (terrainSea, terrainOcean):
				plot.setTerrainType(terrainLake, False, False)
		elif plot.isWater():
			## Figure out if it is Sea ie if it is ocean next to coast
			if iTerrain == terrainOcean:
				for j in range(x - 1, x + 2):
					for k in range(y - 1, y + 2):
						plotX = MAP.plot(j, k)
						if not plotX.isNone() and plotX.getTerrainType() in aCoastTerrains:
							plot.setTerrainType(terrainSea, False, False)
							iTerrain = terrainSea
							break
					else: continue
					break
			## Figure out if it is Polar, Temperate or Tropical
			if iLatitude > 66.5:
				## Polar
				if iTerrain == terrainCoast:
					plot.setTerrainType(terrainPolarCoast, False, False)
				elif iTerrain == terrainOcean:
					plot.setTerrainType(terrainPolarOcean, False, False)
				elif iTerrain == terrainSea:
					plot.setTerrainType(terrainPolarSea, False, False)
			elif iLatitude < 23.5:
				## Tropical
				if iTerrain == terrainCoast:
					plot.setTerrainType(terrainTropicalCoast, False, False)
				elif iTerrain == terrainOcean:
					plot.setTerrainType(terrainTropicalOcean, False, False)
				elif iTerrain == terrainSea:
					plot.setTerrainType(terrainTropicalSea, False, False)

			if bGoodyHuts and iFeature == -1 and (iTerrain == terrainCoast or iTerrain == terrainSea) and not mapRand.get(19, 'GoodyIsland'):
				plot.setImprovementType(improvementGoodyIsland)

		elif not plot.isPeak():
			if iResource == -1 and isSinglePlotIsland(plot) and not mapRand.get(3, 'add bonus'):
				# Single plot island without any resources
				if iLatitude >= 64:
					placeSealOrWalrus(plot, iSeaLion, iWalrus, iNS_Hemisphere == cNORTH)

				elif mapRand.get(2, 'SealionResource') == 0:
					plot.setBonusType(iSeaLion)
					iResource = iSeaLion

			if iTerrain == terrainMarsh and iLatitude < 30 and not plot.isHills() and plot.isCoastal():
				# tropical and semi-tropical marsh coastal plot
				plot.setFeatureType(featureMangrove, 0)
		'''
		Replace resources that the map generator may have put on the wrong parts of the map
		since it only has north/south control not east/west control
		'''
		# Replace Elephants not in north east or central south.
		if iResource == iElephant and (iNS_Hemisphere == cSOUTH and iEW_Hemisphere != cCENTRAL or iNS_Hemisphere == cNORTH and iEW_Hemisphere != cEAST):
			if plot.isCoastal():
				plot.setBonusType(iSeaLion)
				iResource = iSeaLion
			else:
				plot.setBonusType(iKangaroo)
				iResource = iKangaroo
		# Replace Kangaroos from northen hemisphere and South America and African
		if iResource == iKangaroo:
			if iNS_Hemisphere == cNORTH:
				if mapRand.get(99, 'Bonus') < 50:
					plot.setBonusType(iDeer)
					iResource = iDeer
				else:
					plot.setBonusType(iBison)
					iResource = iBison
			elif iEW_Hemisphere == cWEST:
				plot.setBonusType(iLlama)
				iResource = iLlama
			elif iEW_Hemisphere == cCENTRAL:
				if mapRand.get(99, 'Bonus') < 50:
					plot.setBonusType(iDeer)
					iResource = iDeer
				else:
					plot.setBonusType(iBison)
					iResource = iBison
		# Replace Guinea Pigs from everywhere except South America
		if iResource == iGuineaPig and (iNS_Hemisphere != cSOUTH or iEW_Hemisphere != cWEST or iLatitude < 25):
			plot.setBonusType(iRabbit)
			iResource = iRabbit
		# Replace all quadripeds with Kangaroos in Oceania
		if iNS_Hemisphere == cSOUTH and iEW_Hemisphere == cEAST and iLatitude > 10\
		and (iResource == iDeer or iResource == iBison or iResource in aNotInNewWorld):
			plot.setBonusType(iKangaroo)
		# Replace rabbits from South America and Oceania
		if iNS_Hemisphere == cSOUTH and iResource == iRabbit:
			if iEW_Hemisphere == cEAST:
				plot.setBonusType(iKangaroo)
			elif iEW_Hemisphere == cWEST:
				plot.setBonusType(iGuineaPig)

	c2CMapReport("After adjustment")

	## For placing Natural Wonders ##
	import NaturalWonders
	NaturalWonders.NaturalWonders().placeNaturalWonders()
	## Natural Wonders ##

	# c2CMapReport("After placing Natural Wonders")

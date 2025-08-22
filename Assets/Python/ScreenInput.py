## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005

# Class to decipher and make screen input easy to read...
class ScreenInput:

	# Init call...
	def __init__ (self, argsList):
		# Bounds checking - ensure we have enough arguments
		if not isinstance(argsList, (list, tuple)):
			raise TypeError("argsList must be a list or tuple")
		
		if len(argsList) < 15:
			raise ValueError(f"argsList must contain at least 15 elements, got {len(argsList)}")
		
		# Basic assignments with bounds safety
		self.eNotifyCode = argsList[0]
		self.iData = argsList[1]
		self.uiFlags = argsList[2]
		self.iItemID = argsList[3]
		self.ePythonFileEnum = argsList[4]
		self.szFunctionName = argsList[5]
		
		# Type validation for critical boolean fields
		self.bShift = self._validate_boolean(argsList[6], "bShift")
		self.bCtrl = self._validate_boolean(argsList[7], "bCtrl")
		self.bAlt = self._validate_boolean(argsList[8], "bAlt")
		
		# Type validation for critical integer fields
		self.iMouseX = self._validate_integer(argsList[9], "iMouseX")
		self.iMouseY = self._validate_integer(argsList[10], "iMouseY")
		self.iButtonType = self._validate_integer(argsList[11], "iButtonType")
		self.iData1 = self._validate_integer(argsList[12], "iData1")
		self.iData2 = self._validate_integer(argsList[13], "iData2")
		
		# Final boolean field
		self.bOption = self._validate_boolean(argsList[14], "bOption")

	# Helper method for boolean validation
	def _validate_boolean(self, value, field_name):
		"""Validate that a value is boolean, with fallback for integer 0/1"""
		if isinstance(value, bool):
			return value
		elif isinstance(value, int) and value in (0, 1):
			return bool(value)
		else:
			raise TypeError(f"{field_name} must be a boolean or integer 0/1, got {type(value).__name__}: {value}")

	# Helper method for integer validation  
	def _validate_integer(self, value, field_name):
		"""Validate that a value is an integer, explicitly rejecting booleans"""
		if isinstance(value, bool):
			raise ValueError(f"{field_name} must be an integer, not a boolean. Got {value}")
		elif isinstance(value, int):
			return value
		elif isinstance(value, float) and value.is_integer():
			return int(value)
		else:
			raise TypeError(f"{field_name} must be an integer, got {type(value).__name__}: {value}")

	# NotifyCode
	def getNotifyCode (self):
		return self.eNotifyCode

	# Data
	def getData (self):
		return self.iData

	# Flags
	def getFlags (self):
		return self.uiFlags

	# Item ID
	def getID (self):
		return self.iItemID

	# Python File
	def getPythonFile (self):
		return self.ePythonFileEnum

	# Function Name...
	def getFunctionName (self):
		return self.szFunctionName

	# Shift Key Down
	def isShiftKeyDown (self):
		return self.bShift

	# Ctrl Key Down
	def isCtrlKeyDown (self):
		return self.bCtrl

	# Alt Key Down
	def isAltKeyDown (self):
		return self.bAlt

	# X location of the mouse cursor
	def getMouseX (self):
		return self.iMouseX

	# Y location of the mouse cursor
	def getMouseY (self):
		return self.iMouseY

	# WidgetType
	def getButtonType (self):
		return self.iButtonType

	# Widget Data 1
	def getData1 (self):
		return self.iData1

	# Widget Data 2
	def getData2 (self):
		return self.iData2

	# Widget Option
	def getOption (self):
		return self.bOption
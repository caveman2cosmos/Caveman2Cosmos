#
# CvTranslator.py
#
# Exe will call this function for all text it handles, sending one line at a time into it as input.
# To activate the callback: Change the DllExport getUSE_FINISH_TEXT_CALLBACK() to return true.
# The string returned by this function will replace the string that was sent in for whatever the exe plans to do with it.
# Unlikely that this will ever be useful for anything; maybe some sort of debugging perhaps.
def finishText(argsList):
	return argsList[0]

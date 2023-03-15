//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvXMLLoadUtilitySetMod.h
//
//  AUTHOR:  Vincent Veldman  --  11/2007
//
//  PURPOSE: Group of functions for Vectorized Enumeration and MLF for Civilization 4 BtS
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilitySetMod.h"

namespace CvXMLLoadUtilitySetMod
{
	// In the following method we set the order of loading properly
	void setModLoadControlDirArray(bool bSetModControl)
	{
		PROFILE_EXTRA_FUNC();
		// basic variables
		GC.resetModLoadControlVector();
		if (!bSetModControl)
		{
			GC.setModLoadControlVector("Modules");
			return;
		}

		//// To check when bLoad = 1 if the module is valid
		//CvXMLLoadUtilityModTools p_szDirName;
		/************************************************************************************************/
		/* Afforess	                  Start		 06/15/10                                               */
		/*                                                                                              */
		/*                                                                                              */
		/************************************************************************************************/
		CvString szModDirectory = GC.getInitCore().getDLLPath() + "\\";
		/************************************************************************************************/
		/* Afforess	                     END                                                            */
		/************************************************************************************************/

		bool bModuleExist = false;	// Valid Module?

		DEBUG_LOG("MLF.log", "\n\nThe game will now load the modules into the load vector in the order set by the MLF files:");

		// Initialization of a temp LoadArray
		std::vector< std::vector<bool> > aabLoaded(GC.getNumModLoadControlInfos()); //allocate the rows

		for (int iInfosSet = 0; iInfosSet < GC.getNumModLoadControlInfos(); iInfosSet++)
		{
			const CvModLoadControlInfo& infoSets = GC.getModLoadControlInfos(iInfosSet);
			aabLoaded[iInfosSet].resize(infoSets.getNumModules(), false); // allocate the columns

			//loop through the modules of each MLF
			for (int iSet = 0; iSet < infoSets.getNumModules(); iSet++)
			{
				// set the values according to the InfoClass( CvInfos.cpp )
				if (infoSets.isLoad(iSet))
				{
					aabLoaded[iInfosSet][iSet] = true;
				}
			}
		}

		// more basic variables
		bool bContinue = true;
		while (bContinue)	//we start with iDirDepth at the highest value(set during the MLF loading)
		{
			int iDirDepthTemp = 0;		//we don't want to change the global value
			int iInfosLoad = 0;
			int iLoad = 0;

			//Set the top level MLF to have a start to loop through
			for (; iLoad < GC.getModLoadControlInfos(0).getNumModules(); iLoad++)
			{
				if (aabLoaded[0][iLoad])
				{
					break;  //we found next iLoad
				}
			}

			while (bContinue)
			{
				bContinue = false;
				bool bHitDeepestDepthLoop = false;
				//loop through all the MLF's
				for (int iiInfos = 0; iiInfos < GC.getNumModLoadControlInfos() && !bHitDeepestDepthLoop; iiInfos++)
				{
					// only loop through files that are actually 1 dir deeper
					if (GC.getModLoadControlInfos(iiInfos).getDirDepth() == iDirDepthTemp + 1)
					{
						// Check if the Loop Module is a Child of the actual Module which we will load if no Children will be found.
						if (CvString::format("%s\\", GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str()) == GC.getModLoadControlInfos(iiInfos).getParentFolder().c_str())
						{
							//loop through the modules of each MLF
							for (int ii = 0; ii < GC.getModLoadControlInfos(iiInfos).getNumModules() && !bHitDeepestDepthLoop; ii++)
							{
								//Loaded already? Don't need an endless loop
								if (aabLoaded[iiInfos][ii])
								{
									//set the loading module to the current module
									iInfosLoad = iiInfos;
									iLoad = ii;

									//The first valid we find on this level will be the first to load,
									//so we abord further search inside this class, and Continue the loop to the next level
									//To see if this new Module itself has another MLF the next level
									bHitDeepestDepthLoop = true;
									iDirDepthTemp++;

									bContinue = true;
								}
							}
						}
					}
				}
			}

			// means we are loading something which is not the deepest in directory structure that has been found valid to load...
			if (GC.getModLoadControlInfos(iInfosLoad).getDirDepth() != iDirDepthTemp)
			{
				FErrorMsg("Something ain't right with the parental MLF disabling function, put a bug report on http://www.worldofcivilization.net/bugtracker/bugtracker.htm, please supply your MLF configuration. All of them!");
				break;
			}

			//load the deepest file
			if (aabLoaded[iInfosLoad][iLoad])
			{
				// check for valid xml files
				if (isValidModule(bModuleExist, (szModDirectory + GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str()).c_str(), GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str(), CvString::format(".xml").c_str())/*(int)aszValidFilesVerification.size() > 0*/)	// does ANY valid xml file exist?
				{
					// if valid, module XML file(s) exist
					// note: if dir isn't valid, of course xml's for that dir aren't valid either
					DEBUG_LOG("MLF.log", "Load Priority: %d, \"%s\"", GC.getModLoadControlVectorSize(), GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str());
					GC.setModLoadControlVector(GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str());
				}
				else
				{
					// if not valid, module XML file(s) doesn't exist
					DEBUG_LOG("MLF.log", "No valid module: \"%s\"", GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str());
				}
				aabLoaded[iInfosLoad][iLoad] = false;
			}
			else
			{
				DEBUG_LOG("MLF.log", "ERROR Vector element: %d, \"%s\", GC.getModLoadControlVectorSize(), You shouldn't have come here!", GC.getModLoadControlInfos(iInfosLoad).getModuleFolder(iLoad).c_str());
				FAssertMsg(aabLoaded[iInfosLoad][iLoad], "Something is wrong with the MLF Array");
			}

			// iInfosLoad is A child of the iInfosParentLoop, we have to check if all the children are loaded now
			// If there are no children to load anymore, we must disable the parent, so it's not looped through on the next run
			int iBreakEndless = 0;	// this is not needed for the code, just a security fix

			// If iDirDepth is 0 we only have 1 MLF anyway..no need to loop and spoil CPU power
			while (iDirDepthTemp > 0)
			{
				// this code is just a security thing, not important for the actual code..
				iBreakEndless++;
				if (iBreakEndless >= 20)	//I assume noone will go 20 depths in dir structure anyway
				{
					FErrorMsg("Something ain't right with the parental MLF disabling function, put a bug report on http://www.worldofcivilization.net/bugtracker/bugtracker.htm, please supply your MLF configuration. All of them!");
					break;
				}

				// actual code below
				iDirDepthTemp--;
				// loop through ALL MLF infos...
				for (int IloopInfosDirDepth = 0; IloopInfosDirDepth < GC.getNumModLoadControlInfos(); IloopInfosDirDepth++)
				{
					// verify all MLF for their dir depth, only 1 higher(dir Depth lower) then the last loaded MLF needs to be checked
					if (GC.getModLoadControlInfos(IloopInfosDirDepth).getDirDepth() == iDirDepthTemp)
					{
						// by default we assume any parent we find must be disabled
						bool bParentDisable = true;

						// loop throuhg the modules of every MLF to see if we can find the parent inside this one..
						for (int IloopInfosModDirectory = 0; IloopInfosModDirectory < GC.getModLoadControlInfos(IloopInfosDirDepth).getNumModules(); IloopInfosModDirectory++)
						{
							// parent module of our latest loaded module?
							if (CvString::format("%s\\", GC.getModLoadControlInfos(IloopInfosDirDepth).getModuleFolder(IloopInfosModDirectory).c_str()) == GC.getModLoadControlInfos(iInfosLoad).getParentFolder().c_str())
							{
								//only disable if this is the actual parent

								// Loop through the loaded MLF if everything is now set to false?
								for (iLoad = 0; iLoad < GC.getModLoadControlInfos(iInfosLoad).getNumModules(); iLoad++)
								{
									// if just ANY dir is still active, don't disable the parent
									if (aabLoaded[iInfosLoad][iLoad])
									{
										bParentDisable = false;
									}
								}

								// both conditions are met, we have a Parent, and it has no children left needed to be loaded
								if (bParentDisable)
								{
									aabLoaded[IloopInfosDirDepth][IloopInfosModDirectory] = false;

									// we updated 1 entry, so we have to continue the loop with the new IInfosLoad and iLoad
									iInfosLoad = IloopInfosDirDepth;
								}
							}
						}
					}
				}

				//break when we just finished checking the top level
				if (iDirDepthTemp == 0)
				{
					FASSERT_NOT_NEGATIVE(iDirDepthTemp); //should never reach negative values!!
					break;
				}
			}

			// Check if we must continue
			bContinue = false;
			//loop through all the MLF's
			for (int iInfos = 0; iInfos < GC.getNumModLoadControlInfos(); iInfos++)
			{
				//loop through the modules of each MLF
				for (int iIJ = 0; iIJ < GC.getModLoadControlInfos(iInfos).getNumModules(); iIJ++)
				{
					// As long as Modules need to be loaded, we continue
					if (aabLoaded[iInfos][iIJ])
					{
						bContinue = true;
						break;
					}
				}
				if (bContinue) break;
			}
		}
		DEBUG_LOG("MLF.log", "Finished the MLF, you will now continue loading regular XML files");
	}

	void MLFEnumerateFiles(
		std::vector<CvString>& aszFiles,
		const CvString& refcstrRootDirectory,
		const CvString& refcstrModularDirectory,
		const CvString& refcstrExtension,
		bool bSearchSubdirectories)
	{
		PROFILE_EXTRA_FUNC();
		CvString		strFilePath;		// Filepath
		CvString		strModPath;			// Modules path
		CvString		strPattern;			// Pattern
		CvString		strExtension;		// Extension
		CvString		compareCStrExtension; //Compare with refcstrExtension
		HANDLE          hFile;				// Handle to file
		WIN32_FIND_DATA FileInformation;	// File information

		CvString szDebugBuffer;

		strPattern = refcstrRootDirectory + "\\*.*";

		DEBUG_LOG("CvXMLLoadUtilitySetMod_MLFEnumerateFiles.log", "=== MLFEnumerateFiles begin in root directory %s ===", refcstrRootDirectory.c_str());

		hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FileInformation.cFileName[0] != '.')
				{
					//strFilePath.erase();
					strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;
					strModPath = refcstrModularDirectory + "\\" + FileInformation.cFileName;

					if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bSearchSubdirectories)
						{
							DEBUG_LOG("CvXMLLoadUtilitySetMod_MLFEnumerateFiles.log", " * Search subdirectory %s", strFilePath.c_str());

							// Search subdirectory
							MLFEnumerateFiles(aszFiles, strFilePath, strModPath, refcstrExtension);
						}
					}
					else
					{
						// Check extension
						strExtension = FileInformation.cFileName;
						//force lowercase for comparison
						int length = strExtension.size();
						for (int i = 0; i < length; ++i)
						{
							strExtension[i] = tolower(strExtension[i]);
						}
						if (strExtension.rfind("_civ4") != std::string::npos)
						{
							strExtension = strExtension.substr(strExtension.rfind("_civ4") + 1);
						}
						else if (strExtension.rfind("globaldefines") != std::string::npos)
						{
							strExtension = strExtension.substr(strExtension.rfind("globaldefines"));
						}

						//force lowercase for comparison
						compareCStrExtension = refcstrExtension;
						length = compareCStrExtension.size();
						for (int i = 0; i < length; ++i)
						{
							compareCStrExtension[i] = tolower(compareCStrExtension[i]);
						}

						if (compareCStrExtension.rfind("_civ4") != std::string::npos)
						{
							compareCStrExtension = compareCStrExtension.substr(compareCStrExtension.rfind("_civ4") + 1);
						}

						if (!strcmp(strExtension.c_str(), compareCStrExtension.c_str()))
						{
							aszFiles.push_back(strModPath.c_str());
						}
					}
				}
			} while (::FindNextFile(hFile, &FileInformation) == TRUE);

			// Close handle
			::FindClose(hFile);

			//FAssertMsg(::GetLastError() != ERROR_NO_MORE_FILES, "something wrong");
			//DWORD dwError = ::GetLastError();
			//if(dwError != ERROR_NO_MORE_FILES)
			//{
			//	FErrorMsg("something wrong");
			//	return;
			//}
		}
		return;
	}

	bool isValidModule(
		bool& bValid,
		const CvString& refcstrRootDirectory,
		const CvString& refcstrModularDirectory,
		const CvString& refcstrExtension,
		bool							bSearchSubdirectories)
	{
		PROFILE_EXTRA_FUNC();
		CvString		strFilePath;		// Filepath
		CvString		strModPath;			// Modules path
		CvString		strPattern;			// Pattern
		CvString		strExtension;		// Extension
		CvString		compareCStrExtension; //Compare with refcstrExtension
		HANDLE          hFile;				// Handle to file
		WIN32_FIND_DATA FileInformation;	// File information

		strPattern = refcstrRootDirectory + "\\*.*";

		//Afforess Addition : Check that we want to load this module first
		for (int iInfosSet = 0; iInfosSet < GC.getNumModLoadControlInfos(); iInfosSet++)
		{
			CvString szType = GC.getModLoadControlInfos(iInfosSet).getParentFolder();
			if (CvString::format("%s\\", refcstrModularDirectory.c_str()) == szType)
			{
				bool bLoadAny = false;
				for (int iSet = 0; iSet < GC.getModLoadControlInfos(iInfosSet).getNumModules(); iSet++)
				{
					if (GC.getModLoadControlInfos(iInfosSet).isLoad(iSet))
					{
						bLoadAny = true;
						break;
					}
				}
				if (!bLoadAny)
				{
					return false;
				}
			}
		}

		hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FileInformation.cFileName[0] != '.')
				{
					strFilePath.erase();
					strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;
					strModPath = refcstrModularDirectory + "\\" + FileInformation.cFileName;

					if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (bSearchSubdirectories)
						{
							// Search subdirectory
							bValid = isValidModule(bValid, strFilePath, strModPath, refcstrExtension);
							if (bValid) break;
						}
					}
					else
					{
						// Check extension
						strExtension = FileInformation.cFileName;
						//force lowercase for comparison
						int length = strExtension.size();
						for (int i = 0; i < length; ++i)
						{
							strExtension[i] = tolower(strExtension[i]);
						}
						//is xml file?
						if (strExtension.rfind(".xml") != std::string::npos)
						{
							bValid = true;
							break;
						}
					}
				}
			} while (::FindNextFile(hFile, &FileInformation) == TRUE);

			// Close handle
			::FindClose(hFile);

			DWORD dwError = ::GetLastError();
			if (!bValid && dwError != ERROR_NO_MORE_FILES)
			{
				FErrorMsg("something wrong");
				return false;
			}

			if (bValid)
			{
				return true;
			}
		}
		return false;
	}

	void loadModControlArray(std::vector<CvString>& aszFiles, const char* szFileRoot)
	{
		PROFILE_EXTRA_FUNC();
		const CvString szModDirectory = GC.getInitCore().getDLLPath() + "\\";

		for (int iI = 0; iI < GC.getModLoadControlVectorSize(); iI++)
		{
			MLFEnumerateFiles(aszFiles, (szModDirectory + GC.getModLoadControlVector(iI).c_str()).c_str(), GC.getModLoadControlVector(iI).c_str(), CvString::format("%s.xml", szFileRoot).c_str());
		}
	}
}
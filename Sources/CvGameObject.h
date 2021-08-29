#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvGameObject.h
//
//  PURPOSE: Wrapper classes for Civ4 game objects, to avoid memory leaks stored in the objects themselves
//
//------------------------------------------------------------------------------------------------
#ifndef CV_GAME_OBJECT_H
#define CV_GAME_OBJECT_H

#include "CvProperties.h"
#include "CvPropertyManipulators.h"
#include "BoolExpr.h"
#include "CvPython.h"

class CvGameObjectGame;
class CvGameObjectTeam;
class CvGameObjectPlayer;
class CvGameObjectCity;
class CvGameObjectUnit;
class CvGameObjectPlot;
class CvGame;
class CvTeam;
class CvPlayer;
class CvCity;
class CvUnit;
class CvPlot;

class CvGameObject
{
public:
	virtual GameObjectTypes getGameObjectType() const = 0;
	virtual CvProperties* getProperties() const = 0;
	virtual const CvProperties* getPropertiesConst() const = 0;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const = 0;
	virtual void foreachOn(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;
	virtual void foreachNear(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func, int iDistance) const;
	virtual void foreachRelated(GameObjectTypes eType, RelationTypes eRelation, bst::function<void (const CvGameObject*)> func, int iData = 0) const;
	virtual void foreachRelatedCond(GameObjectTypes eType, RelationTypes eRelation, bst::function<void (const CvGameObject*)> func, const BoolExpr* pExpr = NULL, int iData = 0) const;
	virtual void enumerate(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType) const;
	virtual void enumerateOn(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType) const;
	virtual void enumerateNear(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType, int iDistance) const;
	virtual void enumerateRelated(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, int iData = 0) const;
	virtual void enumerateRelatedCond(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, const BoolExpr* pExpr = NULL, int iData = 0) const;

	typedef bst::function<void (const CvPropertyManipulators*)> ManipCallbackFn;
	virtual void foreachManipulator(ManipCallbackFn func) const = 0;

	virtual void eventPropertyChanged(PropertyTypes eProperty, int iNewValue);

	virtual bool isTag(TagTypes eTag) const;
	virtual int getAttribute(AttributeTypes eAttribute) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const = 0;

	virtual CvGameObjectPlayer* getOwner() const = 0;
	virtual CvGameObjectPlot* getPlot() const = 0;
	virtual CvGameObjectTeam* getTeam() const = 0;

	virtual void* createPythonWrapper(PyObject*& pyObj) = 0;
	virtual void disposePythonWrapper(void* pArgument) = 0;

	virtual int adaptValueToGame(int iID, int iValue) const;
};

// Python wrapper specialization
namespace Cy
{
	template <>
	struct PyWrap<CvGameObject*> : PyWrapBase
	{
		typedef CvGameObject* value_type;

		PyWrap(const value_type& obj) : obj(obj)
		{
			wrapper = obj->createPythonWrapper(pyobj);
		}

		virtual ~PyWrap()
		{
			obj->disposePythonWrapper(wrapper);
		}

		void* wrapper;
		value_type obj;
	};
}

DECLARE_PY_WRAPPED(CvGameObject*);

class CvGameObjectGame : public CvGameObject
{
public:
	CvGameObjectGame();
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;

	// No specific manipulators apply to game, only global ones
	virtual void foreachManipulator(ManipCallbackFn func) const {}

	virtual int getAttribute(AttributeTypes eAttribute) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;

	virtual CvGameObjectPlayer* getOwner() const;
	virtual CvGameObjectPlot* getPlot() const;
	virtual CvGameObjectTeam* getTeam() const;

	virtual void* createPythonWrapper(PyObject*& pyObj);
	virtual void disposePythonWrapper(void* pArgument);
};

class CvGameObjectTeam : public CvGameObject
{
public:
	explicit CvGameObjectTeam(CvTeam* pTeam);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;

	// No specific manipulators apply to teams, only global ones
	virtual void foreachManipulator(ManipCallbackFn func) const {}

	virtual bool hasGOM(GOMTypes eType, int iID) const;

	virtual CvGameObjectPlayer* getOwner() const;
	virtual CvGameObjectPlot* getPlot() const;
	virtual CvGameObjectTeam* getTeam() const;

	virtual void* createPythonWrapper(PyObject*& pyObj);
	virtual void disposePythonWrapper(void* pArgument);

protected:
	CvTeam* m_pTeam;
};

class CvGameObjectPlayer : public CvGameObject
{
public:
	explicit CvGameObjectPlayer(CvPlayer* pPlayer);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;

	virtual void foreachManipulator(ManipCallbackFn func) const;

	virtual bool hasGOM(GOMTypes eType, int iID) const;
	virtual bool isTag(TagTypes eTag) const;

	virtual CvGameObjectPlayer* getOwner() const;
	virtual CvGameObjectPlot* getPlot() const;
	virtual CvGameObjectTeam* getTeam() const;

	virtual void* createPythonWrapper(PyObject*& pyObj);
	virtual void disposePythonWrapper(void* pArgument);

	virtual int adaptValueToGame(int iID, int iValue) const;

protected:
	CvPlayer* m_pPlayer;
};

class CvGameObjectCity : public CvGameObject
{
public:
	explicit CvGameObjectCity(CvCity* pCity);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;
	virtual void foreachRelated(GameObjectTypes eType, RelationTypes eRelation, bst::function<void (const CvGameObject*)> func, int iData = 0) const;

	virtual void foreachManipulator(ManipCallbackFn func) const;

	virtual int getAttribute(AttributeTypes eAttribute) const;
	virtual bool isTag(TagTypes eTag) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;

	virtual CvGameObjectPlayer* getOwner() const;
	virtual CvGameObjectPlot* getPlot() const;
	virtual CvGameObjectTeam* getTeam() const;

	virtual void* createPythonWrapper(PyObject*& pyObj);
	virtual void disposePythonWrapper(void* pArgument);

	virtual int adaptValueToGame(int iID, int iValue) const;

protected:
	CvCity* m_pCity;
};

class CvGameObjectUnit : public CvGameObject
{
public:
	explicit CvGameObjectUnit(CvUnit* pUnit);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;

	virtual void foreachManipulator(ManipCallbackFn func) const;

	virtual void eventPropertyChanged(PropertyTypes eProperty, int iNewValue);

	virtual bool isTag(TagTypes eTag) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;

	virtual CvGameObjectPlayer* getOwner() const;
	virtual CvGameObjectPlot* getPlot() const;
	virtual CvGameObjectTeam* getTeam() const;

	virtual void* createPythonWrapper(PyObject*& pyObj);
	virtual void disposePythonWrapper(void* pArgument);

	virtual int adaptValueToGame(int iID, int iValue) const;

protected:
	CvUnit* m_pUnit;
};

class CvGameObjectPlot : public CvGameObject
{
public:
	explicit CvGameObjectPlot(CvPlot* pPlot);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;
	virtual void foreachOn(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func) const;
	virtual void foreachNear(GameObjectTypes eType, bst::function<void (const CvGameObject*)> func, int iDistance) const;
	virtual void foreachRelated(GameObjectTypes eType, RelationTypes eRelation, bst::function<void (const CvGameObject*)> func, int iData = 0) const;

	virtual void foreachManipulator(ManipCallbackFn func) const;

	virtual bool isTag(TagTypes eTag) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;

	virtual CvGameObjectPlayer* getOwner() const;
	virtual CvGameObjectPlot* getPlot() const;
	virtual CvGameObjectTeam* getTeam() const;

	virtual void* createPythonWrapper(PyObject*& pyObj);
	virtual void disposePythonWrapper(void* pArgument);

protected:
	CvPlot* m_pPlot;
};

#endif

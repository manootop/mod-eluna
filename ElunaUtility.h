/*
* Copyright (C) 2010 - 2015 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef _ELUNA_UTIL_H
#define _ELUNA_UTIL_H

#include <unordered_map>
#include <unordered_set>
#include "Common.h"
#include "SharedDefines.h"
#include "ObjectGuid.h"
#ifdef TRINITY
#include "QueryResult.h"
#ifdef CATA
#include "Object.h"
#endif
#else
#include "Database/QueryResult.h"
#endif

// Some dummy includes containing BOOST_VERSION:
// ObjectAccessor.h Config.h Log.h
#ifdef BOOST_VERSION
#define USING_BOOST
#endif

#ifdef USING_BOOST
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#endif

#ifdef TRINITY
typedef QueryResult ElunaQuery;
#define ELUNA_LOG_INFO(...)     TC_LOG_INFO("eluna", __VA_ARGS__);
#define ELUNA_LOG_ERROR(...)    TC_LOG_ERROR("eluna", __VA_ARGS__);
#define ELUNA_LOG_DEBUG(...)    TC_LOG_DEBUG("eluna", __VA_ARGS__);
#define GET_GUID                GetGUID
#else
typedef QueryNamedResult ElunaQuery;
#define ASSERT                  MANGOS_ASSERT
#define ELUNA_LOG_INFO(...)     sLog.outString(__VA_ARGS__);
#define ELUNA_LOG_ERROR(...)    sLog.outErrorEluna(__VA_ARGS__);
#define ELUNA_LOG_DEBUG(...)    sLog.outDebug(__VA_ARGS__);
#define GET_GUID                GetObjectGuid
#define GetGameObjectTemplate   GetGameObjectInfo
#define GetItemTemplate         GetItemPrototype
#define GetTemplate             GetProto
#endif

#ifndef HIGHGUID_PLAYER
#define HIGHGUID_PLAYER         HighGuid::Player
#endif
#ifndef HIGHGUID_UNIT
#define HIGHGUID_UNIT           HighGuid::Unit
#endif
#ifndef HIGHGUID_GAMEOBJECT
#define HIGHGUID_GAMEOBJECT     HighGuid::GameObject
#endif
#ifndef HIGHGUID_TRANSPORT
#define HIGHGUID_TRANSPORT      HighGuid::Transport
#endif
#ifndef HIGHGUID_MO_TRANSPORT
#define HIGHGUID_MO_TRANSPORT   HighGuid::Mo_Transport
#endif
#ifndef HIGHGUID_VEHICLE
#define HIGHGUID_VEHICLE        HighGuid::Vehicle
#endif
#ifndef HIGHGUID_PET
#define HIGHGUID_PET            HighGuid::Pet
#endif
#ifndef HIGHGUID_DYNAMICOBJECT
#define HIGHGUID_DYNAMICOBJECT  HighGuid::DynamicObject
#endif
#ifndef HIGHGUID_CORPSE
#define HIGHGUID_CORPSE         HighGuid::Corpse
#endif
#ifndef HIGHGUID_ITEM
#define HIGHGUID_ITEM           HighGuid::Item
#endif
#ifndef HIGHGUID_INSTANCE
#define HIGHGUID_INSTANCE       HighGuid::Instance
#endif
#ifndef HIGHGUID_GROUP
#define HIGHGUID_GROUP          HighGuid::Group
#endif
#ifndef HIGHGUID_CONTAINER
#define HIGHGUID_CONTAINER      HighGuid::Container
#endif
#ifndef MAKE_NEW_GUID
#define MAKE_NEW_GUID(l, e, h)  ObjectGuid(h, e, l)
#endif
#ifndef GUID_ENPART
#define GUID_ENPART(guid)       ObjectGuid(guid).GetEntry()
#endif
#ifndef GUID_LOPART
#define GUID_LOPART(guid)       ObjectGuid(guid).GetCounter()
#endif
#ifndef GUID_HIPART
#define GUID_HIPART(guid)       ObjectGuid(guid).GetHigh()
#endif

class Unit;
class WorldObject;
struct FactionTemplateEntry;

namespace ElunaUtil
{
    uint32 GetCurrTime();

    uint32 GetTimeDiff(uint32 oldMSTime);

    class ObjectGUIDCheck
    {
    public:
        ObjectGUIDCheck(ObjectGuid guid);
        bool operator()(WorldObject* object);

        ObjectGuid _guid;
    };

    // Binary predicate to sort WorldObjects based on the distance to a reference WorldObject
    class ObjectDistanceOrderPred
    {
    public:
        ObjectDistanceOrderPred(WorldObject const* pRefObj, bool ascending = true);
        bool operator()(WorldObject const* pLeft, WorldObject const* pRight) const;

        WorldObject const* m_refObj;
        const bool m_ascending;
    };

    // Doesn't get self
    class WorldObjectInRangeCheck
    {
    public:
        WorldObjectInRangeCheck(bool nearest, WorldObject const* obj, float range,
            uint16 typeMask = 0, uint32 entry = 0, uint32 hostile = 0, uint32 dead = 0);
        WorldObject const& GetFocusObject() const;
        bool operator()(WorldObject* u);

        WorldObject const* const i_obj;
        Unit const* i_obj_unit;
        FactionTemplateEntry const* i_obj_fact;
        uint32 const i_hostile; // 0 both, 1 hostile, 2 friendly
        uint32 const i_entry;
        float i_range;
        uint16 const i_typeMask;
        uint32 const i_dead; // 0 both, 1 alive, 2 dead
        bool const i_nearest;
    };

    /*
     * Usage:
     * Inherit this class, then when needing lock, use
     * ReadGuard guard(GetLock());
     * or
     * WriteGuard guard(GetLock());
     *
     * The lock is automatically released at end of scope
     */
    class RWLockable
    {
    public:

#ifdef USING_BOOST
        typedef boost::shared_mutex LockType;
        typedef boost::shared_lock<LockType> ReadGuard;
        typedef boost::unique_lock<LockType> WriteGuard;
#else
        typedef ACE_RW_Thread_Mutex LockType;
        typedef ACE_Read_Guard<LockType> ReadGuard;
        typedef ACE_Write_Guard<LockType> WriteGuard;
#endif

        LockType& GetLock() { return _lock; }

    private:
        LockType _lock;
    };

    /*
     * Encodes `data` in Base-64 and store the result in `output`.
     */
    void EncodeData(const unsigned char* data, size_t input_length, std::string& output);

    /*
     * Decodes `data` from Base-64 and returns a pointer to the result, or `NULL` on error.
     *
     * The returned result buffer must be `delete[]`ed by the caller.
     */
    unsigned char* DecodeData(const char* data, size_t *output_length);
};

#endif

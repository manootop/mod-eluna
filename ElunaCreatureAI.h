/*
 * Copyright (C) 2010 - 2015 Eluna Lua Engine <http://emudevs.com/>
 * This program is free software licensed under GPL version 3
 * Please see the included DOCS/LICENSE.md for more information
 */

#ifndef _ELUNA_CREATURE_AI_H
#define _ELUNA_CREATURE_AI_H

#include "LuaEngine.h"

#ifndef TRINITY
class AggressorAI;
typedef AggressorAI ScriptedAI;
#else
struct ScriptedAI;
#endif

struct ElunaCreatureAI : ScriptedAI
{
    // used to delay the spawn hook triggering on AI creation
    bool justSpawned;
    // used to delay movementinform hook (WP hook)
    std::vector< std::pair<uint32, uint32> > movepoints;
#ifndef TRINITY
#define me  m_creature
#endif

    ElunaCreatureAI(Creature* creature) : ScriptedAI(creature), justSpawned(true)
    {
        me->GetMap()->GetEluna()->GetTableMgr()->CreateTable(me->GetGUID());
    }
    ~ElunaCreatureAI()
    {
        me->GetMap()->GetEluna()->GetTableMgr()->DeleteTableRef(me->GetGUID());
    }

    //Called at World update tick
#ifndef TRINITY
    void UpdateAI(const uint32 diff) override
#else
    void UpdateAI(uint32 diff) override
#endif
    {
        if (justSpawned)
        {
            justSpawned = false;
            JustRespawned();
        }

        Map* map = me->FindMap();
        if (!map)
            return;

        if (!movepoints.empty())
        {
            for (auto& point : movepoints)
            {
                if (!map->GetEluna()->MovementInform(me, point.first, point.second))
                    ScriptedAI::MovementInform(point.first, point.second);
            }
            movepoints.clear();
        }

        if (!map->GetEluna()->UpdateAI(me, diff))
        {
#ifdef TRINITY
            if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
                ScriptedAI::UpdateAI(diff);
#else
            if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE))
                ScriptedAI::UpdateAI(diff);
#endif
        }
    }

    //Called for reaction at enter to combat if not in combat yet (enemy can be nullptr)
    //Called at creature aggro either by MoveInLOS or Attack Start
    void EnterCombat(Unit* target) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->EnterCombat(me, target))
                ScriptedAI::EnterCombat(target);
    }

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->DamageTaken(me, attacker, damage))
                ScriptedAI::DamageTaken(attacker, damage);
    }

    //Called at creature death
    void JustDied(Unit* killer) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->JustDied(me, killer))
                ScriptedAI::JustDied(killer);
    }

    //Called at creature killing another unit
    void KilledUnit(Unit* victim) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->KilledUnit(me, victim))
                ScriptedAI::KilledUnit(victim);
    }

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature* summon) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->JustSummoned(me, summon))
                ScriptedAI::JustSummoned(summon);
    }

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature* summon) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->SummonedCreatureDespawn(me, summon))
                ScriptedAI::SummonedCreatureDespawn(summon);
    }

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32 type, uint32 id) override
    {
        // delayed since hook triggers before actually reaching the point
        // and starting new movement would bug
        movepoints.push_back(std::make_pair(type, id));
    }

    // Called before EnterCombat even before the creature is in combat.
    void AttackStart(Unit* target) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->AttackStart(me, target))
                ScriptedAI::AttackStart(target);
    }

    // Called for reaction at stopping attack at no attackers or targets
    void EnterEvadeMode() override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->EnterEvadeMode(me))
                ScriptedAI::EnterEvadeMode();
    }

    // Called when the creature is target of hostile action: swing, hostile spell landed, fear/etc)
    void AttackedBy(Unit* attacker) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->AttackedBy(me, attacker))
                ScriptedAI::AttackedBy(attacker);
    }

    // Called when creature is spawned or respawned (for reseting variables)
    void JustRespawned() override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->JustRespawned(me))
                ScriptedAI::JustRespawned();
    }

    // Called at reaching home after evade
    void JustReachedHome() override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->JustReachedHome(me))
                ScriptedAI::JustReachedHome();
    }

    // Called at text emote receive from player
    void ReceiveEmote(Player* player, uint32 emoteId) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->ReceiveEmote(me, player, emoteId))
                ScriptedAI::ReceiveEmote(player, emoteId);
    }

    // called when the corpse of this creature gets removed
    void CorpseRemoved(uint32& respawnDelay) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->CorpseRemoved(me, respawnDelay))
                ScriptedAI::CorpseRemoved(respawnDelay);
    }

#ifndef TRINITY
    // Enables use of MoveInLineOfSight
    bool IsVisible(Unit* who) const override
    {
        return true;
    }
#endif

    void MoveInLineOfSight(Unit* who) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->MoveInLineOfSight(me, who))
                ScriptedAI::MoveInLineOfSight(who);
    }

    // Called when hit by a spell
    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->SpellHit(me, caster, spell))
                ScriptedAI::SpellHit(caster, spell);
    }

    // Called when spell hits a target
    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->SpellHitTarget(me, target, spell))
                ScriptedAI::SpellHitTarget(target, spell);
    }

#ifdef TRINITY

    // Called when the creature is summoned successfully by other creature
    void IsSummonedBy(Unit* summoner) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->OnSummoned(me, summoner))
                ScriptedAI::IsSummonedBy(summoner);
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->SummonedCreatureDies(me, summon, killer))
                ScriptedAI::SummonedCreatureDies(summon, killer);
    }

    // Called when owner takes damage
    void OwnerAttackedBy(Unit* attacker) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->OwnerAttackedBy(me, attacker))
                ScriptedAI::OwnerAttackedBy(attacker);
    }

    // Called when owner attacks something
    void OwnerAttacked(Unit* target) override
    {
        if (Map* map = me->FindMap())
            if (!map->GetEluna()->OwnerAttacked(me, target))
                ScriptedAI::OwnerAttacked(target);
    }
#endif

#ifndef TRINITY
#undef me
#endif
};

#endif

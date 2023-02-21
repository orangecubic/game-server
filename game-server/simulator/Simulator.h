#pragma once

#include "box2d/box2d.h"
#include "Vec2.h"
#include "HpItem.h"
#include "Player.h"
#include "Rockfall.h"
#include "GunShot.h"
#include "IMap.h"
#include "DefaultMap.h"
#include "SimulatorSetting.h"
#include <map>
#include <unordered_map>
#include <functional>
#include <set>

struct CollisionEventSetting {
    int mapCode;
    int playerCode;
    int fieldItemCode;
    int gunShotCode;
    int rockfallCode;
};

class Simulator : public b2ContactListener {
public:

    class EventCallback {
    public:
        virtual void onPlayerContactMap(Player*, IMap*) = 0;
        virtual void onPlayerHitGunshot(Player*, GunShot*) = 0;
        virtual void onRockfallHitGunshot(Rockfall*, GunShot*) = 0;
        virtual void onHitRockfall(Player*, Rockfall*) = 0;
        virtual void onPickupHp(Player*, HpItem*) = 0;
    };
    Simulator(int id, EventCallback* callback, float worldGravity, const std::set<int>& enabledContact = {
        EntityType::Map | EntityType::Player,
        EntityType::Map | EntityType::Rockfall,
        EntityType::Map | EntityType::PlayerGunShot,
        EntityType::Player | EntityType::FieldItem,
        EntityType::Player | EntityType::Rockfall,
        EntityType::Player | EntityType::PlayerGunShot,
        EntityType::Rockfall | EntityType::PlayerGunShot,
    });

    Player* spawnPlayer(const simVec2& position, int initialHp, int movementForce, int entityId = -1);
    GunShot* spawnPlayerGunShot(Player* player, int gunshotDamage, int entityId = -1);
    IFieldItem* spawnHpItem(const simVec2& position, int hpValue, int entityId = -1);
    Rockfall* spawnRockfall(const simVec2& position, int rockfallDamage, int entityId = -1);

    int getSimulatorId();
    IEntity* getEntity(int id);
    
    void removeEntity(int id);

    void simulate(float delta);

    const std::unordered_map<int, IEntity*>& getEntityMap();

    IMap* getMap();

    b2World* getWorld();
    
    void triggerCollisionEvent(IEntity*, IEntity*);
    
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact) {}
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {}
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}

    virtual ~Simulator();
private:
    const int mId;

    EventCallback* mEventCallback;
    b2World* mWorld;
    IMap* mMap;

    CollisionEventSetting mCollisionEventCode;
    
    std::set<int> mEnabledContact;
    int mEntitySequence = 0;
    std::unordered_map<int, IEntity*> mEntityMap;
    std::unordered_map<int, std::function<void(IEntity*, IEntity*)>> mContactListenerMap;
    std::vector<std::function<void()>> mContactEventQueue;
    std::vector<int> mDestroyQueue;
    
    void onPlayerContactMap(IEntity*, IEntity*);
    void onRockfallContactMap(IEntity*, IEntity*);
    void onPlayerContactGunshot(IEntity*, IEntity*);
    void onPlayerContactItem(IEntity*, IEntity*);
    void onPlayerContactRockfall(IEntity*, IEntity*);
    void onRockfallContactGunshot(IEntity*, IEntity*);
    void onGunshotContactMap(IEntity*, IEntity*);
};


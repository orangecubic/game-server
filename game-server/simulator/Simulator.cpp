#include "Simulator.h"
#include <algorithm>

Simulator::Simulator(int id, EventCallback* callback, float worldGravity, const std::set<int>& enabledContact) : mId(id), mEventCallback(callback), mEnabledContact(enabledContact) {
    
    mWorld = new b2World(b2Vec2(0, worldGravity));
    mWorld->SetContactListener(this);

    mContactListenerMap[EntityType::Map | EntityType::Player] = std::bind(&Simulator::onPlayerContactMap, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::Map | EntityType::Rockfall] = std::bind(&Simulator::onRockfallContactMap, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::Map | EntityType::PlayerGunShot] = std::bind(&Simulator::onGunshotContactMap, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::Player | EntityType::PlayerGunShot] = std::bind(&Simulator::onPlayerContactGunshot, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::Player | EntityType::FieldItem] = std::bind(&Simulator::onPlayerContactItem, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::Player | EntityType::Rockfall] = std::bind(&Simulator::onPlayerContactRockfall, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::PlayerGunShot | EntityType::Rockfall] = std::bind(&Simulator::onRockfallContactGunshot, this, std::placeholders::_1, std::placeholders::_2);
    mContactListenerMap[EntityType::PlayerGunShot] = [](IEntity*, IEntity*) {};
    mContactListenerMap[EntityType::Player] = [](IEntity*, IEntity*) {};
    
    mMap = new DefaultMap(this, mEntitySequence++, mWorld, EntityType::Player | EntityType::PlayerGunShot | EntityType::Rockfall);
    mEntityMap[mMap->getEntityId()] = mMap;
    if (enabledContact.empty()) {
        return;
    }
    
    std::vector<int> entityTypes = {EntityType::Map, EntityType::Player, EntityType::Rockfall, EntityType::PlayerGunShot, EntityType::FieldItem};
    std::map<int, int> eventCodeMap;
    for (auto code : enabledContact) {
        for (auto type : entityTypes) {
            if (code & type) {
                eventCodeMap[type] = eventCodeMap[type] | (code == type ? code : code - type);
            }
        }
    }
    
    auto disabledEventMap = mContactListenerMap;
    for (auto eventCode : mEnabledContact) {
        disabledEventMap.erase(eventCode);
    }
    
    mCollisionEventCode.playerCode = eventCodeMap[EntityType::Player];
    mCollisionEventCode.fieldItemCode = eventCodeMap[EntityType::FieldItem];
    mCollisionEventCode.gunShotCode = eventCodeMap[EntityType::PlayerGunShot];
    mCollisionEventCode.mapCode = eventCodeMap[EntityType::Map];
    mCollisionEventCode.rockfallCode = eventCodeMap[EntityType::Rockfall];
}

Player* Simulator::spawnPlayer(const simVec2& position, int initialHp, int movementForce, int entityId) {
    
    auto player = new Player(this, entityId == -1 ? mEntitySequence : entityId, mWorld, initialHp, movementForce, simVec2(0.5, 0.5), position, mCollisionEventCode.playerCode);
    mEntityMap[player->getEntityId()] = player;
    
    mEntitySequence++;
    return player;
}

GunShot* Simulator::spawnPlayerGunShot(Player* player, int gunshotDamage, int entityId) {
    auto gunShot = new GunShot(this, entityId == -1 ? mEntitySequence : entityId, mWorld, simVec2(0.5, 0.5), player, gunshotDamage, mCollisionEventCode.gunShotCode);
    mEntityMap[gunShot->getEntityId()] = gunShot;
    
    mEntitySequence++;
    return gunShot;
}

IFieldItem* Simulator::spawnHpItem(const simVec2& position, int hpValue, int entityId) {
    auto hp = new HpItem(this, entityId == -1 ? mEntitySequence : entityId, mWorld, position, simVec2(0.5, 0.5), hpValue, mCollisionEventCode.fieldItemCode);
    mEntityMap[hp->getEntityId()] = hp;
    
    mEntitySequence++;
    return hp;
}

Rockfall* Simulator::spawnRockfall(const simVec2& position, int rockfallDamage, int entityId) {
    auto rockfall = new Rockfall(this, entityId == -1 ? mEntitySequence : entityId, mWorld, simVec2(0.5, 0.7), position, rockfallDamage, mCollisionEventCode.rockfallCode);
    mEntityMap[rockfall->getEntityId()] = rockfall;
    
    mEntitySequence++;
    return rockfall;
}

IEntity* Simulator::getEntity(int id) {
    auto entityIter = mEntityMap.find(id);
    if (entityIter == mEntityMap.end()) {
        return nullptr;
    }
    return entityIter->second;
}

void Simulator::removeEntity(int id) {
    mDestroyQueue.push_back(id);
}

void Simulator::simulate(float delta) {
    for (auto& pair : mEntityMap) {
        pair.second->simulate(delta);
    }

    mWorld->Step(delta, 6, 2);

    for (auto& handler : mContactEventQueue) {
        handler();
    }
    mContactEventQueue.clear();
    
    for (auto& entityId : mDestroyQueue) {
        
        auto entityIter = mEntityMap.find(entityId);
        
        if (entityIter == mEntityMap.end()) {
            continue;
        }
        delete entityIter->second;
        mEntityMap.erase(entityIter);
    }
    mDestroyQueue.clear();
}

int Simulator::getSimulatorId() {
    return mId;
}

void Simulator::triggerCollisionEvent(IEntity* entityA, IEntity* entityB) {
    if (entityA->getEntityType() > entityB->getEntityType()) {
        std::swap(entityA, entityB);
    }
    
    auto& handler = mContactListenerMap[entityA->getEntityType() | entityB->getEntityType()];

    assert(handler);
    IEntity& entityARef = *entityA;
    IEntity& entityBRef = *entityB;

    // capture by reference to avoid heap allocation
    auto f = [&]() {
        handler(&entityARef, &entityBRef);
    };

    mContactEventQueue.push_back(f);
}

void Simulator::BeginContact(b2Contact* contact) {
    auto entityA = reinterpret_cast<IEntity*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    auto entityB = reinterpret_cast<IEntity*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if (entityA->getEntityType() > entityB->getEntityType()) {
        std::swap(entityA, entityB);
    }

    auto& handler = mContactListenerMap[entityA->getEntityType() | entityB->getEntityType()];

    assert(handler && mEnabledContact.find(entityA->getEntityType() | entityB->getEntityType()) != mEnabledContact.end());

    IEntity& entityARef = *entityA;
    IEntity& entityBRef = *entityB;

    // capture by reference to avoid heap allocation
    auto f = [&]() {
        handler(&entityARef, &entityBRef);
    };
    mContactEventQueue.push_back(f);
}

void Simulator::onPlayerContactMap(IEntity* mapE, IEntity* playerE) {
    auto map = reinterpret_cast<IMap*>(mapE);
    auto player = reinterpret_cast<Player*>(playerE);

    mEventCallback->onPlayerContactMap(player, map);
}

void Simulator::onRockfallContactMap(IEntity* mapE, IEntity* rockfallE) {
    auto map = reinterpret_cast<IMap*>(mapE);
    auto rockfall = reinterpret_cast<Rockfall*>(rockfallE);

    rockfall->destroyEntity();
}

void Simulator::onPlayerContactGunshot(IEntity* playerE, IEntity* gunshotE) {
    auto player = reinterpret_cast<Player*>(playerE);
    auto gunshot = reinterpret_cast<GunShot*>(gunshotE);

    // player->getBody()->SetAngularVelocity(20);
    player->getBody()->SetLinearVelocity(b2Vec2(gunshot->getLinearVelocity().x*1.5, 10));
    player->decreaseHp(gunshot->getDamage());
    gunshot->destroyEntity();
    
    mEventCallback->onPlayerHitGunshot(player, gunshot);
}

void Simulator::onPlayerContactItem(IEntity* playerE, IEntity* itemE) {
    
    auto player = reinterpret_cast<Player*>(playerE);
    auto item = reinterpret_cast<IFieldItem*>(itemE);

    item->getItem(player);

    if (item->itemCode() == ItemCode_Hp) {
        mEventCallback->onPickupHp(player, reinterpret_cast<HpItem*>(item));
    }

    item->destroyEntity();
}

void Simulator::onPlayerContactRockfall(IEntity* playerE, IEntity* rockfallE) {
    auto player = reinterpret_cast<Player*>(playerE);
    auto rockfall = reinterpret_cast<Rockfall*>(rockfallE);

    float xVelocity = player->getCurrentDirection() == PlayerMovementStatus_Left ? 5 : -5;

    player->setLinearVelocity(simVec2(xVelocity, 8));
    player->decreaseHp(rockfall->getDamage());
    
    mEventCallback->onHitRockfall(player, rockfall);

    rockfall->destroyEntity();
}

void Simulator::onRockfallContactGunshot(IEntity* rockfallE, IEntity* gunshotE) {
    auto rockfall = reinterpret_cast<Rockfall*>(rockfallE);
    auto gunshot = reinterpret_cast<GunShot*>(gunshotE);
    
    gunshot->destroyEntity();

    auto gunshotVelocity = gunshot->getLinearVelocity();
    rockfall->setLinearVelocity(simVec2(gunshotVelocity.x, 0));
    rockfall->setAngularVelocity(gunshotVelocity.x);

    mEventCallback->onRockfallHitGunshot((Rockfall*)rockfallE, (GunShot*)gunshotE);
}

void Simulator::onGunshotContactMap(IEntity* mapE, IEntity* gunshotE) {
    auto gunshot = reinterpret_cast<GunShot*>(gunshotE);
    auto map = reinterpret_cast<IMap*>(mapE);
    
    gunshot->destroyEntity();
    // mEventCallback->onRockfallHitGunshot((Rockfall*)rockfallE, (GunShot*)gunshotE);
}

const std::map<int, IEntity*>& Simulator::getEntityMap() {
    return mEntityMap;
}

IMap* Simulator::getMap() {
    return mMap;
}

b2World* Simulator::getWorld() {
    return mWorld;
}

Simulator::~Simulator() {
    for (auto& pair : mEntityMap) {
        delete pair.second;
    }

    delete mWorld;
}

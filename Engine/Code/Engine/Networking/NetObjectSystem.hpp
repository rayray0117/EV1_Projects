#pragma once
#include "Engine/Networking/Net.hpp"
#include <map>
#include "Engine/Networking/Interval.hpp"
//////////////////////////////////////////////////////////////////////////
class NetMessage;
class NetSession;
class NetConnection;
//////////////////////////////////////////////////////////////////////////
enum ENOSMessage
{
	NETOBJECT_CREATE_OBJECT = NUM_CORE_MESSAGES,
	NETOBJECT_DESTROY_OBJECT,
	NETOBJECT_SEND_UPDATE,
	NUM_NETOBJECT_MGS
};

enum ENetAuthoriy
{
	SEVER_ONLY,
	CLIENT_ONLY,
	SEVER_CLINET,
	NUM_NET_AUTH
};
//////////////////////////////////////////////////////////////////////////
namespace NetObjectSystem
{

struct NetObjectTypeDefinition;


void StartUp(NetSession* gameSession, float hz);


class NetObject
{
public:
	NetObject(NetObjectTypeDefinition *defn)
		: definition(defn)
		, net_id(INVALID_NETWORK_ID)
		, local_object(nullptr)
		, last_received_snapshot(nullptr)
		, current_snapshot(nullptr)
	{}

	~NetObject(){}

public:
	uint16 net_id;
	NetObjectTypeDefinition *definition;

	void* local_object;
	void* last_received_snapshot;
	void* current_snapshot;
};



typedef void(*append_create_info_cb)(NetMessage*, void*);
typedef void* (*process_create_info_cb)(NetMessage*, NetObject* obj);
typedef void(*append_destroy_info_cb)(NetMessage*, void*);
typedef void(*process_destroy_info_cb)(NetMessage*, void*);

typedef void(*append_snapshot_cb)(NetMessage*, void*);
typedef void(*apply_snapshot_cb)(void* local_obj, void* snap);
typedef void(*get_current_snapshot_cb)(void* current_snapshot, void* local_obj);
typedef void(*process_snapshot_cb)(void* last_snapshot, NetMessage*);

struct NetObjectTypeDefinition
{
	append_create_info_cb append_create_info;
	process_create_info_cb process_create_info;
	append_destroy_info_cb append_destroy_info;
	process_destroy_info_cb process_destroy_info;

	append_snapshot_cb append_snapshot;
	apply_snapshot_cb apply_snapshot;
	get_current_snapshot_cb get_current_snapshot;
	process_snapshot_cb process_snapshot;
};

static std::map<uint16, NetObject*> s_netObjects;
static std::map<uint8, NetObjectTypeDefinition*> s_netObjectTypeDefs;
extern NetSession* g_NetObjectSession;
static Interval s_interval;
extern ENetAuthoriy g_auth;

uint16 GetUnusedID();
NetObject* Replicate(void* object_ptr, uint8 type_id);
void StopRelication(uint16 net_id);
void OnReceiveCreate(NetMessage* msg);
void OnReceiveDestroy(NetMessage* msg);
NetObjectTypeDefinition* FindNetObjectDefinition(uint8 type_id);
void AddNetObjectDefinition(uint8 type_id, NetObjectTypeDefinition* def);
NetObject* FindNetObject(uint16 net_id);
void RegisterNetObject(NetObject* nop);
void UnregisterNetObject(NetObject* nop);
uint8 GetDefsId(NetObjectTypeDefinition* def);


void NetObjectSystemStep();
void SendNetObjectUpdates();
void SendNetObjectUpdateTo(NetConnection *cp);
void OnNetObjectUpdateRecieved(NetMessage *update_msg);

void SendCreateInfosForJoinRequest(NetConnection *cp);

}
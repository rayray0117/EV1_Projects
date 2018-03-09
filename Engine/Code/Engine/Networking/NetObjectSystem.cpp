#include "Engine/Networking/NetObjectSystem.hpp"
#include "NetMessage.hpp"
#include "NetSession.hpp"
#include "NetConnection.hpp"
#include "../Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////
uint16 g_lastUsedId = 0;
NetSession* NetObjectSystem::g_NetObjectSession;
//std::map<uint16, NetObjectSystem::NetObject*> NetObjectSystem::s_netObjects;
//std::map<uint8, NetObjectSystem::NetObjectTypeDefinition*> NetObjectSystem::s_netObjectTypeDefs;
ENetAuthoriy NetObjectSystem::g_auth = SEVER_ONLY;
//////////////////////////////////////////////////////////////////////////
void NetObjectSystem::StartUp(NetSession* gameSession, float hz)
{
	g_NetObjectSession = gameSession;
	g_NetObjectSession->register_message_function(NETOBJECT_CREATE_OBJECT, OnReceiveCreate );
	g_NetObjectSession->register_message_function(NETOBJECT_DESTROY_OBJECT, OnReceiveDestroy );
	g_NetObjectSession->register_message_function(NETOBJECT_SEND_UPDATE, OnNetObjectUpdateRecieved);
	s_interval.set_frequency(hz);
}

uint16 NetObjectSystem::GetUnusedID()
{
	for (uint16 i = 1; i <= g_lastUsedId; ++i)
	{
		if (nullptr == FindNetObject(i))
		{
			return i;
		}
	}

	return ++g_lastUsedId;
}


NetObjectSystem::NetObject* NetObjectSystem::Replicate(void* object_ptr, uint8 type_id)
{
	NetObjectTypeDefinition* defn = FindNetObjectDefinition(type_id);
	if (defn == nullptr) 
	{
		return nullptr;
	}

	NetObject *nop = new NetObject(defn);

	nop->local_object = object_ptr;
	nop->net_id = GetUnusedID();

	RegisterNetObject(nop);

	NetMessage create(NETOBJECT_CREATE_OBJECT);
	create.sender = g_NetObjectSession->my_connection;
	create.write(type_id);
	create.write(nop->net_id);

	defn->append_create_info(&create, object_ptr);

	NetSession* sp = g_NetObjectSession;
	sp->SendMessageToOthers(create);

	return nop;
}

void NetObjectSystem::OnReceiveCreate(NetMessage* msg)
{
	uint8 type_id;
	msg->read(type_id);
	uint16 net_id;
	msg->read(net_id);

	NetObjectTypeDefinition *defn = FindNetObjectDefinition(type_id);
	ASSERT_OR_DIE(defn != nullptr,"");

	NetObject* nop = new NetObject(defn);
	nop->net_id = net_id;

	void* local_object = defn->process_create_info(msg, nop);
	ASSERT_OR_DIE(local_object != nullptr, "");

	nop->local_object = local_object;
	RegisterNetObject(nop); // register object with system
}

void NetObjectSystem::StopRelication(uint16 net_id)
{
	// remove from our system
	NetObject* nop = FindNetObject(net_id);
	if (nop == nullptr) 
	{
		return;
	}

	UnregisterNetObject(nop);

	// tell everyone 
	NetMessage msg(NETOBJECT_DESTROY_OBJECT);
	msg.write(nop->net_id);

	// usually does nothing - no-op.
	if(nop->definition->append_destroy_info != nullptr)
		nop->definition->append_destroy_info(&msg, nop->local_object);

	NetSession* sp = g_NetObjectSession;
	sp->SendMessageToOthers(msg);
	SAFE_DELETE(nop);
}


void NetObjectSystem::OnReceiveDestroy(NetMessage *msg)
{
	uint16 net_id;
	msg->read(net_id);
	NetObject* nop = FindNetObject(net_id);

	if (nop == nullptr) 
	{
		return;
	}

	UnregisterNetObject(nop);

	// THIS is critical;
	nop->definition->process_destroy_info(msg, nop->local_object);

	delete nop;
}

NetObjectSystem::NetObjectTypeDefinition* NetObjectSystem::FindNetObjectDefinition(uint8 type_id)
{
	auto found = s_netObjectTypeDefs.find(type_id);
	if (found != s_netObjectTypeDefs.end())
	{
		return found->second;
	}

	return nullptr;
}

void NetObjectSystem::AddNetObjectDefinition(uint8 type_id, NetObjectTypeDefinition* def)
{
	s_netObjectTypeDefs[type_id] = def;
}

void NetObjectSystem::RegisterNetObject(NetObject* nop)
{
	uint16 id = nop->net_id;
	s_netObjects[id] = nop;
}

NetObjectSystem::NetObject* NetObjectSystem::FindNetObject(uint16 net_id)
{
	auto found = s_netObjects.find(net_id);
	if (found != s_netObjects.end())
	{
		return found->second;
	}

	return nullptr;
}

void NetObjectSystem::UnregisterNetObject(NetObject* nop)
{
	auto found = s_netObjects.find(nop->net_id);
	s_netObjects.erase(found);
}

uint8 NetObjectSystem::GetDefsId(NetObjectTypeDefinition* def)
{
	for (auto iterator = s_netObjectTypeDefs.begin(); iterator != s_netObjectTypeDefs.end(); ++iterator)
	{
		if (iterator->second == def)
		{
			return iterator->first;
		}
	}
	uint8 r = 0;
	return r - 1; 
}

void NetObjectSystem::NetObjectSystemStep()
{
	PROFILE_SCOPE_FUNCTION();
	//*/
	if (s_interval.check_and_reset()) 
	{
		if (g_NetObjectSession->isHost() && g_auth == SEVER_ONLY)
		{
			SendNetObjectUpdates();
		}
		else if (!g_NetObjectSession->isHost() && g_auth == CLIENT_ONLY)
		{
			SendNetObjectUpdates();
		}
		else if (g_auth == SEVER_CLINET)
		{
			SendNetObjectUpdates();
		}
	}
	
	if (g_NetObjectSession->isHost() && g_auth == SEVER_ONLY) 
		return;
	else if (!g_NetObjectSession->isHost() && g_auth == CLIENT_ONLY) 
		return;

	// clients do shit every frame... 
	for(auto iterator = s_netObjects.begin(); iterator != s_netObjects.end(); ++iterator)
	{
		NetObject* nop = iterator->second;
		if(nop == nullptr)
			continue;
		nop->definition->apply_snapshot(nop->local_object, nop->last_received_snapshot);
	}
	//*/
}

void NetObjectSystem::SendNetObjectUpdates()
{
	//*/
	for (auto iterator = s_netObjects.begin(); iterator != s_netObjects.end(); ++iterator)
	{
		// current snapshot only needs to be allocated
		// once and can be over-written
		NetObject* nop = iterator->second;
		if (nop == nullptr)
			continue;
		nop->definition->get_current_snapshot(nop->current_snapshot, nop->local_object);
	}

	/*/
	for(NetConnection *cp : g_NetObjectSession->m_connections) 
	{
		SendNetObjectUpdateTo(cp);
	}
	//*/
	LinkNode<NetConnection *>* n = g_NetObjectSession->m_connectionList.m_head;
	for (uint i = 0; i < g_NetObjectSession->m_connectionList.length; ++i)
	{
		NetConnection* cp = n->data;
		//if(cp != g_NetObjectSession->host_connection)
			SendNetObjectUpdateTo(cp);
		n = n->sibling_right;
	}
}

void NetObjectSystem::SendNetObjectUpdateTo(NetConnection *cp)
{
	//*/
	for (auto iterator = s_netObjects.begin(); iterator != s_netObjects.end(); ++iterator)
	{
		NetObject* nop = iterator->second;
		if (nop == nullptr)
			continue;

		NetMessage* update_msg = new NetMessage(NETOBJECT_SEND_UPDATE);

		update_msg->write(nop->net_id);

		nop->definition->append_snapshot(update_msg, nop->current_snapshot);
		if(update_msg==nullptr || update_msg->message_type_index == NET_MSG_INVALID)
			continue;

		cp->Send(update_msg);
	}
	//*/
}

void NetObjectSystem::OnNetObjectUpdateRecieved(NetMessage *update_msg)
{
	UNUSED(update_msg);
	//*/
	uint16 net_id;
	update_msg->read(net_id);

	NetObject *nop = FindNetObject(net_id);
	if (nullptr == nop) {
		return;
	}

	// like current snapshot - we can just overwrite here
	// so only allocate this memory once. 
	nop->definition->process_snapshot(nop->last_received_snapshot, update_msg);

	// apply it? 
	//*/
}

void NetObjectSystem::SendCreateInfosForJoinRequest(NetConnection *cp)
{
	for (auto iterator = s_netObjects.begin(); iterator != s_netObjects.end(); ++iterator)
	{
		NetObject* nop = iterator->second;
		if (nop == nullptr)
			continue;

		NetMessage create(NETOBJECT_CREATE_OBJECT);
		create.write(GetDefsId(nop->definition));
		create.write(nop->net_id);

		nop->definition->append_create_info(&create, nop->local_object);

		//nop->definition->process_create_info(update_msg, nop);

		cp->Send(&create);
	}
}

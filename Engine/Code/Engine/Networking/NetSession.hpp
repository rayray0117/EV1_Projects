#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/Core/Event.hpp"
#include "Engine/Core/LinkList.hpp"
#include <functional>
//////////////////////////////////////////////////////////////////////////
class NetConnection;
class NetMessageDefinition;
class NetMessage;
//////////////////////////////////////////////////////////////////////////
enum ESessionState
{
	SESSION_DISCONNECTED,
	SESSION_CONNECTING,
	//SESSION_JOINING,
	SESSION_CONNECTED,
	NUM_SESSION_STATES
};
//////////////////////////////////////////////////////////////////////////
class NetSession
{
public:
	NetSession() {}
	virtual ~NetSession() {}

public:
	virtual bool Host(uint16 port) = 0;
	virtual bool Join(net_address const &addr) = 0; // production version: session_join_info_t instead
	virtual void Leave() = 0;
	virtual void Update() = 0;

	virtual bool StartListening() = 0;
	virtual void StopListening() = 0;
	virtual bool isListening() const = 0;
public:
	bool register_message_definition(uint8 msg_id, NetMessageDefinition& defn);
	bool register_message_function( uint8 msg_id, std::function<void(NetMessage*)> /*Event<void(NetMessage*)>*/ const &callback );

	NetMessageDefinition* get_message_definition(uint8 id) const;


	inline bool isHost() const
	{
		return (my_connection == host_connection) && (host_connection != nullptr);
	}

	inline bool isClient() const
	{
		return (my_connection != host_connection) && (my_connection != nullptr);
	}

	inline bool isRunning() const 
	{
		return (my_connection != nullptr);
	}

	inline bool isReady()
	{
		return (m_state == SESSION_CONNECTED);
	}

	inline void SetState(ESessionState state)
	{
		m_state = state;
	}

	uint8 getFreeConnectionIndex() const;

	void JoinConnection(uint8 idx, NetConnection* conn);
	void DestroyConnection(NetConnection* cp);

	NetConnection* getConnection(uint8 index);
	void SendMessageToIndex(uint8 index, const NetMessage& msg);
	void SendMessageToOthers(const NetMessage& msg);
	void SendMessageToAll(const NetMessage& msg);

public:
	ESessionState m_state;

	// connection info
	LinkList<NetConnection*> m_connectionList;

	std::vector<NetConnection*> m_connections;
	uint m_maxConnectionCount = 16;

	NetConnection* my_connection = nullptr;		// helpers
	NetConnection* host_connection = nullptr; 	// 

	// message data;
	std::vector<NetMessageDefinition*> message_defintions;
};
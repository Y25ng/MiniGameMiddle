

#pragma once


#include "CoreMinimal.h"
#include "Protocol.h"


// #define SERVERIP "192.168.194.162" // ±èÁöÀº´Ô ¾îÇÃ IP
// #define SERVERIP "172.28.35.240" // ±èÁöÀº´Ô IP
// #define SERVERIP "172.28.35.239" // 
 #define SERVERIP "192.168.0.9"
// #define SERVERIP "192.168.0.24" // ÀÓ½Ã IP
#define SERVERPORT 9000


class AMiniGameCharacter;


class ServerManager
{
private:
	FSocket* m_socket;
	char m_buf[InitPacket::MAX_BUFFERSIZE];
	unsigned char m_previousPacketSize;

public:
	static ServerManager& GetInstance()
	{
		static ServerManager instance;
		return instance;
	}

private:

	ServerManager();
	~ServerManager();
	
	AMiniGameCharacter* m_Character;
	AMiniGameCharacter* m_Character2;
	AMiniGameCharacter* m_Character3;
	
	// bool m_bGameStart;

public:
	ServerManager( ServerManager const& ) = delete;
	ServerManager& operator=( ServerManager const& ) = delete;

	ServerManager( ServerManager&& ) = delete;
	ServerManager& operator=( ServerManager&& ) = delete;

	// Init
	void Initialize();
	bool ConnectToServer();
	void ShutDown();

	// ÆÐÅ¶ ¼Û¼ö½Å ÇÔ¼ö
	void RecvPacket();
	void SendPacket( char datainfo, void* packet );
	void ProcessPacket( char* packet );

	AMiniGameCharacter* GetCharacter() { return m_Character; }
	void SetCharacter(AMiniGameCharacter* varCharacter) { m_Character = varCharacter; }

	AMiniGameCharacter* GetCharacter2() { return m_Character2; }
	void SetCharacter2(AMiniGameCharacter* varCharacter) { m_Character2 = varCharacter; }

	AMiniGameCharacter* GetCharacter3() { return m_Character3; }
	void SetCharacter3(AMiniGameCharacter* varCharacter) { m_Character3 = varCharacter; }

	void SetOtherCharacterStartInfo( Packet::InitPlayers& p, int playerMapSize );
	// void SetCharacterMoveInfo( Packet::Move& p );
	
	/*
	bool GetbGameStart() { return m_bGameStart; }
	void SetbGameStart( bool var ) { m_bGameStart = var; }
	*/
};



#include "ServerManager.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "UserManager.h"
#include "UIManager.h"
#include "ActorManager.h"
#include "MiniGameCharacter.h"
#include <algorithm>


ServerManager::ServerManager()
    :m_buf(), m_previousPacketSize(0), m_Character(nullptr), m_Character2(nullptr), m_Character3(nullptr), m_bGameStart(false)
{
}

ServerManager::~ServerManager()
{
}

void ServerManager::Initialize()
{
}

bool ServerManager::ConnectToServer()
{
	/// ������ connect ��û
	m_socket = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateSocket( NAME_Stream, TEXT( "default" ) );
    if (!m_socket)
    {
        return false;
    }

	FIPv4Address ip;
	if ( !FIPv4Address::Parse( SERVERIP, ip ) )
    {
        return false;
    }

	TSharedRef<FInternetAddr> address = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM )->CreateInternetAddr();
	address->SetIp( ip.Value );
	address->SetPort( SERVERPORT );

	bool connect = m_socket->Connect( *address );

	if ( !connect )
    {
        return false;
    }

	m_socket->SetNonBlocking( true );
	m_socket->SetNoDelay( true );

    return true;
}

void ServerManager::ShutDown()
{
    if (m_socket)
    {
        m_socket->Close();
    }
}
// 10 7 7
void ServerManager::RecvPacket()
{
    if ( !m_socket )
        return;

    char buf2[InitPacket::MAX_BUFFERSIZE];
    char* packet = m_buf;
    int32 bytesSents = 0;
    int32 packetSize = 0;
    bool initCome = true;

    bool returnValue = m_socket->Recv((uint8*)(buf2), InitPacket::MAX_BUFFERSIZE - 1, bytesSents);
    if ( !returnValue )
    {
        return;
    }
    // ���� ������ ����
    else if ( bytesSents == 0 )
    {
        return;
    }

    // ������ �޾Ҵ� ��Ŷ�� ���� ��� �� �ڿ�, ������ �޾Ҵ� ��Ŷ�� ������ m_previousPacketSize �� 0
    std::copy(buf2, buf2 + bytesSents, m_buf + m_previousPacketSize);

    do
    {
        // �޾ƿ� ������
        packetSize = packet[0];
        
        // �޾ƿ� ��Ŷ ������ ũ�⺸�� ���� �����Ͱ� ���� ��� -> ��Ŷ�� �����ؼ� ProcessPacket ����
        if (packetSize <= bytesSents)
        {
            char assemble[InitPacket::MAX_BUFFERSIZE] = { NULL, };
            std::copy(m_buf, m_buf + packetSize, assemble);
            ProcessPacket(assemble);

            packet += packetSize;
            bytesSents -= packetSize;

            if (bytesSents != 0)
            {
                char initBuffer[InitPacket::MAX_BUFFERSIZE];

                std::copy(packet, packet + InitPacket::MAX_BUFFERSIZE, initBuffer);
                std::copy(initBuffer, initBuffer + InitPacket::MAX_BUFFERSIZE, m_buf);
                packet = m_buf;
            }
            else
            {
                m_previousPacketSize = 0;
                ZeroMemory(m_buf, sizeof(m_buf));
            }
        }
        // ��Ŷ ������ ũ�Ⱑ ���� �����ͺ��� ���� -> ��Ŷ�� �� �޾ƾ� ��
        else
        {
            m_previousPacketSize += bytesSents;
        }
    } while (bytesSents > 0);
}

void ServerManager::SendPacket( char datainfo, void* packet )
{
    if ( packet == nullptr )
        return;

    switch ( datainfo )
    {
    case ClientToServer::LOGIN_REQUEST:
    {
        Packet::LoginRequest p = *( Packet::LoginRequest* )( packet );
        int32 bytesSents = 0;
        m_socket->Send( ( uint8* )( packet ), sizeof( p ), bytesSents );
    }
    break;
    case ClientToServer::MOVE:
    {
        Packet::Move p = *(Packet::Move*)( packet );
        int32 bytesSents = 0;
        m_socket->Send( (uint8*)( packet ), sizeof( p ), bytesSents );
    }
    break;
    default:
        break;
    }
}

void ServerManager::ProcessPacket( char* packet )
{
    if ( !packet )
        return;

    switch ( packet[ 1 ] )
    {
    case ServerToClient::FIRSTINFO:
    {
        Packet::FirstPlayer p = *reinterpret_cast<Packet::FirstPlayer*> (packet);
        p.owner;

        if ( m_Character == nullptr )
            break;

        UserManager::GetInstance().PushPlayer(p.owner, m_Character);
        
    }
    break;
    case ServerToClient::LOGON_OK:
    {
        Packet::LoginResult p = *reinterpret_cast< Packet::LoginResult* > ( packet );
        
    }
    break;
    case ServerToClient::LOGON_FAILED:
    {
        Packet::LoginResult p = *reinterpret_cast< Packet::LoginResult* > ( packet );

    }
    case ServerToClient::INITPLAYERS:
    {
  
        Packet::InitPlayers p = *reinterpret_cast< Packet::InitPlayers* > ( packet );
        int32 playerMapSize = UserManager::GetInstance().GetPlayerMap().Num();

        // ��ü p�� ����ִ� ������ ���� �÷��̾ ���� �������
        if ( UserManager::GetInstance().GetPlayerMap().Find( p.owner ) )
        {
            // ���� �÷��̾ ���� ���� �Ҵ�
            UserManager::GetInstance().SetPlayerDefaultInfo( p.owner, p.x, p.y, p.directionX, p.directionY, p.color );
            UserManager::GetInstance().SetMainCharacterIndex( p.owner );

            break;
        }
        else // ��ü p�� ����ִ� ������ ���� �÷��̾ ���� ������ �ƴ϶��
        {
            // �ٸ� ĳ���Ϳ� ���� ������ �Ҵ�
            SetOtherCharacterStartInfo( p, playerMapSize );
        }     
    }
    break;
    case ServerToClient::MOVE:
    {
        if ( ActorManager::GetInstance().GetLobbyBottom() != nullptr )
        {
            return;
        }

        Packet::Move p = *reinterpret_cast< Packet::Move* > ( packet );

        if ( UserManager::GetInstance().GetPlayerMap().Find( p.owner ) == nullptr )
        {
            return;
        }

        // ���� ������ �÷��̾��� ĳ���͵��� �����ӿ� ���� ���� ����
        SetCharacterMoveInfo( p );
    }
    case ServerToClient::TIME:
    {
        Packet::Timer p = *reinterpret_cast<Packet::Timer*> (packet);

        // ���� �÷��̾ ���� ���� �Ҵ�
        UserManager::GetInstance().SetPlayerTime(p.time);
        UIManager::GetInstance().SetGameTimeSec(p.time);

        break;
    }

    case ServerToClient::COLLISION_BLOCK:
    {
        Packet::CollisionTile p = *reinterpret_cast<Packet::CollisionTile*>( packet );
        ActorManager::GetInstance().ChangeBottomColor( UserManager::GetInstance().GetCharacterColor( p.owner ), p.tileIndex );
        {
            UE_LOG( LogTemp, Error, TEXT( "%d" ), p.tileIndex );
        }  
    }
    break;
    case ServerToClient::COLLISION_PLAYER:
    {
        Packet::CollisionPlayer p = *reinterpret_cast< Packet::CollisionPlayer* >( packet );

        for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM;  i++ )
        {
            if ( p.owners[ i ] == -1 )
                continue;

            int32 playerKey = p.owners[ i ];
            UserManager::GetInstance().GetPlayerMap()[ playerKey ]->ApplyPlayerForces( p.owners );
        }

    }
    break;
    case ServerToClient::COLLISION_WALL:
    {
        Packet::CollisionWall p = *reinterpret_cast< Packet::CollisionWall* >( packet );

        for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM; i++ )
        {
            int32 playerKey = p.owner;
            UserManager::GetInstance().GetPlayerMap()[ playerKey ]->ApplyWallForces( p.wallNum );
        }
    }
    break;

    default:
        break;
    }
}

// ���� �÷��̾ �ƴ� �ٸ� �÷��̾� ĳ���͵鿡 ���� ���� �Ҵ�
void ServerManager::SetOtherCharacterStartInfo( Packet::InitPlayers& p, int playerMapSize )
{
    if ( playerMapSize == 1 )
    {
        UserManager::GetInstance().PushPlayer( p.owner, m_Character2 );
    }
    else if ( playerMapSize == 2 )
    {
        UserManager::GetInstance().PushPlayer( p.owner, m_Character3 );
    }

    UserManager::GetInstance().SetPlayerDefaultInfo( p.owner, p.x, p.y, p.directionX, p.directionY, p.color );
}

// ���� ������ �÷��̾��� ĳ���͵��� �����ӿ� ���� ���� ���� �Լ�
void ServerManager::SetCharacterMoveInfo( Packet::Move& p )
{
    // ĳ������ ���� ��ġ ������ ��ǥ ��ġ ���� �Ҵ�
    FVector tempLocation = FVector( p.x, p.y, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation().Z );
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetStartLocation( UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation() );
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetLocation( tempLocation );

    // ĳ���Ͱ� ���� ���ϰ� �ִ� ���� ���� �Ҵ�
    FVector tempDirection = FVector( p.directionX, p.directionY, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorForwardVector().Z );
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetDirection( tempDirection );

    float tempSpeed = p.speed;
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetSpeed( tempSpeed );

    // �����κ��� ĳ������ ������ ������ ������ ������ ���� ���θ� ��Ÿ���� �÷��� ������ true �� �Ҵ�
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetbRecvLocation( true );
}



#include "ServerManager.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "UserManager.h"
#include "UIManager.h"
#include "MainUI.h"
#include "LogInUI.h"
#include "SignupUI.h"
#include "ActorManager.h"
#include "TimeManager.h"
#include "MiniGameCharacter.h"
#include <algorithm>


ServerManager::ServerManager()
    :m_buf(), m_previousPacketSize(0), m_Character(nullptr), m_Character2(nullptr), m_Character3(nullptr) // , m_bGameStart(false)
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
	/// 서버에 connect 요청
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
    // 받은 데이터 없음
    else if ( bytesSents == 0 )
    {
        return;
    }


    if ( bytesSents >= InitPacket::MAX_BUFFERSIZE )
    {
        FString DebugMessage = FString::Printf( TEXT( "Too many packet" ) );
        GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Red, DebugMessage );
    }

    // 이전에 받았던 패킷이 있을 경우 그 뒤에, 이전에 받았던 패킷이 없으면 m_previousPacketSize 는 0
    std::copy(buf2, buf2 + bytesSents, m_buf + m_previousPacketSize);

    do
    {
        // 받아올 데이터
        packetSize = packet[0];
        
        // 받아올 패킷 데이터 크기보다 받은 데이터가 많을 경우 -> 패킷을 조립해서 ProcessPacket 수행
        if (packetSize <= bytesSents)
        {
            char assemble[InitPacket::MAX_BUFFERSIZE] = { '\0',};
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
                memset( m_buf, 0, sizeof( m_buf ) );
            }
        }
        // 패킷 데이터 크기가 받은 데이터보다 많음 -> 패킷을 더 받아야 함
        else
        {
            m_previousPacketSize += bytesSents;
            return;
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
    case ClientToServer::SKILLUSE_REQUEST:
    {
        Packet::SkillUseRequest p = *( Packet::SkillUseRequest* ) ( packet );
        int32 bytesSents = 0;
        m_socket->Send( ( uint8* ) ( packet ), sizeof( p ), bytesSents );
    }
    break;
    case ClientToServer::SIGNUP_REQUEST:
    {
        Packet::SignUpRequest p = *(Packet::SignUpRequest*)( packet );
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
        UIManager::GetInstance().RemoveUI( EUIPathKey::LOGIN );
        UIManager::GetInstance().CreateUI< UMainUI >( UserManager::GetInstance().GetWorld(), EUIPathKey::MAIN );
        UIManager::GetInstance().AddUI( EUIPathKey::MAIN );

        // FString nickname = FString( ANSI_TO_TCHAR()
        // UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetNickName(  )
    }
    break;
    case ServerToClient::LOGON_FAILED:
    {
        Packet::LoginResult p = *reinterpret_cast< Packet::LoginResult* > ( packet );
        Cast< ULogInUI >( UIManager::GetInstance().GetWidget( EUIPathKey::LOGIN ) )->SetLogInConditionOpacity( 1.0f );
    }
    case ServerToClient::INITPLAYERS:
    {
  
        Packet::InitPlayers p = *reinterpret_cast< Packet::InitPlayers* > ( packet );
        int32 playerMapSize = UserManager::GetInstance().GetPlayerMap().Num();

        // 객체 p가 담고있는 정보가 현재 플레이어에 대한 정보라면
        if ( UserManager::GetInstance().GetPlayerMap().Find( p.owner ) )
        {
            // 현재 플레이어에 대한 정보 할당
            UserManager::GetInstance().SetPlayerDefaultInfo( p.owner, p.x, p.y, p.directionX, p.directionY, p.color );
            UserManager::GetInstance().SetMainCharacterIndex( p.owner );

            break;
        }
        else // 객체 p가 담고있는 정보가 현재 플레이어에 대한 정보가 아니라면
        {
            // 다른 캐릭터에 대한 정보들 할당
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

        // 나를 제외한 플레이어의 캐릭터들의 움직임에 대한 정보 세팅
        UserManager::GetInstance().SetCharacterMoveInfo( p );
    }
    case ServerToClient::TIME:
    {
        Packet::Timer p = *reinterpret_cast<Packet::Timer*> (packet);
        int tempTime = static_cast< int >( p.time );

        // 현재 플레이어에 대한 정보 할당
        TimeManager::GetInstance().SetGameTimeSec( tempTime );

        if ( tempTime == ETimeType::DELETELOBBYBOTTOM )
        {
            ActorManager::GetInstance().DeleteLobbyBottom();
        }

        if ( tempTime == ETimeType::GAMESTART )
        {
            TimeManager::GetInstance().SetbGameStart( true );
            // m_bGameStart = true;
        }

        break;
    }

    case ServerToClient::COLLISION_BLOCK:
    {
        Packet::CollisionTile p = *reinterpret_cast<Packet::CollisionTile*>( packet );
        ActorManager::GetInstance().ChangeBottomColor( UserManager::GetInstance().GetCharacterColor( p.owner ), p.tileIndex );
        {
            // UE_LOG( LogTemp, Error, TEXT( "%d" ), p.tileIndex );
        }  
    }
    break;
    case ServerToClient::COLLISION_PLAYER:
    {
        Packet::CollisionPlayer p = *reinterpret_cast< Packet::CollisionPlayer* >( packet );

        TMap<int32, bool> strongPlayerMap;
        bool bHasStrongPlayer = false;


        for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM; i++ )
        {
            if ( p.owners[ i ] == -1 || UserManager::GetInstance().GetPlayerMap().Find( p.owners[ i ] ) == nullptr )
            {
                continue;
            }

            if ( UserManager::GetInstance().GetPlayerMap()[ p.owners[ i ] ]->GetbStrong() == true )
            {
                strongPlayerMap.Add( p.owners[ i ], true );
                bHasStrongPlayer = true;
            }
            else
            {
               strongPlayerMap.Add( p.owners[ i ], false );
            }
        }

        if ( bHasStrongPlayer == true )
        {
            for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM; i++ )
            {
                if ( strongPlayerMap.Find( p.owners[ i ] ) == nullptr )
                {
                    continue;
                }

                if ( strongPlayerMap[ p.owners[ i ] ] == false )
                {
                    UserManager::GetInstance().GetPlayerMap()[ p.owners[ i ] ]->SetPlayerStun( true );
                }
            }
        }

        for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM;  i++ )
        {
            if ( p.owners[ i ] == -1 )
                continue;

            int32 playerKey = p.owners[ i ];

            if ( bHasStrongPlayer == false )
            {              
                UserManager::GetInstance().GetPlayerMap()[ playerKey ]->ApplyPlayerForces( p.owners );
            }
            else
            {
                if ( strongPlayerMap[ playerKey ] == true )
                {
                    UserManager::GetInstance().GetPlayerMap()[ playerKey ]->ApplyPlayerForces( p.owners );
                }     
            }
        }

        for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM; i++ )
        {
            if ( p.owners[ i ] == -1 )
                continue;

            int32 playerKey = p.owners[ i ];

            if ( strongPlayerMap[ playerKey ] == true )
                UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetbStrong( false );
        }
    }
    break;
    case ServerToClient::COLLISION_WALL:
    {
        /*
        Packet::CollisionWall p = *reinterpret_cast< Packet::CollisionWall* >( packet );

        for ( int i = 0; i < InitWorld::INGAMEPLAYER_NUM; i++ )
        {
            int32 playerKey = p.owner;
            UserManager::GetInstance().GetPlayerMap()[ playerKey ]->ApplyWallForces( p.wallNum );
        }
        */
    }
    break;
    case ServerToClient::ENDGAME:
    {

    }
    break;
    case ServerToClient::SKILLUSE_REQUEST_SUCCESS:
    {
        Packet::SkillUseResult p = *reinterpret_cast< Packet::SkillUseResult* >( packet );

        int32 playerKey = p.owner;
        UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetbStrong( true );
        UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetMP( 0.0f );

        if ( playerKey == UserManager::GetInstance().GetMainCharacterIndex() )
        {
            Cast< UMainUI >( UIManager::GetInstance().GetWidget( EUIPathKey::MAIN ) )->UpdateMP();
        }
    }
    break;
    case ServerToClient::SKILLUSE_REQUEST_FAILED:
    {

    }
    break;
    case ServerToClient::MP_UPDATE:
    {
        Packet::PlayerMpUpdate p = *reinterpret_cast< Packet::PlayerMpUpdate* >( packet );

        int32 playerKey = p.owner;
        UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetMP( p.mp );
        Cast< UMainUI >( UIManager::GetInstance().GetWidget( EUIPathKey::MAIN ) )->UpdateMP();
    }
    break;
    case ServerToClient::SKILLEND:
    {
        Packet::SkillEnd p = *reinterpret_cast< Packet::SkillEnd* >( packet );
        int32 playerKey = p.owner;
        UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetbStrong( false );
    }
    break;
    case ServerToClient::SIGNUP_OK:
    {
        Packet::SignUpResult p = *reinterpret_cast<Packet::SignUpResult*>( packet );
        int32 playerKey = p.owner;

        UIManager::GetInstance().RemoveUI( EUIPathKey::SIGNUP );
        // FString nickname = FString( ANSI_TO_TCHAR(  )        
        // UserManager::GetInstance().GetPlayerMap()[ playerKey ]->SetNickName(  )
    }
    case ServerToClient::SIGNUP_FAILED:
    {
        Packet::SignUpResult p = *reinterpret_cast<Packet::SignUpResult*>( packet );
        int32 playerKey = p.owner;
        Cast< USignupUI >( UIManager::GetInstance().GetWidget( EUIPathKey::SIGNUP ) )->SetSignupConditionOpacity( 1.0f );
    }
    break;
    default:
        break;
    }
}

// 현재 플레이어가 아닌 다른 플레이어 캐릭터들에 대한 정보 할당
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

/*
// 나를 제외한 플레이어의 캐릭터들의 움직임에 대한 정보 세팅 함수
void ServerManager::SetCharacterMoveInfo( Packet::Move& p )
{
    // 캐릭터의 시작 위치 정보와 목표 위치 정보 할당
    FVector tempLocation = FVector( p.x, p.y, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation().Z );
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetStartLocation( UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorLocation() );
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetLocation( tempLocation );

    // 캐릭터가 현재 향하고 있는 방향 정보 할당
    FVector tempDirection = FVector( p.directionX, p.directionY, UserManager::GetInstance().GetPlayerMap()[ p.owner ]->GetActorForwardVector().Z );
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetTargetDirection( tempDirection );

    float tempSpeed = p.speed;
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetSpeed( tempSpeed );

    // 서버로부터 캐릭터의 움직임 정보를 받을지 말지의 대한 여부를 나타내는 플래그 변수에 true 값 할당
    UserManager::GetInstance().GetPlayerMap()[ p.owner ]->SetbRecvLocation( true );
}
*/

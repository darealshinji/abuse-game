#include "dp_prot.h"
#include "jmalloc.hpp"
#include "jwindow.hpp"
#include "cache.hpp"
#include <ctype.h>
#include <time.h>

#include "jwindow.hpp"
#include "netcfg.hpp"
#include "input.hpp"

#include "crtdbg.h"
#include "console.hpp"

#include "dprint.hpp"

LPDIRECTPLAY4A       g_pDP;
LPDIRECTPLAYLOBBY3A  g_pDPLobby;
// {7FAABB28-D1AB-4b3b-957C-58A124E03542}
static const GUID ABUSE_GUID = GUID_NULL;
//{0x7faabb28, 0xd1ab, 0x4b3b, {0x95, 0x7c, 0x58, 0xa1, 0x24, 0xe0, 0x35, 0x42}};

struct SESSIONINFO {
    GUID SessionGUID;
    int found;
    char* Name;
};

HANDLE DirectPlayThreadID = NULL;
int SessionCount = 0;
int ProtocolCount = 0;
int DPConnected = 0;
DPPROTOCOL **DPProtocolList = NULL;
SESSIONINFO **AbuseSessions = NULL;
DPID ServerPlayer = 0;
DPID LocalPlayer = 0;

int AmServer = 0;
DP_protocol DirectPlayProt;
DP_protocol *DP;
int EnumeratingSessions = 0;

GUID CurrentSession = GUID_NULL;
DP_BIG_PACKET BigPacket;

int g_SelectedProtocol = -1;

char* HostIP = "";
unsigned short sPort = 0;

#define NAMEMAX 33

BOOL FAR PASCAL EnumSessions(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext);
void FindSessions();

/* --------------------------------------------------------------------------- 
Creates the DP address from data collected in the session init dialog
 ------------------------------------------------------------------------- /**/
long MakeDPAddress(VOID** ppAddress, DWORD* pdwAddressSize) {
    DPCOMPOUNDADDRESSELEMENT addressElements[3];
    VOID*   pAddress = NULL;
    DWORD   dwAddressSize = 0;
    DWORD   dwElementCount;
    GUID    guidServiceProvider;
    HRESULT hRet;

    if (!DPProtocolList) // Make sure there are protocols to select from...
        return 0;
    
    dwElementCount = 0;
    if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_MODEM)) {
        // Modem needs a service provider, a phone number string and a modem string

        // Dervice provider
        addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
        addressElements[dwElementCount].dwDataSize   = sizeof(GUID);
        addressElements[dwElementCount].lpData       = (VOID*)&DPSPGUID_MODEM;
        dwElementCount++;

        /*// Sdd a modem string if available
        lstrcpy( strModemString, "" );
        if( GetDlgItemText( hWnd, IDC_MODEMCOMBO, strModemString, NAMEMAX ) )
        {
            addressElements[dwElementCount].guidDataType = DPAID_Modem;
            addressElements[dwElementCount].dwDataSize   = lstrlen(strModemString) + 1;
            addressElements[dwElementCount].lpData       = strModemString;
            dwElementCount++;
        }

        // Add phone number string
        lstrcpy( strPhoneNumberString, "" );
        GetDlgItemText(hWnd, IDC_PHONEEDIT, strPhoneNumberString, NAMEMAX);
        addressElements[dwElementCount].guidDataType = DPAID_Phone;
        addressElements[dwElementCount].dwDataSize   = lstrlen(strPhoneNumberString) + 1;
        addressElements[dwElementCount].lpData       = strPhoneNumberString;
        dwElementCount++; */
    }
    else if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_TCPIP)) {
        // TCP/IP needs a service provider, an IP address, and optional port #
        // Service provider
        addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
        addressElements[dwElementCount].dwDataSize   = sizeof(GUID);
        addressElements[dwElementCount].lpData       = (VOID*)&DPSPGUID_TCPIP;
        dwElementCount++;

        // IP address string
        addressElements[dwElementCount].guidDataType = DPAID_INet;
        addressElements[dwElementCount].dwDataSize   = lstrlen(HostIP) + 1;
        addressElements[dwElementCount].lpData       = HostIP;
        dwElementCount++;

        // Optional Port number
        if (sPort > 0) {
            addressElements[dwElementCount].guidDataType = DPAID_INetPort;
            addressElements[dwElementCount].dwDataSize   = sizeof(WORD);
            addressElements[dwElementCount].lpData       = &sPort;
            dwElementCount++;
        }
    }
    else if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_IPX)) {
        // IPX just needs a service provider

        // Service provider
        addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
        addressElements[dwElementCount].dwDataSize   = sizeof(GUID);
        addressElements[dwElementCount].lpData       = (VOID*)&DPSPGUID_IPX;
        dwElementCount++;
    }
    else {
        // Anything else, let service provider collect settings, if any
        
        // Service provider
        addressElements[dwElementCount].guidDataType = DPAID_ServiceProvider;
        addressElements[dwElementCount].dwDataSize   = sizeof(GUID);
        addressElements[dwElementCount].lpData       = (VOID*)&guidServiceProvider;
        dwElementCount++;
    }

    // See how much room is needed to store this address
    hRet = g_pDPLobby->CreateCompoundAddress(addressElements, dwElementCount, NULL, &dwAddressSize);
    if (hRet != DPERR_BUFFERTOOSMALL)
        return hRet;

    // Allocate space
    pAddress = malloc(dwAddressSize);
    if (pAddress == NULL)
        return DPERR_NOMEMORY;

    // Create the address
    hRet = g_pDPLobby->CreateCompoundAddress(addressElements, dwElementCount, pAddress, &dwAddressSize);
    if (FAILED(hRet)) {
        delete pAddress;
        return hRet;
    }

    // Return the address info */
    *ppAddress = pAddress;
    *pdwAddressSize = dwAddressSize; 

    return DP_OK;
}

/* --------------------------------------------------------------------------- 
Initializes the service provider and begins session enumeration
 ------------------------------------------------------------------------- /**/
int InitSession()
{
    VOID*          pAddress      = NULL;
    DWORD          dwAddressSize = 0;
    HRESULT hRet;

    hRet = MakeDPAddress(&pAddress, &dwAddressSize);
    if (FAILED(hRet))
        return 0;
    // Initialize the connection using the address
    hRet = g_pDP->InitializeConnection(pAddress, 0);
    if (hRet == DP_OK)
        DPConnected = 1;
    else {
        _RPT1(_CRT_WARN, "InitSession -- DirectPlay Error!  Couldn't initialize connection!  (hRet = 0x%x)\n", hRet);
        return 0;
    }
    return 1;
}

/* --------------------------------------------------------------------------- 
EnumPlayers callback function to determine the server player
 ------------------------------------------------------------------------- /**/
BOOL FAR PASCAL EnumPlayers(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
    if (dwFlags & DPENUMPLAYERS_SERVERPLAYER)
        ServerPlayer = dpId;
    return TRUE;
}

/* --------------------------------------------------------------------------- 
EnumConnections callback; returns list of usable protocols
 ------------------------------------------------------------------------- /**/
BOOL FAR PASCAL EnumConnections(LPCGUID lpguidSP, LPVOID lpConnection, DWORD dwConnectionSize, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
    ProtocolCount++;
    DPProtocolList = (DPPROTOCOL**) jrealloc(DPProtocolList, sizeof(DPProtocolList[0]) * ProtocolCount, "innet:EnumConnections - DPProtocolList");
    int i = ProtocolCount - 1;
    DPProtocolList[i] = (DPPROTOCOL*) jmalloc(sizeof(DPPROTOCOL), "innet:EnumConnections - DPProtocolList entry");
    DPProtocolList[i]->guid = *lpguidSP;
    DPProtocolList[i]->ConnectionSize = dwConnectionSize;
    DPProtocolList[i]->Connection = jmalloc(dwConnectionSize, "innet:EnumConnections - DPConnection");
    memcpy(DPProtocolList[i]->Connection, lpConnection, dwConnectionSize);
    DPProtocolList[i]->Name = *lpName;
    DPProtocolList[i]->dwFlags = dwFlags;
    return TRUE;
}

/* --------------------------------------------------------------------------- 
EnumSessions callback; finds games that have been created by others
 ------------------------------------------------------------------------- /**/
BOOL FAR PASCAL EnumSessions(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext)
{
    if (dwFlags & DPESC_TIMEDOUT)
        return FALSE; // Timed out, so stop the enumeration.

    int i;
    for (i = 0; i < SessionCount; i++) {
        if (AbuseSessions[i]->SessionGUID == pdpsd->guidInstance) {
//            AbuseSessions[i]->found = 0;
            return TRUE; // Have already enumerated this session, don't return it again...
        }
    }
    // Found a new session, save it.
    SessionCount++;
    AbuseSessions = (SESSIONINFO**) jrealloc(AbuseSessions, sizeof(AbuseSessions[0]) * SessionCount, "EnumSessions: Session List");
    AbuseSessions[SessionCount - 1] = (SESSIONINFO*) jmalloc(sizeof(SESSIONINFO), "EnumSessions: Session List entry");
    AbuseSessions[SessionCount - 1]->SessionGUID = pdpsd->guidInstance;
    AbuseSessions[SessionCount - 1]->Name = (char*) jmalloc(strlen(pdpsd->lpszSessionNameA) + 1, "");
    AbuseSessions[SessionCount - 1]->found = 0;
    strcpy(AbuseSessions[SessionCount - 1]->Name, pdpsd->lpszSessionNameA);
    return TRUE;
}

/* --------------------------------------------------------------------------- 
Starts session enumeration
 ------------------------------------------------------------------------- /**/
void FindSessions() {
    static DWORD LastCheck = 0;
    DWORD Now = GetTickCount();
    HRESULT hRet;
    DPSESSIONDESC2 dpsd;
    ZeroMemory(&dpsd, sizeof(dpsd));
    dpsd.dwSize = sizeof(dpsd);
    dpsd.guidApplication = ABUSE_GUID;
    if (Now - LastCheck > 1000) {
        hRet = g_pDP->EnumSessions(&dpsd, 0, EnumSessions, NULL, DPENUMSESSIONS_ASYNC | DPENUMSESSIONS_AVAILABLE);
        LastCheck = Now;
        if (hRet == DP_OK)
            EnumeratingSessions = 1;
    }
}

/* --------------------------------------------------------------------------- 
Release the DirectPlay interfaces
 ------------------------------------------------------------------------- /**/
void FreeDirectPlay()
{
    int i;
    for (i = 0; i < ProtocolCount; i++) {
        delete DPProtocolList[i]->Connection;
        delete DPProtocolList[i];
    }
    delete DPProtocolList;
    DPProtocolList = NULL;

    ProtocolCount = 0;

    if (g_pDPLobby)
        g_pDPLobby->Release();
    if (g_pDP) {
        g_pDP->Release();
    }
    DPConnected = 0;
    g_pDPLobby = NULL;
    g_pDP = NULL;
    DP = NULL;
}

int ComInitialized = 0;

/* --------------------------------------------------------------------------- 
Initializes COM if it's not already started, creates g_pDP and g_pDPLobby
interfaces, then enumerates session providers.
 ------------------------------------------------------------------------- /**/
int StartDirectPlay()
{   
    HRESULT hRet;

    if (g_pDP)
        return 1;
    if (!ComInitialized) {
        hRet = CoInitialize(NULL);
        if (hRet == S_OK)
            ComInitialized = 1;
        else {
            dprintf("DirectPlay startup error!\nDirectPlay: Couldn't initialize COM library.\n    CoInitialize returned 0x%x\n", hRet);
            return 0;
        }
    }
    hRet = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL, IID_IDirectPlay4A, (VOID**) &g_pDP);
    if (hRet) {
        dprintf("DirectPlay startup error!\nDirectPlay interface failed to initialize.  (hRet = 0x%x)\n", hRet);
        return 0;
    }

    hRet = CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A, (VOID**) &g_pDPLobby);
    if (hRet) {
        dprintf("DirectPlay startup error!\nDirectPlayLobby interface failed to initialize.  (hRet = 0x%x)\n", hRet);
        FreeDirectPlay();
        return 0;
    }

    hRet = g_pDP->EnumConnections(NULL, EnumConnections, 0, DPCONNECTION_DIRECTPLAY);
    if (hRet) {
        dprintf("DirectPlay startup error!\nFailed to enumerate DirectPlay connections.  (hRet = 0x%x)\n", hRet);
        FreeDirectPlay();
        return 0;
    }

    // Get ANSI DirectPlayLobby interface
    DP = &DirectPlayProt;

    //dprintf("DirectPlay interface created successfully.\n");
    _RPT1(_CRT_WARN, "StartDirectPlay -- DirectPlay interface (0x%x) created successfully.\n", (DWORD) g_pDP);
    return !(hRet);
}

/* --------------------------------------------------------------------------- 
 -------------------------------------------------------------------------
 DP_socket method definitions
 ------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::SendAckPacket(int PacketID)
{
    if (!Remote_ID)
        return;
    DPPACKET PacketAck;
    PacketAck.DataSize = 0;
    PacketAck.MessageNum = -1;
    PacketAck.Port = SocketPort;
    PacketAck.MessageType = PACKET_ACK;
    PacketAck.Reserved = PacketID;
    HRESULT hRet;
    hRet = g_pDP->Send(LocalPlayer, Remote_ID, 0, &PacketAck, PacketSize(&PacketAck));
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
//void DP_socket::AddConnection(DPPACKET* Packet, DWORD Size)
void DP_socket::AddConnection(DP_address *addr, DPPACKET* Packet, DWORD Size)
{
    _RPT3(_CRT_WARN, "DP_socket::AddConnection -- Adding connection from player 0x%x to player 0x%x on port %d\n", LocalPlayer, addr->PlayerID, Packet->Reserved);
    g_pDP->DestroyPlayer(Packet->Reserved);
    DP_socket *NewSock;
    NewSock = new DP_socket(Packet->Reserved, addr);
    NewSock->NextConnect = ConnectRequests;
    ConnectRequests = NewSock;
    NewSock->SendConnectAck();
    NewSock->InQueue.Serial = 1;
    NewSock->HandlePackets(Packet, Size, addr);
    NewSock->Send();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::SendConnectAck()
{
    DPPACKET ConnectAck;
    SendRetries = 0;
    ConnectAck.DataSize = 0;
    ConnectAck.MessageNum = -1;
    ConnectAck.Port = SocketPort;
    ConnectAck.MessageType = PACKET_CONNECT_ACK;
    ConnectAck.Reserved = LocalPlayer;
    OutQueue.AddPacketToQueue(&ConnectAck);
    _RPT3(_CRT_WARN, "DP_socket::SendConnectAck -- Sending connect acknowledgement to player 0x%x from player 0x%x on port %d\n", Remote_ID, LocalPlayer, SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::SendConnectRequest(DP_address *addr)
{
    DPPACKET ConnectRequest;
    SendRetries = 0;
    ConnectRequest.DataSize = 0;
    if (addr)
        ConnectRequest.Port = addr->Port;
    else
        ConnectRequest.Port = SocketPort;
    ConnectRequest.MessageType = PACKET_CONNECT;
    ConnectRequest.Reserved = SocketPort;
    OutQueue.AddPacketToQueue(&ConnectRequest);
    _RPT4(_CRT_WARN, "DP_socket::SendConnectRequest -- Attempting to connect to player 0x%x from player 0x%x on port %d with port %d\n", Remote_ID, LocalPlayer, SocketPort, ConnectRequest.Port);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::Send() {
    if (ErrCode)
        return;
    time_marker Now;
    if (!OutQueue.DataSize() || (SendRetries && Now.diff_time(&LastSendTime) < DPRESEND_TIMEOUT) || Remote_ID == DPID_ALLPLAYERS)
        return;
    SendRetries++;
    LastSendTime.get_time();
    HRESULT hRet;
    hRet = g_pDP->Send(LocalPlayer, Remote_ID, 0, OutQueue.Data, OutQueue.DataSize());
    if (hRet != DP_OK) {
        switch (hRet) {
        case DPERR_CONNECTIONLOST:
        case DPERR_INVALIDPLAYER:
        case DPERR_INVALIDPARAMS: // Apparently this is what's returned when a player connection dies...
            ErrCode = 1;
            break;
        }
        _RPT3(_CRT_WARN, "DP_socket::Send -- Error sending packet to player 0x%x on port %d.  (hRet = 0x%x)\n", Remote_ID, SocketPort, hRet);
    }
    else
        _RPT4(_CRT_WARN, "DP_socket::Send -- Sent %d bytes from player 0x%x to player 0x%x on port %d\n", OutQueue.DataSize(), LocalPlayer, Remote_ID, SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_socket::write(void* buf, int size, net_address *addr)
{
    SendRetries = 0;
    DPPACKET Packet;
    Packet.DataSize = size;
    Packet.MessageType = PACKET_DATA;
    Packet.Reserved = LocalPlayer;
    memcpy(Packet.Data, buf, size);
    if (!addr) {
        Packet.Port = SocketPort;
        _RPT4(_CRT_WARN, "DP_socket::write -- Writing %d bytes to player 0x%x on port %d in packet %d\n", size, Remote_ID, Packet.Port, OutQueue.GetNum() + 1);
    }
    else {
        Packet.Port = ((DP_address*) addr)->Port;
        _RPT4(_CRT_WARN, "DP_socket::write (addr override) -- Writing %d bytes to player 0x%x on port %d in packet%d\n", size, ((DP_address*) addr)->PlayerID, Packet.Port, OutQueue.GetNum() + 1);
    }
    OutQueue.AddPacketToQueue(&Packet);
    Send();
    if (!ErrCode)
        return size;
    else
        return -1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_socket::read(void *buf, int size, net_address **addr)
{
    if (ErrCode)
        return -1;
    DWORD BytesRead = 0;
    time_marker Start, Now;
    do {
        while (!ready_to_read() && Now.diff_time(&Start) < DPREAD_TIMEOUT) {
            DP->select(0);
        }
        BytesRead += read_backup(buf, size);
     } while (size && Now.diff_time(&Start) < DPREAD_TIMEOUT);
    _RPT3(_CRT_WARN, "DP_socket::read -- Read %d bytes from player 0x%x on port %d\n", BytesRead, Remote_ID, SocketPort);
    if (addr) {
        DP_address *NewAddr = new DP_address();
        NewAddr->PlayerID = Last_Received_ID;
        NewAddr->Port = SocketPort;
        *addr = NewAddr;
    }
    return BytesRead;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_socket::~DP_socket()
{
    _RPT3(_CRT_WARN, "DP_socket::~DP_socket --  Deleting socket from player 0x%x to player 0x%x on port %d\n", LocalPlayer, Remote_ID, SocketPort);
    if (backup)
        jfree(backup);
    backup = NULL;
    DP->remove_socket_from_list(this);
    if (OutQueue.Count())
        DP_Deleted_Socket *DelSock = new DP_Deleted_Socket(this);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_socket::DP_socket(int port, DP_address *dest)
{  
    SendRetries = 0;
    Connecting = 0;
    ConnectRequests = NULL;
    backup = NULL;
    backup_size = backup_end = backup_start = 0;
    ErrCode = 0;
    
    status = INIT;
    DP->add_socket_to_list(this);
    if (port == -1) {
        HRESULT hRet;
        DPNAME dpname;
        ZeroMemory(&dpname, sizeof(DPNAME));
        dpname.dwSize = sizeof(DPNAME);
        dpname.lpszShortNameA = "";
        hRet = g_pDP->CreatePlayer((DPID*) &SocketPort, &dpname, NULL, NULL, 0, 0);
        if (hRet != DP_OK) {
            _RPT1(_CRT_ASSERT, "DP_socket::DP_socket -- error creating player for port ID!  (hRet = 0x%x)\n", hRet);
        }
    }
    else
        SocketPort = port;

    if (dest)
        Remote_ID = dest->PlayerID;
    else
        Remote_ID = DPID_ALLPLAYERS;
	_RPT2(_CRT_WARN, "DP_socket::DP_socket -- Creating new socket from player 0x%x on port %d\n", LocalPlayer, SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::HandlePackets(DPPACKET* Packet, DWORD Size, DP_address *Dest)
{
    InQueue.AddPackets(Packet, Size);
    while (InQueue.Count()) {
        ProcessPacket();
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::send_ping_packet()
{
    //send_data(0);   // send a 0 length data packet to remote host
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::clear()
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_socket::ready_to_write() {
    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_socket::ProcessPacket()
{
    DPPACKET Packet;
    InQueue.Read(&Packet);
    switch (Packet.MessageType) {
        case PACKET_ACK:
            OutQueue.Acknowledge(Packet.Reserved);
            _RPT1(_CRT_WARN, "DP_socket::ProcessPacket -- Got acknowledgement for packet #%d\n", Packet.Reserved);
            return 0;
        case PACKET_CONNECT:
            // Pitch it, we're getting a dupe connect from this player
            SendAckPacket(Packet.MessageNum);
            Send();
            break;
        case PACKET_CONNECT_ACK:
            Connecting = 0;
            _RPT1(_CRT_WARN, "DP_socket::ProcessPacket -- Got connect acknowledgement from 0x%x\n", Packet.Reserved);
            SendAckPacket(Packet.MessageNum);
            break;
        case PACKET_DATA:
            SendAckPacket(Packet.MessageNum);
            add_backup((unsigned char*) &Packet.Data, Packet.DataSize);
            return Packet.DataSize;
            break;
        default:
            break;
    }
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_socket::ready_to_read()     
{ 
    if (backup_end - backup_start > 0)
        return backup_end - backup_start;
    Send();
    DP->Read();
    while (InQueue.Count())
        ProcessPacket();
    if (backup_end - backup_start > 0)
        return backup_end - backup_start;
    if (ConnectRequests)
        return 1;
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_socket::read_backup(void *&buf, int &size)    // read previously buffered data
{ 
    int s = backup_end - backup_start;
    if (s && size) {
        if (size >= s) {
            memcpy(buf, backup + backup_start, s);
            buf = (void*) ((uchar*) buf + s);
            backup_start = backup_end = 0;
            size -= s;
            return s;
        }
        else {
            memcpy(buf, backup + backup_start, size);
            buf = (void*) ((uchar*) + size);
            int ret = size;
            backup_start += size;
            size = 0;
            return ret;
        }
    } else return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_socket::add_backup(uchar *buf, int size)
{
    if (size) {
        if (backup_size - backup_end >= size) {
            memcpy(backup + backup_end, buf, size);
            backup_end += size;
        }
        else {
            backup_size += backup_end + size - backup_size;
            backup = (uchar*) jrealloc(backup, backup_size, "backup buffer");
            add_backup(buf, size);
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_address *DP_socket::GetAddress()
{
    DP_address *NewAddr = new DP_address;
    NewAddr->PlayerID = Remote_ID;
    NewAddr->Port = SocketPort;
    NewAddr->SessionGUID = GUID_NULL;
    return NewAddr;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
net_socket *DP_socket::accept(net_address *&from)
{
    if (ConnectRequests) {
        DP_socket *last = NULL, *ret = ConnectRequests;
        while (ret->NextConnect) {
            last = ret;
            ret = ret->NextConnect;
        }
        from = ret->GetAddress();
        if (last)
            last->NextConnect = NULL;
        else
            ConnectRequests = NULL;
        return ret;
    }
    else
        return NULL;
}
/* --------------------------------------------------------------------------- 
 -------------------------------------------------------------------------
 DP_Deleted_Socket method definitions
 ------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_Deleted_Socket::DP_Deleted_Socket(DP_socket *Sock) : DP_socket(0, NULL)
{
    DP->remove_socket_from_list(this);
    OutQueue.Copy(&Sock->OutQueue);
    Remote_ID = Sock->Remote_ID;
    SocketPort = Sock->SocketPort;
    DP->add_deleted_socket(this);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_Deleted_Socket::~DP_Deleted_Socket()
{
    DP->remove_deleted_socket(this);
}

/* --------------------------------------------------------------------------- 
 -------------------------------------------------------------------------
 DP_protocol method definitions
 -------------------------------------------------------------------------
 ------------------------------------------------------------------------- /**/
net_socket *DP_protocol::connect_to_server(net_address *addr, 
						net_socket::socket_type sock_type)
{
    _RPT0(_CRT_WARN, "DP_protocol::connect_to_server -- Connecting to server\n");
    if (DP->debug_level(DB_MAJOR_EVENT)) {
        fprintf(stderr, "(connect to server ");
        addr->print();
        fprintf(stderr, ")");
    }
    DP_socket *NewSock;
    if (JoinSession((DP_address*) addr)) {
        NewSock = new DP_socket(-1, (DP_address*) addr);    // allocate a dynamic port
        NewSock->Connect((DP_address*) addr);
        return NewSock;
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
net_address *DP_protocol::get_node_address(char *&server_name, int def_port, int force_port)
{
    DP_address *ret = new DP_address((unsigned char*) server_name, def_port);
    return ret;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
net_socket *DP_protocol::create_listen_socket(int port, net_socket::socket_type sock_type, char *sock_name)
{
    _RPT1(_CRT_WARN, "DP_protocol::create_listen_socket -- Creating listen socket on port %d\n", port);
    if (DP->debug_level(DB_MAJOR_EVENT))
        fprintf(stderr, "opening listening socket : port = %d, sock_type =%d\n", port, sock_type);
    
    DP_socket *ret = NULL;    
    if (sock_type == net_socket::SOCKET_FAST) {
        DP_Fast_Socket *NewSock = new DP_Fast_Socket(port);
        ret = NewSock;
    }
    else {
        DP_socket *NewSock = new DP_socket(port);

        ret = NewSock;
    }
    return ret;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_protocol::DP_protocol()
{
    list = NULL;
    Connected_To_Session = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::add_socket_to_list(DP_socket *who)
{
    who->next = list;
    list = who;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
net_socket* DP_protocol::start_notify(int port, void *data, int len)
{
    StartNewGame((char*) data);
    return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::remove_socket_from_list(DP_socket *who)
{
    if (who == list)
        list = list->next;
    else 
        if (list) {
            DP_socket *last = list;
            while (last->next && last->next != who)
                last = last->next;
            if (last->next)
                last->next = who->next;
        }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::add_deleted_socket(DP_socket *who)
{
    who->next = Deleted_List;
    Deleted_List = who;
    _RPT3(_CRT_WARN, "DP_protocol::add_deleted_socket -- Adding deleted socket from 0x%x to 0x%x on port %d\n", LocalPlayer, who->Remote_ID, who->SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::remove_deleted_socket(DP_socket *who)
{
    if (who == Deleted_List)
        Deleted_List = Deleted_List->next;
    else 
        if (Deleted_List) {
            DP_socket *last = Deleted_List;
            while (last->next && last->next != who)
                last = last->next;
            if (last->next)
                last->next = who->next;
        }
    _RPT3(_CRT_WARN, "DP_protocol::remove_deleted_socket -- Removing deleted socket from 0x%x to 0x%x on port %d\n", LocalPlayer, who->Remote_ID, who->SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_protocol::select(int block)
{
    int t = 0;
    do {
        DP_socket *p;
        for (p = list; p; p = p->next) {
            if ((p->status & DP_socket::READ_CHECKING) && (p->ready_to_read()))
                t++;
            if ((p->status & DP_socket::WRITE_CHECKING) && (p->ready_to_write()))
                t++;
        }
        for (p = Deleted_List; p; p = p->next) {
            p->Send();
        }
    } while (t == 0 && block);
    return t;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::cleanup()
{
    ServerPlayer = 0;
    AmServer = 0;

    reset_find_list();
    DP_socket *first = list;
    list = NULL;
    while (first) {
        DP_socket *q = first;
        first = first->next;
        delete q;
    }
    while (Deleted_List) {
        DP_socket *q = Deleted_List;
        Deleted_List = Deleted_List->next;
        delete q;
    }
    if (!(main_net_cfg && main_net_cfg->restart_state()))
        FreeDirectPlay();                 // Free everything; engine is quitting
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::clear_sockets(DP_socket *exclude)
{
    DP_socket *p = list;
    for (; p; p = p->next)
        if (p != exclude)
            p->clear();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_protocol::~DP_protocol()
{
    cleanup();
    FreeDirectPlay();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
net_address *DP_protocol::find_address(int port, char *name) 
{ 
    HRESULT hRet;
    DPSESSIONDESC2 dpsd;
    ZeroMemory(&dpsd, sizeof(dpsd));
    dpsd.dwSize = sizeof(dpsd);
    dpsd.guidApplication = ABUSE_GUID;
    hRet = g_pDP->EnumSessions(&dpsd, 0, EnumSessions, NULL, DPENUMSESSIONS_ASYNC | DPENUMSESSIONS_AVAILABLE);
    int i;
    for (i = 0; i < SessionCount; i++) {
        if (!AbuseSessions[i]->found) {
            AbuseSessions[i]->found = 1;
            strcpy(name, AbuseSessions[i]->Name);
            DP_address *addr = new DP_address;
            addr->PlayerID = 1; // Assume that the server player will be ID 1
            addr->SessionGUID = AbuseSessions[i]->SessionGUID;
            return addr;
        }
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::reset_find_list()
{
    int i;
    for (i = 0; i < SessionCount; i++) {
        delete AbuseSessions[i]->Name;
        delete AbuseSessions[i];
    }
    delete AbuseSessions;
    SessionCount = 0;
    AbuseSessions = NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_protocol::installed() 
{ 
    if (g_pDP)
        return 1;
    else
        return StartDirectPlay();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
char* DP_protocol::GetSessionName(int id)
{
    return DPProtocolList[id]->Name.lpszLongNameA;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_protocol::GetNumSessions()
{
    return ProtocolCount;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_protocol::StartNewGame(char* name)
{
    HRESULT hRet;

    VOID*          pAddress      = NULL;
    DWORD          dwAddressSize = 0;
    
    DPSESSIONDESC2 dpsd;
    ZeroMemory(&dpsd, sizeof(dpsd));
    dpsd.dwSize = sizeof(dpsd);
    
    dpsd.lpszSessionNameA = name;
    dpsd.dwMaxPlayers = 1024;
    // The DPSESSION_KEEPALIVE flag keeps the session alive 
    //  if players abnormally quit.
    dpsd.dwFlags = DPSESSION_KEEPALIVE | DPSESSION_MIGRATEHOST | DPSESSION_DIRECTPLAYPROTOCOL;
    dpsd.guidApplication = ABUSE_GUID;
    EnumeratingSessions = 0;
    hRet = g_pDP->Open(&dpsd, DPOPEN_CREATE);
    DPNAME dpname;
    ZeroMemory(&dpname, sizeof(DPNAME));
    dpname.dwSize = sizeof(DPNAME);
    dpname.lpszShortNameA = "";
    hRet = g_pDP->CreatePlayer(&ServerPlayer, &dpname, NULL, NULL, 0, DPPLAYER_SERVERPLAYER);
    if (hRet != DP_OK) {
        dprintf("DirectPlay Error!  Couldn't create server player!  (hRet = 0x%x)\n", hRet);
        _RPT1(_CRT_ASSERT, "DP_protocol::StartNewGame -- ERROR!  Couldn't create server player!  (hRet = 0x%x)\n", hRet);
        return;
    }
    DPConnected = 1;
    LocalPlayer = ServerPlayer;
    AmServer = 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_protocol::JoinSession(DP_address *addr)
{
    HRESULT hRet;
    DPSESSIONDESC2 dpsd;
    AmServer = 0;
    if (!Connected_To_Session) {
        dprintf("Connecting...\n");
        _RPT0(_CRT_WARN, "DP_protocol::JoinSession -- Connecting to session...\n");
        ZeroMemory(&dpsd, sizeof(dpsd));
        dpsd.dwSize = sizeof(dpsd);
        if (addr) {
            dpsd.guidInstance = addr->SessionGUID;
            dpsd.dwFlags = DPSESSION_DIRECTPLAYPROTOCOL | DPSESSION_KEEPALIVE | DPSESSION_KEEPALIVE;
            hRet = DPERR_CONNECTING;
            while (hRet == DPERR_CONNECTING)
                hRet = g_pDP->Open(&dpsd, DPOPEN_JOIN | DPOPEN_RETURNSTATUS);
        }
        if (hRet == DP_OK) {
            EnumeratingSessions = 0;
            hRet = g_pDP->EnumPlayers(NULL, EnumPlayers, NULL, DPENUMPLAYERS_SERVERPLAYER);
            if (hRet == DP_OK) {
                Connected_To_Session = 1;
                CurrentSession = addr->SessionGUID;
                DP_address *ret = new DP_address;
                DPNAME dpname;
                ZeroMemory(&dpname, sizeof(DPNAME));
                dpname.dwSize = sizeof(DPNAME);
                dpname.lpszShortNameA = "";
                hRet = g_pDP->CreatePlayer(&LocalPlayer, &dpname, NULL, NULL, 0, 0);
                if (hRet == DP_OK) {
                    _RPT1(_CRT_WARN, "DP_protocol::JoinSession -- Connected to session as player 0x%x\n", LocalPlayer);
                    return 1;
                }
                else
                    _RPT1(_CRT_ASSERT, "DP_protocol::JoinSession -- ERROR!  Couldn't create local player! (hRet = 0x%x)\n", hRet);
            }
            else
                g_pDP->Close();
        }
    }
    else
        return 1;
    return 0;
}

/* --------------------------------------------------------------------------- 
Finds the socket that's listen on port 'port'
 ------------------------------------------------------------------------- /**/
DP_socket *DP_protocol::FindSocket(int port)
{
    DP_socket* current = list;
    while (current) {
        if (current->SocketPort == port)
            return current;
        current = current->next;
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
Finds the socket that's listening to port in *addr
 ------------------------------------------------------------------------- /**/
DP_socket *DP_protocol::FindSocket(DP_address *addr)
{
    DP_socket *current = list;
    while (current) {
        if (current->SocketPort == addr->Port)
            return current;
        current = current->next;
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
Finds the deleted socket corresponding to *addr
 ------------------------------------------------------------------------- /**/
DP_socket *DP_protocol::FindDeletedSocket(DP_address *addr)
{
    DP_socket *current = Deleted_List;
    while (current) {
        if (current->SocketPort == addr->Port)
            return current;
        current = current->next;
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
// Takes care of packets that don't have a destination port 
 ------------------------------------------------------------------------- /**/
void DP_protocol::HandlePackets(DPPACKET* Packet, DWORD Size, DP_address *Dest)
{
    DPPACKET *Current = Packet;
    while (Size) {
        switch (Current->MessageType) {
            case PACKET_CONNECT_ACK:
            case PACKET_DATA:
                _RPT2(_CRT_WARN, "DP_protocol::HandlePackets -- Cleaning up packets from player 0x%x on port %d\n", Dest->PlayerID, Dest->Port);

                if (Dest->PlayerID) {
                    DPPACKET PacketAck;
                    PacketAck.DataSize = 0;
                    PacketAck.MessageNum = -1;
                    PacketAck.Port = Dest->Port;
                    PacketAck.MessageType = PACKET_ACK;
                    PacketAck.Reserved = Current->MessageNum;
                    HRESULT hRet;
                    hRet = g_pDP->Send(LocalPlayer, Dest->PlayerID, 0, &PacketAck, PacketSize(&PacketAck));
#ifdef _DEBUG
                    if (hRet != DP_OK)
                        _RPT0(_CRT_WARN, "DP_protocol::HandlePackets -- Unable to send cleanup ack packet\n");
#endif
                }
                break;
        }
        Size -= PacketSize(Current);
        Current = NextPacket(Current);
    }
}

/* --------------------------------------------------------------------------- 
Reads incoming packets and dispatches them to their destination socket
 ------------------------------------------------------------------------- /**/
void DP_protocol::Read()
{
    DPID Source_ID;
    HRESULT hRet = DP_OK;
    while (hRet == DP_OK) {
        DWORD DataSize = OVERSIZE_PACKET_SIZE + PACKET_HEADER_SIZE;
        DPPACKET *Pack = (DPPACKET*) &BigPacket;
        hRet = g_pDP->Receive(&Source_ID, &LocalPlayer, DPRECEIVE_TOPLAYER, Pack, &DataSize);
        if (hRet == DPERR_BUFFERTOOSMALL) {
            Pack = (DPPACKET*) malloc(DataSize);
            hRet = g_pDP->Receive(&Source_ID, &LocalPlayer, DPRECEIVE_TOPLAYER, Pack, &DataSize);
        }
        if (hRet == DP_OK) {
            if (!(Source_ID == LocalPlayer || Source_ID == 0 || DataSize < PACKET_HEADER_SIZE)) {
                DP_address addr;
                addr.Port = Pack->Port;
                addr.PlayerID = Source_ID;
                DP_socket *DestSock = FindSocket(&addr);
                switch (Pack->MessageType) {
                    case PACKET_CONNECT:
                        if (!FindSocket(Pack->Reserved) && DestSock) {
                            DestSock->AddConnection(&addr, Pack, DataSize);
                            break;
                        }
                        addr.Port = Pack->Reserved;
                        DestSock = FindSocket(&addr);
                    case PACKET_DATA:
                    case PACKET_ACK:
                    case PACKET_CONNECT_ACK:
                        if (DestSock)
                            DestSock->HandlePackets(Pack, DataSize, &addr);
                        else {
                            DestSock = FindDeletedSocket(&addr);
                            if (DestSock) {
                                DestSock->HandlePackets(Pack, DataSize, &addr);
                                if (!DestSock->OutQueue.Count())
                                    delete DestSock;
                            }
                            else
                                HandlePackets(Pack, DataSize, &addr);
                        }
                        break;
                }
            }
        }
        if (Pack != (DPPACKET*) &BigPacket)
            delete Pack;
    }
}
 
/* --------------------------------------------------------------------------- 
Returns the local (unique) address for this player
 ------------------------------------------------------------------------- /**/
net_address *DP_protocol::get_local_address()
{
    DP_address *addr = new DP_address;
    addr->PlayerID = LocalPlayer;
    addr->SessionGUID = CurrentSession;
    return addr;
}

/* --------------------------------------------------------------------------- 
Shows configuration for DirectPlay protocol
 ------------------------------------------------------------------------- /**/
int DP_protocol::SelectProvider()
{
    image *ns = cash.img(cash.reg("art/frame.spe","net_screen", SPEC_IMAGE, 1));
    int ns_w = ns->width(), ns_h = ns->height();
    int x = (xres + 1) / 2 - ns_w / 2,
        y = (yres + 1) / 2 - ns_h / 2;
    ns->put_image(screen, x, y);
    JCFont *fnt = wm->font();
    image *cancel_image = cash.img(cash.reg("art/frame.spe","cancel", SPEC_IMAGE, 1))->copy();

    ifield *list = NULL;
    char *nw_s;
    for (int i = 0; i < ProtocolCount; i++) {
        nw_s = DPProtocolList[i]->Name.lpszShortNameA;
        list = center_ifield(new button(x + 80 - 17, y + 30, i, nw_s, list), x, x + ns_w, list);
    }
    list = new button(x + 80 + 17, y + ns_h - 20 - fnt->height(), NET_CANCEL, cancel_image, list); 

    nw_s = "Select Protocol";
    wm->font()->put_string(screen,x+ns_w/2-strlen(nw_s)*fnt->width()/2,y+21/2-fnt->height()/2, nw_s,wm->medium_color());

    int ret = 0;
    {
        input_manager inm(screen, list);
        inm.allow_no_selections();
        inm.clear_current();
   
        int done = 0;
        event ev;
        do {
            wm->flush_screen();
            do {
                wm->get_event(ev);
            } while (ev.type == EV_MOUSE_MOVE && wm->event_waiting()); 
            inm.handle_event(ev, NULL);
            if (ev.type == EV_MESSAGE) {
                switch (ev.message.id) {
	                case NET_CANCEL:
                        done = 1;	
                    default:
                        if (ev.message.id >= 0 && ev.message.id < ProtocolCount) {
                            ret = 1;
                            done = 1;
                            g_SelectedProtocol = ev.message.id;
                        }
                }
            }
            if (ev.type == EV_KEY && ev.key == JK_ESC)
                done = 1;
        } while (!done);
    }
    delete cancel_image;
    return ret;
}

/* --------------------------------------------------------------------------- 
    // Shows service provider configuration
 ------------------------------------------------------------------------- /**/
int DP_protocol::DoConfig()
{
    if (main_net_cfg && (main_net_cfg->state == net_configuration::CLIENT || main_net_cfg->state == net_configuration::SERVER))
        return 1;
    if (DPConnected) {
        DP->cleanup();
        FreeDirectPlay();
        StartDirectPlay();
    }
    if (!SelectProvider())
        return 0;
    int ret = 0;
    int done = 0;
    int initerr = 0;    
    image *ns = cash.img(cash.reg("art/frame.spe","net_screen", SPEC_IMAGE, 1));
    int ns_w = ns->width(), ns_h = ns->height();
    int x = (xres + 1) / 2 - ns_w / 2,
        y = (yres + 1) / 2 - ns_h / 2;
    JCFont *fnt = wm->font();
    image *ok_image = cash.img(cash.reg("art/frame.spe","dev_ok", SPEC_IMAGE, 1))->copy(),
          *cancel_image = cash.img(cash.reg("art/frame.spe","cancel", SPEC_IMAGE, 1))->copy();

    ifield *list = NULL;
    char *nw_s;
    if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_TCPIP)) {
        nw_s = "DirectPlay TCP/IP Configuration";
        list = center_ifield(new text_field(x, y + 30, NET_ADDR, "Host IP Address", "************************", HostIP, NULL), x, x + ns_w, NULL);
        list = center_ifield(new info_field(x, y + 30, NET_ADDR + NET_PORT, "Leave address blank for LAN games", list), x, x + ns_w, list);
        list = center_ifield(new text_field(x, y + 30, NET_PORT, "Port", "******", sPort, list), x, x + ns_w, list);
    }
    else if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_MODEM)) {
        nw_s = "Modem Configuration";
        initerr = 1;
    }
    else if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_SERIAL)) {
        nw_s = "Serial Link Configuration";
        initerr = 1;
    }
    else if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_IPX)) {
        nw_s = "IPX Configuration";
        if (InitSession())
            return 1;
        initerr = 1;
    }

    ns->put_image(screen, x, y);
    wm->font()->put_string(screen,x+ns_w/2-strlen(nw_s)*fnt->width()/2,y+21/2-fnt->height()/2, nw_s,wm->medium_color());
    if (!initerr)
        list = new button(x + 80 - 17, y + ns_h - 20 - fnt->height(), NET_OK, ok_image, list);
    else
        list = center_ifield(new info_field(x, y + 30, NET_ADDR + NET_PORT, "Error initializing Protocol!", NULL), x, x + ns_w, NULL);
    list = new button(x + 80 + 17, y + ns_h - 20 - fnt->height(), NET_CANCEL, cancel_image, list);

    input_manager inm(screen, list);
    inm.allow_no_selections();
    inm.clear_current();

    event ev;
    do {
        wm->flush_screen();
        do {
            wm->get_event(ev);
        } while (ev.type == EV_MOUSE_MOVE && wm->event_waiting()); 
        inm.handle_event(ev, NULL);
        if (ev.type==EV_MESSAGE) {
            switch (ev.message.id) {
                case NET_OK: {
                    if (IsEqualGUID(DPProtocolList[g_SelectedProtocol]->guid, DPSPGUID_TCPIP)) {
                        char *nm = inm.get(NET_ADDR)->read();
                        if (strcmp(HostIP, ""))
                            delete HostIP;
                        HostIP = (char*) malloc(strlen(nm)+1);
                        sPort = atoi(inm.get(NET_PORT)->read());
                        strcpy(HostIP, nm);
                    }
                    ret = 1;
                    done = 1;
                    break;
                    }
	            case NET_CANCEL:
                    done = 1;	    
            }
        }
        if (ev.type == EV_KEY && ev.key == JK_ESC)
            done = 1;
    } while (!done);
    
    // Get service provider address from information in dialog
    if (ret) 
        ret = InitSession();
    delete ok_image;
    delete cancel_image;
    return ret;
}

/* --------------------------------------------------------------------------- 
 -------------------------------------------------------------------------
 DP_Fast_Socket method definitions
 ------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Fast_Socket::read(void *buf, int size, net_address **addr)
{
    DWORD BytesRead = 0;
    time_marker Start, Now;
    if (ready_to_read())
        BytesRead += read_backup(buf, size);
    _RPT2(_CRT_WARN, "DP_Fast_Socket::read -- Read %d bytes from player 0x%x\n", BytesRead, Remote_ID);
    if (addr) {
        DP_address *NewAddr = new DP_address();
        NewAddr->PlayerID = Last_Received_ID;
        NewAddr->Port = SocketPort;
        *addr = NewAddr;
    }
    return BytesRead;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_Fast_Socket::~DP_Fast_Socket()
{
    _RPT3(_CRT_WARN, "Deleting DP_Fast_Socket from player 0x%x to player 0x%x on port %d\n", LocalPlayer, Remote_ID, SocketPort);
    if (backup)
        jfree(backup);
    backup = NULL;
    DP->remove_socket_from_list(this);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_Fast_Socket::DP_Fast_Socket(int port, DP_address *dest) : DP_socket(port, dest)
{  
	_RPT1(_CRT_WARN, "Creating new DP_Fast_Socket on port %d\n", SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_Fast_Socket::send_ping_packet()
{
    //send_data(0);   // send a 0 length data packet to remote host
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_Fast_Socket::clear()
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Fast_Socket::ready_to_write() {
    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_Fast_Socket::HandlePackets(DPPACKET* Packet, DWORD Size, DP_address *Dest)
{
    InQueue.AddPackets(Packet, Size);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Fast_Socket::ProcessPacket()
{
    DPPACKET Packet;
    InQueue.Read(&Packet);
    switch (Packet.MessageType) {
        case PACKET_ACK:
            _RPT1(_CRT_WARN, "DP_Fast_Socket::ProcessPacket -- WARNING! Got acknowledgement for packet #%d\n", Packet.Reserved);
            return 0;
        case PACKET_CONNECT:
            _RPT1(_CRT_WARN, "DP_Fast_Socket::ProcessPacket -- WARNING! Got connect request from player 0x%x\n", Packet.Reserved);
            return 0;
        case PACKET_CONNECT_ACK:
            _RPT1(_CRT_WARN, "DP_Fast_Socket::ProcessPacket -- WARNING! Got connect ack from player 0x%x\n", Packet.Reserved);
            return 0;
        case PACKET_DATA:
            Last_Received_ID = Packet.Reserved;
            add_backup((unsigned char*) &Packet.Data, Packet.DataSize);
//            _RPT3(_CRT_WARN, "DP_Fast_Socket::ProcessPacket -- Read %d bytes from 0x%x on port %d\n", Packet.DataSize, Last_Received_ID, SocketPort);
            return Packet.DataSize;
            break;
        default:
            break;
    }
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Fast_Socket::ready_to_read()     
{ 
    if (backup_end - backup_start > 0) {
        return backup_end - backup_start;
    }
    DP->Read();
    if (InQueue.Count())
        ProcessPacket();

    if (backup_end - backup_start > 0) {
        return backup_end - backup_start;
    }
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_address *DP_Fast_Socket::GetAddress()
{
    DP_address *NewAddr = new DP_address;
    NewAddr->PlayerID = Remote_ID;
    NewAddr->Port = SocketPort;
    NewAddr->SessionGUID = GUID_NULL;
    return NewAddr;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Fast_Socket::write(void* buf, int size, net_address *addr)
{
    DP_address *DPaddr = (DP_address*) addr;
    DPPACKET Packet;
    Packet.DataSize = size;
    Packet.MessageType = PACKET_DATA;
    Packet.Port = SocketPort;
    Packet.Reserved = LocalPlayer;
    memcpy(Packet.Data, buf, size);
    HRESULT hRet;
    if (!addr) {
        hRet = g_pDP->Send(LocalPlayer, Remote_ID, 0, &Packet, PacketSize(&Packet));
        _RPT3(_CRT_WARN, "DP_Fast_Socket::write (fast) -- Sent %d bytes to player 0x%x on port %d\n", size + PACKET_HEADER_SIZE, Remote_ID, SocketPort);
    }
    else {
        hRet = g_pDP->Send(LocalPlayer, DPaddr->PlayerID, 0, &Packet, PacketSize(&Packet));
        _RPT3(_CRT_WARN, "DP_Fast_Socket::write (fast addr) -- Sent %d bytes to player 0x%x on port %d\n", size + PACKET_HEADER_SIZE, DPaddr->PlayerID, SocketPort);
    }
    if (hRet == DP_OK)
        return size;
    else
        return -1;
}

/* --------------------------------------------------------------------------- 
 -------------------------------------------------------------------------
 DP_Base_Socket method definitions
 ------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Base_Socket::write(void* buf, int size, net_address *addr)
{
    _RPT4(_CRT_WARN, "DP_Base_Socket::write (fast) -- !!TRIED TO WRITE on DP_Base_Socket!!  %d bytes from player 0x%x to player 0x%x on port %d\n", size + PACKET_HEADER_SIZE, LocalPlayer, Remote_ID, SocketPort);
    _ASSERT(!"DP_Base_Socket::write should never be called");
    return -1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Base_Socket::read(void *buf, int size, net_address **addr)
{
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_Base_Socket::~DP_Base_Socket()
{
    _RPT3(_CRT_WARN, "Deleting socket from player 0x%x to player 0x%x on port %d\n", LocalPlayer, Remote_ID, SocketPort);
    if (backup)
        jfree(backup);
//    DP->remove_socket_from_list(this);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_Base_Socket::DP_Base_Socket(int port, DP_address *dest)
{  
    backup = NULL;
    backup_size = backup_end = backup_start = 0;

    status = INIT;

//    DP->add_socket_to_list(this);
    // setup an outgoing packet structure
    if (dest) {
        if (port != -1)
            SocketPort = port;
        else
            SocketPort = dest->Port;
        Remote_ID = dest->PlayerID;
    }
    else {
        SocketPort = port;
        Remote_ID = DPID_ALLPLAYERS;
    }
	_RPT1(_CRT_WARN, "Creating new DP_Base_Socket on port %d\n", SocketPort);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_Base_Socket::send_ping_packet()
{
    //send_data(0);   // send a 0 length data packet to remote host
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_Base_Socket::clear()
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Base_Socket::ready_to_write() {
    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Base_Socket::ProcessPacket()
{
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Base_Socket::ready_to_read()     
{ 
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int DP_Base_Socket::read_backup(void *&buf, int &size)    // read previously buffered data
{ 
    int s = backup_end - backup_start;
    if (s && size) {
        if (size >= s) {
            memcpy(buf, backup + backup_start, s);
            buf = (void*) ((uchar*) buf + s);
            backup_start = backup_end = 0;
            size -= s;
            return s;
        }
        else {
            memcpy(buf, backup + backup_start, size);
            buf = (void*) ((uchar*) + size);
            int ret = size;
            backup_start += size;
            size = 0;
            return ret;
        }
    } else return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DP_Base_Socket::add_backup(uchar *buf, int size)
{
    if (size) {
        if (backup_size - backup_end >= size) {
            memcpy(backup + backup_end, buf, size);
            backup_end += size;
        }
        else {
            backup_size += backup_end + size - backup_size;
            backup = (uchar*) jrealloc(backup, backup_size, "backup buffer");
            add_backup(buf, size);
        }
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DP_address *DP_Base_Socket::GetAddress()
{
    DP_address *NewAddr = new DP_address;
    NewAddr->PlayerID = Remote_ID;
    NewAddr->Port = SocketPort;
    NewAddr->SessionGUID = GUID_NULL;
    return NewAddr;
}

/* --------------------------------------------------------------------------- 
 --------------------------------------------------------------------------- 
    CPacketQueue method implementations
 --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CWritePacketQueue::AddPacketToQueue(DPPACKET *NewData)
{
    if (QueueSize + PacketSize(NewData) > BufferSize)
        ReSize(BufferSize + MAX_PACKET_SIZE);
    NewData->MessageNum = MessageNum++;
    memcpy(&((unsigned char*) Data)[QueueSize], NewData, PacketSize(NewData));
    QueueSize += PacketSize(NewData);
    PacketCount++;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int CReadPacketQueue::Read(DPPACKET *OutPacket)
{
    if (PacketCount) {
        memcpy(OutPacket, Data, PacketSize(Data));
        QueueSize -= PacketSize(OutPacket);
        
        memmove(Data, (char*) Data + PacketSize(Data), QueueSize);
        PacketCount--;
        //        Data = (unsigned char*) realloc(Data, PACKET_INITIAL_SIZE);
        return PacketSize(OutPacket);
    }
    else
        return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
DPPACKET* CPacketQueue::operator[] (int Index)
{
    if (Index >= PacketCount)
        return NULL;
    DPPACKET *Current = (DPPACKET*) Data;
    while (Index > 0) {
        Current = (DPPACKET*) ((unsigned char*) Current + PACKET_HEADER_SIZE + Current->DataSize);
        Index--;
    }
    return Current;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CWritePacketQueue::Acknowledge(int Message)
{
    if (Message >= MessageNum)
        return;
    DWORD MoveSize = QueueSize;
    DPPACKET *Current = (DPPACKET*) Data;
    while (MoveSize && (Message > Current->MessageNum)) {
        MoveSize -= PacketSize(Current);
        Current = NextPacket(Current);
    }
    if (MoveSize && (Message == Current->MessageNum)) {
        QueueSize -= PacketSize(Current);
        MoveSize -= PacketSize(Current);
        memmove(Current, NextPacket(Current), MoveSize);
        PacketCount--;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CReadPacketQueue::AddPackets(DPPACKET *NewData, DWORD BufSize)
{
    DPPACKET *Current = NewData;
    while (BufSize) {
        AddPacketToQueue(Current);
        BufSize -= PacketSize(Current);
        Current = NextPacket(Current);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CReadPacketQueue::AddPacketToQueue(DPPACKET *NewData)
{
    if (Serial)
        if ((NewData->MessageType != PACKET_ACK) && NextExpectedMessageNum != -1)
            if (NewData->MessageNum < NextExpectedMessageNum)
                return;
#ifdef _DEBUG
    if (NewData->MessageNum != -1)
        _RPT1(_CRT_WARN, "Adding message number %d to queue\n", NewData->MessageNum);
#endif
    if (QueueSize + PacketSize(NewData) > BufferSize)
        ReSize(BufferSize + MAX_PACKET_SIZE);
    NextExpectedMessageNum = NewData->MessageNum + 1;
    memcpy(&((unsigned char*) Data)[QueueSize], NewData, PacketSize(NewData));
    QueueSize += PacketSize(NewData);
    PacketCount++;
}
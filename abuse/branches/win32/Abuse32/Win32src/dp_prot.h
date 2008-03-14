#ifndef __DPLAY_SOCK_INCLUDED
#define __DPLAY_SOCK_INCLUDED

#include <windows.h>
#include "sock.hpp"
#include "macs.hpp"
#include "timing.hpp"
#include <string.h>
#include "dplay.h"
#include "dplobby.h"

#include <crtdbg.h>

#define PACKET_DATA 0
#define PACKET_ACK 1
#define PACKET_CONNECT 2
#define PACKET_CONNECT_ACK 4

#define MAX_PACKET_SIZE 2048
#define PACKET_HEADER_SIZE 20
#define PACKET_INITIAL_SIZE MAX_PACKET_SIZE - PACKET_HEADER_SIZE

#define OVERSIZE_PACKET_SIZE 65536

#define DPREAD_TIMEOUT 120
#define DPRESEND_TIMEOUT 0.2

struct DPPROTOCOL {
    GUID guid;
    DWORD ConnectionSize;
    void* Connection;
    DPNAME Name;
    DWORD dwFlags;
};

struct DPPACKET {
    int Port;
    int MessageNum;
    int MessageType;
    int Reserved;
    DWORD DataSize;
    char Data[MAX_PACKET_SIZE];
};

struct DP_BIG_PACKET {
    int Port;
    int MessageNum;
    int MessageType;
    int Reserved;
    DWORD DataSize;
    char Data[OVERSIZE_PACKET_SIZE];
};

#define NextPacket(x) (DPPACKET*) ((unsigned char*) x + PACKET_HEADER_SIZE + ((DPPACKET*) x)->DataSize)

#define PacketSize(x) ((DPPACKET*) x)->DataSize + PACKET_HEADER_SIZE

class CPacketQueue {
protected:
    int PacketCount;
    DWORD BufferSize;
    DWORD QueueSize;
public:
    int Serial;
    unsigned char *Data;
    CPacketQueue() {
        Serial = 0;
        QueueSize = 0;
        PacketCount = 0;
        Data = NULL;
        ReSize(MAX_PACKET_SIZE);
    }
    ~CPacketQueue() {
        delete Data;
    }
    __inline void ReSize(DWORD size) {               // Re-size the queue
        BufferSize = size;
        Data = (unsigned char *) realloc(Data, BufferSize);
    }
    __inline DWORD DataSize() {                      // Return the amount of data in the queue
        return QueueSize;
    }
    virtual void AddPacketToQueue(DPPACKET *NewData) { }; // Adds a packet to the queue
    virtual void AddPackets(DPPACKET *NewData, DWORD BufSize) { }; // Adds multiple packets to the queue
    __inline DWORD CurrentPacketSize() {                  // Return the size of the first packet in queue
        return ((DPPACKET*) Data)->DataSize;
    }
    DPPACKET* operator[] (int Index);                     // Return the packet at position 'Index'
    __inline int Count() {                                // Return current number of queued packets
        return PacketCount;
    };
    virtual void Copy(CPacketQueue *Queue) {              // Copy this queue
        PacketCount = Queue->PacketCount;
        ReSize(Queue->BufferSize);
        QueueSize = Queue->QueueSize;
        memcpy(Data, Queue->Data, QueueSize);
        Serial = Queue->Serial;
    };
};

// -------------------------------------------------------------------------
class CReadPacketQueue : public CPacketQueue {
private:
    int NextExpectedMessageNum;
public:
    CReadPacketQueue() : CPacketQueue() {
        NextExpectedMessageNum = -1;
    };
    virtual void AddPacketToQueue(DPPACKET *NewData);
    virtual void AddPackets(DPPACKET *NewData, DWORD BufSize);
    __inline DPPACKET* Peek() {
        return ((DPPACKET*) Data);
    }
    int Read(DPPACKET *OutPacket);
};

// -------------------------------------------------------------------------
class CWritePacketQueue : public CPacketQueue {
private:
    int MessageNum;
public:
    CWritePacketQueue() : CPacketQueue() {
        MessageNum = 0;
    };
    virtual void AddPacketToQueue(DPPACKET *NewData);
    void Acknowledge(int Message);
    __inline int GetNum() {
        return MessageNum;
    }
    virtual void Copy(CWritePacketQueue *Queue) {
        MessageNum = Queue->MessageNum;
        PacketCount = Queue->PacketCount;
        ReSize(Queue->BufferSize);
        QueueSize = Queue->QueueSize;
        memcpy(Data, Queue->Data, QueueSize);
        Serial = Queue->Serial;
    };
};

extern int ProtocolCount;
extern DPPROTOCOL **DPProtocolList;

// -------------------------------------------------------------------------
class DP_address : public net_address
{
public:
    GUID SessionGUID;
    DPID PlayerID;
    int Port;
    virtual protocol protocol_type() const { return DPLAY; }
    virtual int equal(const net_address *who) const {
        return who->protocol_type() == protocol_type() && ((DP_address*) who)->PlayerID == PlayerID;
    }
    virtual int set_port(int Port) {
        this->Port = Port;
        return 1;
    }
    virtual void print() { 
        printf("Player ID: %d  Port:%d\n", PlayerID, Port);
    }
    DP_address() {
        Port = 0;
        PlayerID = DPID_ALLPLAYERS;
        SessionGUID = GUID_NULL;
    }
    DP_address(uchar *addr_data, ushort port) {
        Port = port;
        memcpy(&SessionGUID, &addr_data[strlen((char*) addr_data)+1], sizeof(SessionGUID)); 
        PlayerID = atoi((char*) addr_data); 
    }
    int get_port() {
        return Port;
    }
    net_address* copy() {
        DP_address *addr = new DP_address;
        addr->PlayerID = PlayerID;
        addr->Port = Port;
        addr->SessionGUID = SessionGUID;
        return (net_address*) addr;
    }
    void store_string(char* st, int st_length) {     // Store a "copy" of the address in a string
        sprintf(st, "%.08x:%.08x", PlayerID, Port);
        memcpy(&st[strlen(st)+1], &SessionGUID, sizeof(SessionGUID));
    }
};
// -------------------------------------------------------------------------
class DP_socket : public net_socket
{
protected:
    DP_socket *ConnectRequests;
    int SendRetries;
    int Connecting;
    time_marker LastSendTime;
public:
    CReadPacketQueue InQueue;
    CWritePacketQueue OutQueue;
    DPID Remote_ID, Last_Received_ID;
    int SocketPort;
    uchar *backup;                   // when a partial read request happens
//    int BlockingReads;
    int backup_start, backup_end, backup_size;
    int read_backup(void *&buf, int &size);
    int ErrCode;
    void add_backup(uchar *buf, int size);
    DP_socket *NextConnect;
    DP_socket *next;

    enum { 
        SEND_RETRY_TIMEOUT = 100,  // .1 seconds timeout before re-sending data packet
	    RETRY_TOTAL = 600          // give up after retrying for 1 minute
    };

    enum { 
        INIT = 0,
        READ_CHECKING = 1,
        WRITE_CHECKING = 2,
        READ_SELECTED = 4,
        WRITE_SELECTED = 8 
    };
    uchar status;

    DP_socket(int port, DP_address *dest = NULL);
    virtual ~DP_socket();

    void send_ping_packet();
    virtual int error() { 
        return ErrCode;
    }
    virtual int ready_to_read();
    virtual int ready_to_write();
    virtual int write(void *buf, int size, net_address *addr = NULL);
    virtual int read(void *buf, int size, net_address **addr = 0);

    virtual int get_fd()    { return Remote_ID; }
    virtual void read_selectable()     { status |= READ_CHECKING; }
    virtual void read_unselectable()   { status &= ~READ_CHECKING; }
    virtual void write_selectable()    { status |= WRITE_CHECKING; }
    virtual void write_unselectable()  { status &= ~WRITE_CHECKING; }
    virtual int listen(int port)       {
        return 0;
    }
    virtual net_socket *accept(net_address *&from);
    virtual int from_port() { return -1; }
    virtual void clear();
    virtual void print() { fprintf(stderr,"DirectPlay net socket");  }
    int ProcessPacket();
    DP_address *GetAddress();
    void SendAckPacket(int PacketID);
    void SendConnectAck();
    void Send();
    void SendConnectRequest(DP_address *addr = NULL);
    void AddConnection(DP_address *addr, DPPACKET* Packet, DWORD Size);
    void Connect(DP_address *addr = NULL) {
        InQueue.Serial = 1;
        Connecting = 1;
        SendConnectRequest(addr);
        Send();
    };
    virtual void HandlePackets(DPPACKET* Packet, DWORD Size, DP_address *Dest);
};

// -------------------------------------------------------------------------
class DP_Base_Socket : public net_socket
{
public:
    CReadPacketQueue InQueue;
    DPID Remote_ID, Last_Received_ID, SocketPort;
    uchar *backup;                   // when a partial read request happens
    int backup_start, backup_end, backup_size;
    int read_backup(void *&buf, int &size);
    void add_backup(uchar *buf, int size);
    DP_socket *next;

    enum { INIT = 0,
           READ_CHECKING = 1,
	       WRITE_CHECKING = 2,
	       READ_SELECTED = 4,
	       WRITE_SELECTED = 8 
    };
    uchar status;

    DP_Base_Socket(int port, DP_address *dest = NULL);
    virtual ~DP_Base_Socket();

    void send_ping_packet();
    virtual int error() { 
        return 0;
    }
    virtual int ready_to_read();
    virtual int ready_to_write();
    virtual int write(void *buf, int size, net_address *addr = NULL);
    virtual int read(void *buf, int size, net_address **addr = 0);

    virtual int get_fd()    { return Remote_ID; }
    virtual void read_selectable()     { status |= READ_CHECKING; }
    virtual void read_unselectable()   { status &= ~READ_CHECKING; }
    virtual void write_selectable()    { status |= WRITE_CHECKING; }
    virtual void write_unselectable()  { status &= ~WRITE_CHECKING; }
    virtual int listen(int port)       {
        return 0;
    }
    virtual int from_port() { return -1; }
    virtual void clear();
    virtual void print() { fprintf(stderr,"DirectPlay net socket");  }
    int ProcessPacket();
    DP_address *GetAddress();
};

// -------------------------------------------------------------------------
class DP_Listen_Socket : public DP_Base_Socket
{
protected:
    DP_socket *ConnectRequests;
    int SendRetries;
    int Connecting;
public:
    CWritePacketQueue OutQueue;
    DPID Remote_ID, Local_ID, Last_Received_ID, Remote_Fast_Socket;
    uchar *backup;                   // when a partial read request happens
    int SocketPort;
    DP_socket *NextConnect;

    enum { 
        SEND_RETRY_TIMEOUT = 100,  // .1 seconds timeout before resending data packet
        RETRY_TOTAL = 600          // give up after retrying for 1 minute
    };

    DP_Listen_Socket(int port, DP_address *dest = NULL);
    virtual ~DP_Listen_Socket();

    void send_ping_packet();
    virtual int ready_to_read();
    virtual int ready_to_write();
    virtual int read(void *buf, int size, net_address **addr = 0);

    virtual net_socket *accept(net_address *&from);
    virtual int from_port() { return -1; }
    virtual void clear();
    virtual void print() { fprintf(stderr,"DirectPlay Listen net socket");  }
    int ProcessPacket();
    DP_address *GetAddress();
    void SendAckPacket(int PacketID);
    void SendConnectAck();
    int Send();
    void SendConnectRequest();
    void AddConnection(DP_address *addr, DPPACKET* Packet, DWORD Size);
    void Connect() {
        InQueue.Serial = 1;
        Connecting = 1;
        SendConnectRequest();
        Send();
    };
};

// -------------------------------------------------------------------------
class DP_Fast_Socket : public DP_socket {
public:
    DP_Fast_Socket(int port = -1, DP_address *dest = NULL);
    virtual ~DP_Fast_Socket();

    void send_ping_packet();
    virtual int ready_to_read();
    virtual int ready_to_write();
    virtual int write(void *buf, int size, net_address *addr = NULL);
    virtual int read(void *buf, int size, net_address **addr = 0);

    virtual int listen(int port)       {
        return 0;
    }
    virtual void clear();
    virtual void print() {
        fprintf(stderr, "DirectPlay Fast net socket");
    }
    int ProcessPacket();
    DP_address *GetAddress();
    virtual void HandlePackets(DPPACKET* Packet, DWORD Size, DP_address *Dest);
};

// -------------------------------------------------------------------------
class DP_Deleted_Socket : public DP_socket {
public:
    DP_Deleted_Socket(DP_socket *Sock);
    virtual ~DP_Deleted_Socket();
    virtual void print() {
        fprintf(stderr, "DirectPlay Deleted net socket");
    }
};

// -------------------------------------------------------------------------
class DP_protocol : public net_protocol {
    int Installed;
    int Connected_To_Session;
    int max_packet_size() { 
        return MAX_PACKET_SIZE;
    }
    DP_socket *list;
    DP_socket *Deleted_List;
    time_marker last_ping;
public:
    enum {
        CONNECT_PING_RETRY  = 100,    // retry connect command every .1 seconds
	    CONNECT_RETRY_TOTAL = 32      // retry for 4 mins.
    };
    enum {
        NET_ADDR   = 4096,
        NET_OK     = 4097,
        NET_CANCEL = 4098,
        NET_PORT   = 4099
    };
    DP_protocol();
    virtual net_address *get_local_address();
    virtual net_address *get_node_address(char *&server_name, int def_port, int force_port);
    virtual net_socket *connect_to_server(net_address *addr, net_socket::socket_type sock_type=net_socket::SOCKET_SECURE);  
    virtual net_socket *create_listen_socket(int port, net_socket::socket_type sock_type, char *sock_name);
    virtual net_socket *create_listen_socket(int port, net_socket::socket_type sock_type) {
        return create_listen_socket(port, sock_type, "Socket");
    }
    int select(int block);

    int installed();                                 // Returns non-zero if DirectPlay is installed

    char *name()    { 
        return "DirectPlay driver";
    }
    void add_socket_to_list(DP_socket *who);         // Adds a socket to the socket pool
    void remove_socket_from_list(DP_socket *who);    // Removes a socket from the socket pool
    void add_deleted_socket(DP_socket *who);         // Adds a socket to the deleted pool
    void remove_deleted_socket(DP_socket *who);      // Removes a socket from the deleted pool
    int max_packet_data_size() {
        return MAX_PACKET_SIZE;
    }
    void clear_sockets(DP_socket *exclude);          //
    void cleanup();                                  // Re-sets protocol
    ~DP_protocol();

    net_address *find_address(int port, char *name); // Finds games to join
    void reset_find_list();                          // Clears list of games
//    int SelectSession(int id);
    char* GetSessionName(int id);                    // Returns the game name for join list[id]
    int GetNumSessions();                            // Returns the number of games found
    virtual int DoConfig();                          // Does DirectPlay-specific configuration
    void StartNewGame(char* name);                   // Starts up a new DirectPlay game
    DP_socket *FindSocket(DPID Player, int port);    // Returns socket matching info specified in parameters
    DP_socket *FindSocket(int port);
    DP_socket *FindSocket(DP_address *addr);
    DP_socket *FindDeletedSocket(DP_address *addr);
    virtual net_socket *start_notify(int port, void *data, int len);
    int SelectProvider();                            // Does DirectPlay service provider configuration
    int JoinSession(DP_address *addr);               // Joins a DirectPlay session
    void Read();                                     // Reads incoming packets from DirectPlay and 
                                                     //   dispatches messages to appropriate sockets
    void HandlePackets(DPPACKET* Packet, DWORD Size, DP_address *Dest); // Takes care of packets that don't have a destination port
};

extern DP_protocol *DP;

int StartDirectPlay();

#endif


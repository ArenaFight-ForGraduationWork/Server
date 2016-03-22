
#define MAX_USER 1000
extern Player players[MAX_USER];
extern RoomManager room_manager;

extern HANDLE IOCP;




void error_display(char *msg, int err_no);

void Init_Player(int id);

void ShutDown_Server();
void Init_Server();
void Send_Packet(void *packet, unsigned id);

void Process_Packet(char *packet, unsigned id);

void Worker_Thread();

int GetNewClientID();
void Accept_Thread();
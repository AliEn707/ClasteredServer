#ifndef SERVER_MESSAGES_HEADER
#define SERVER_MESSAGES_HEADER

#define MSG_S_CLIENT_ATTRIBUTES 1
#define MSG_S_SERVER_CONNECTED 2
#define MSG_S_SERVER_DISCONNECTED 3
#define MSG_S_SERVERS_INFO 4
#define MSG_S_CLIENT_CONNECTED 5
#define MSG_S_CLIENT_DISCONNECTED 6
#define MSG_S_CLIENT_ATTRIBUTES_SET 7
#define MSG_S_CLIENT_ATTRIBUTES_UNSET 8
#define MSG_S_NEW_ID 9
#define MSG_S_SERVER_READY 10

void serverMessageProcessorInit();

#endif

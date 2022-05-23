#include "net.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
float deltaX[PLAYER_LIMIT],deltaY[PLAYER_LIMIT];
float directionX[PLAYER_LIMIT],directionY[PLAYER_LIMIT];
int playerId=0;
int readyCount=0;
int playerCount=1;
std::vector<wchar_t> name[PLAYER_LIMIT];
std::vector<wchar_t> connectIp;
std::mutex m_food,m_bug,m_sendBuffer;
const int buflen = 4096;
const char PORT[]="2715";
SOCKET cSocket[PLAYER_LIMIT];
void iToc(char *s,int num){
	int *tmp=(int*)s;
	*tmp=num;
}
int cToi(char *s){
	int *tmp=(int*)s;
	return *tmp;
}
void fToc(char *s,float num){
	float *tmp=(float*)s;
	*tmp=num;
}
float cTof(char *s){
	float *tmp=(float*)s;
	return *tmp;
}
void lToc(char *s,long long num){
	long long *tmp=(long long*)s;
	*tmp=num;
}
long long cTol(char *s){
	long long *tmp=(long long*)s;
	return *tmp;
}
void receive_thread(int id){
	char recvbuf[buflen+1];
    SOCKET ClientSocket=cSocket[id];
    int iResult;
	iToc(recvbuf,id);
	iResult = send( ClientSocket, recvbuf, 4, 0 );
	if(id==-1){
		closesocket(ClientSocket);
       	WSACleanup();
		return;
	}
    if (iResult == SOCKET_ERROR) {
       	printf("send failed with error: %d\n", WSAGetLastError());
       	closesocket(ClientSocket);
       	WSACleanup();
    }
    int p = 0;
	do {
        iResult = recv(ClientSocket, recvbuf + p, buflen - p, 0);
        if (iResult > 0) {
            int p2 = solveMessage(recvbuf, iResult + p);
            for (int i = 0; i < p2; i++) {
                recvbuf[i] = recvbuf[i + iResult + p - p2];
            }
            p = p2;
        }
        if (iResult == 0)
            printf("Connection closing...\n");
        else if(iResult<0){
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }
    } while (iResult > 0);
}

void setPlayerCount(int num);
bool server(){
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);
	std::thread *(ts[PLAYER_LIMIT]);
	for(int i=1;i<PLAYER_LIMIT;i++){
	    iResult = listen(ListenSocket, SOMAXCONN);
    	if (iResult == SOCKET_ERROR) {
	        printf("listen failed with error: %d\n", WSAGetLastError());
        	closesocket(ListenSocket);
        	WSACleanup();
        	return false;
    	}
    	// Accept a client socket
    	ClientSocket = accept(ListenSocket, NULL, NULL);
    	if (ClientSocket == INVALID_SOCKET) {
        	printf("accept failed with error: %d\n", WSAGetLastError());
        	closesocket(ListenSocket);
        	WSACleanup();
        	return false;
    	}
		cSocket[i]=ClientSocket;
		if(mode==0){
			ts[i]=new std::thread{receive_thread,i};
			setPlayerCount(i+1);
		}else{
			ts[i]=new std::thread{receive_thread,-1};
			i--;
		}
		//printf("PlayerCount %d\n",i+1);
	}

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return false;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
    return true;
}
bool serverPlayer(){
	new std::thread{server};
	return true;
}

SOCKET ConnectSocket = INVALID_SOCKET;
void user_thread(){
    char recvbuf[buflen+1];
    int iResult;
    int p = 0;
	while(true){
		iResult = recv(ConnectSocket, recvbuf+p, buflen-1-p, 0);
		if (iResult > 0) {
			int p2=solveMessage(recvbuf,iResult+p);
            for (int i = 0; i < p2; i++) {
                recvbuf[i] = recvbuf[i + iResult+p - p2];
            }
            p = p2;
		}
    	else if(iResult<0){
    	    printf("recv failed with error: %d\n", WSAGetLastError());
    	    closesocket(ConnectSocket);
    	    WSACleanup();
    	    return;
    	}
	}
}
bool userPlayer(char ip[]){
	 WSADATA wsaData;
     char recvbuf[buflen+1];
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return false;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }
	iResult = recv(ConnectSocket, recvbuf, buflen-1, 0);
    if (iResult > 0) {
		playerId=cToi(recvbuf);
		if(playerId==-1){
			closesocket(ConnectSocket);
        	WSACleanup();
			return false;
		}
    }else{
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }
	new std::thread(user_thread);
    return true;
}



char sendBuffer[buflen+1];
int sendBufferP=0;
void sync(){
	if(sendBufferP==0){
		return;
	}
	std::unique_lock<std::mutex> lck {m_sendBuffer};
    /*iToc(sendBuffer + sendBufferP, 0);
    sendBufferP += 4;*/
	if(playerId){
		send( ConnectSocket, sendBuffer, sendBufferP, 0 );
		sendBufferP=0;
	}else{
		for(int i=1;i<playerCount;i++){
			send(cSocket[i],sendBuffer,sendBufferP,0);
		}
		sendBufferP=0;
	}
}
void addsendBuffer(char buffer[],int l){
    if (sendBufferP + 4 + l > buflen) {
        sync();
    }
	std::unique_lock<std::mutex> lck {m_sendBuffer};
	iToc(sendBuffer+sendBufferP,l+4);
	sendBufferP+=4;
	for(int i=0;i<l;i++){
		sendBuffer[sendBufferP+i]=buffer[i];
	}
	sendBufferP+=l;
	
}
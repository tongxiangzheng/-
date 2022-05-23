#include "def.h"
#include <vector>
#include <map>
#include <mutex>
extern int playerId,readyCount,playerCount;
extern float deltaX[PLAYER_LIMIT],deltaY[PLAYER_LIMIT];
extern float directionX[PLAYER_LIMIT],directionY[PLAYER_LIMIT];
extern int mode,submode;
extern std::vector<wchar_t> name[PLAYER_LIMIT];
extern std::vector<wchar_t> connectIp;
extern int score[PLAYER_LIMIT];
extern std::mutex m_food,m_bug,m_sendBuffer;
void iToc(char *s,int num);
int cToi(char *s);
void fToc(char *s,float num);
float cTof(char *s);
void lToc(char *s,long long num);
long long cTol(char *s);

bool serverPlayer();
bool userPlayer(char ip[]);
int solveMessage(char buffer[],int l);
void addsendBuffer(char buffer[],int l);
void sync();
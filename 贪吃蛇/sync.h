#include"net.h"
#include "Node.h"
extern std::map<long long, Point_AND_Color> food;
extern Node<Point>* bug_begin[PLAYER_LIMIT], * bug_end[PLAYER_LIMIT];
extern std::vector<Color> bug[PLAYER_LIMIT];
extern bool is_dead[PLAYER_LIMIT];
void bug_push_back(int id,Color col);
void bug_inset_node(int id,float x,float y);
void bug_remove_node(int id);
void insert_food(long long foodId,Point_AND_Color data);
void remove_food(long long id);
void setDeltaXY(int id,float x,float y);
void sendXY(double x,double y);
void setPlayerCount(int num);
void setReadyCount(int num);
void setPlayerName(int id,char name[],int l);
void playerReady();
void server_playerXY(int id,float x,float y);
void server_a_player_ready(int id,char s[],int l);
void set_player_score(int id,int score); 
void game_pause();
void player_dead(int id);
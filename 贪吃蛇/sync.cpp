
#include"sync.h"
#include<cmath>
std::map<long long, Point_AND_Color> food;
Node<Point>* bug_begin[PLAYER_LIMIT], * bug_end[PLAYER_LIMIT];
std::vector<Color> bug[PLAYER_LIMIT];
void bug_push_back(int id,Color col){
	bug[id].push_back(col);
	char command[]="sbpb________________";
	iToc(command+4,id);
	fToc(command+8,col.r);
	fToc(command+12,col.g);
	fToc(command+16,col.b);
	addsendBuffer(command,20);
}
void insert_food(long long foodId,Point_AND_Color data){
	char command[]="sifd____________________________";
	lToc(command+4,foodId);
	fToc(command+12,data.x);
	fToc(command+16,data.y);
	fToc(command+20,data.r);
	fToc(command+24,data.g);
	fToc(command+28,data.b);
	addsendBuffer(command,32);
}
void remove_food(long long id){
	char command[]="srfd________";
	lToc(command+4,id);
	addsendBuffer(command,12);
}
void setDeltaXY(int id,float x,float y){
	deltaX[id]=x;
	deltaY[id]=y;
	char command[]="spdl____________";
	iToc(command+4,id);
	fToc(command+8,x);
	fToc(command+12,y);
	addsendBuffer(command,16);
}
void server_playerXY(int id,float x,float y){
	char command[]="spxy____________";
	iToc(command+4,id);
	fToc(command+8,x);
	fToc(command+12,y);
	addsendBuffer(command,16);
}
void bug_inset_node(int id,float x,float y){
	char command[]="sbin____________";
	iToc(command+4,id);
	fToc(command+8,x);
	fToc(command+12,y);
	addsendBuffer(command,16);
}
void bug_remove_node(int id){
	char command[]="sbrm________";
	iToc(command+4,id);
	addsendBuffer(command,8);
}
void sendXY(double x,double y){
	if(x>=32000){
		return;//最小化，不知道如何处理 
	}
	if(is_dead[playerId]){
		return;
	}
	x=x*2-SCR_WIDTH;
	y=-y*2+SCR_HEIGHT;
	double d=sqrt(x*x+y*y);
	x/=d;
	y/=d;
	if(playerId){
		char command[]="pdxy____________";
		iToc(command+4,playerId);
		fToc(command+8,x);
		fToc(command+12,y);
		addsendBuffer(command,16);
	}else{
		directionX[0]=x;
		directionY[0]=y;
		server_playerXY(0,directionX[0],directionY[0]);
	}
}
void game_pause() {
	if (playerId==0) {
		submode ^= 1;
		return;
	}
	char command[] = "pgps";
	addsendBuffer(command,4);
}
void player_dead(int id){
	char command[]="spde____";
	iToc(command+4,id);
	addsendBuffer(command,8);
}
float strlenth(const std::vector<wchar_t> &str);
extern float nameLenth[PLAYER_LIMIT];
void setmode(int m);
void server_setmode(int mode){
	char command[]="ssmd____";
	iToc(command+4,mode);
	addsendBuffer(command,8);
}
void server_a_player_ready(int id,char s[],int l){
	setReadyCount(readyCount+1);
	setPlayerName(id,s,l);
	name[id].clear();
	for(int i=0;i<s[i]; i++) {
		name[id].push_back(s[i]);
	}
	nameLenth[id]=strlenth(name[id]);
	if(readyCount==playerCount){
		setmode(1);
		server_setmode(1);
	}
}
void set_player_score(int id,int score){
	char command[]="sspc________";
	iToc(command+4,id);
	iToc(command+8,score);
	addsendBuffer(command,12);
}
void dead(int id);
void solve(char command[],int l){
	//printf("solve:%d %c%c%c%c %d\n",l,command[0],command[1],command[2],command[3],cToi(command+4));
	if(command[0]=='s'){
		if(command[1]=='p'&&command[2]=='l'&&command[3]=='c'){
			playerCount=cToi(command+4);
		}else if(command[1]=='r'&&command[2]=='e'&&command[3]=='c'){
			readyCount=cToi(command+4);
		}else if(command[1]=='s'&&command[2]=='p'&&command[3]=='n'){
			int id=cToi(command+4);
			name[id].clear();
			for(int i=8;i<command[i]; i++) {
				name[id].push_back(command[i]);
			}
			nameLenth[id]=strlenth(name[id]);
		}else if(command[1]=='s'&&command[2]=='m'&&command[3]=='d'){
			setmode(cToi(command+4));
		}else if(command[1]=='p'&&command[2]=='d'&&command[3]=='l'){
			int id=cToi(command+4);
			deltaX[id]=cTof(command+8);
			deltaY[id]=cTof(command+12);
		}else if(command[1]=='i'&&command[2]=='f'&&command[3]=='d'){
			long long id=cTol(command+4);
			Point_AND_Color d{cTof(command+12),cTof(command+16),cTof(command+20),cTof(command+24),cTof(command+28)};
			std::unique_lock<std::mutex> lck_food {m_food};
			food[id]=d;
			lck_food.unlock();
		}else if(command[1]=='r'&&command[2]=='f'&&command[3]=='d'){
			long long id=cTol(command+4);
			std::unique_lock<std::mutex> lck_food {m_food};
			food.erase(id);
			lck_food.unlock();
		}else if(command[1]=='b'&&command[2]=='p'&&command[3]=='b'){
			int id=cToi(command+4);
			std::unique_lock<std::mutex> lck_bug {m_bug};
			bug[id].push_back({cTof(command+8),cTof(command+12),cTof(command+16)});
			lck_bug.unlock();
		}else if(command[1]=='p'&&command[2]=='x'&&command[3]=='y'){
			int id=cToi(command+4);
			directionX[id]=cTof(command+8);
			directionY[id]=cTof(command+12);
		}else if(command[1]=='b'&&command[2]=='i'&&command[3]=='n'){
			int id=cToi(command+4);
			std::unique_lock<std::mutex> lck_bug {m_bug};
			if(bug_begin[id]){
				bug_begin[id]=bug_begin[id]->add_last(new Node<Point>{{cTof(command+8),cTof(command+12)}});
			}else{
				bug_begin[id]=new Node<Point>({cTof(command+8),cTof(command+12)});
				bug_end[id]=bug_begin[id];
			}
			lck_bug.unlock();
		}else if(command[1]=='b'&&command[2]=='r'&&command[3]=='m'){
			int id=cToi(command+4);
			std::unique_lock<std::mutex> lck_bug {m_bug};
			bug_end[id]=remove(bug_end[id]);
			lck_bug.unlock();
		}else if(command[1]=='p'&&command[2]=='d'&&command[3]=='e'){
			int id=cToi(command+4);
			is_dead[id]=true;
			if(id==playerId){
				setmode(2);
			}
			dead(id);
		}else if(command[1]=='s'&&command[2]=='p'&&command[3]=='c'){
			int id=cToi(command+4);
			score[id]=cToi(command+8);
		}
	}else if(command[0]=='p'){
		if(command[1]=='r'&&command[2]=='e'&&command[3]=='d'){
			server_a_player_ready(cToi(command+4),command+8,l-8);
		}else if(command[1]=='d'&&command[2]=='x'&&command[3]=='y'){
			int id=cToi(command+4);
			directionX[id]=cTof(command+8);
			directionY[id]=cTof(command+12);
			server_playerXY(id,directionX[id],directionY[id]);
		}else if (command[1] == 'g' && command[2] == 'p' && command[3] == 's') {
			if (mode == 1){
				submode^=1;
			}
		}


	}
}
int solveMessage(char buffer[],int l){
	buffer[l]=0;
	//printf("message begin:%d\n",l);
	for(int i=0;i<l;){
		int len=cToi(buffer+i);
		//printf("%d %d: ",len, i);
		if (i + len > l) {
			return l - i;
		}
		solve(buffer+i+4,len-4);
		i+=len;
	}
	//printf("message end\n");
	return 0;
}

void setPlayerCount(int num){
	playerCount=num;
	char command[]="splc____";
	iToc(command+4,num);
	addsendBuffer(command,8);
}
void setReadyCount(int num){
	readyCount=num;
	char command[]="srec____";
	iToc(command+4,num);
	addsendBuffer(command,8);
}
void setPlayerName(int id,char name[],int l){
	char command[256]="sspn";
	iToc(command+4,id);
	for(int i=0;i<l;i++){
		command[i+8]=name[i];
	}
	addsendBuffer(command,l+8);
}
/*void addReadyCount(){
	char s[]="sarc";
	addsendBuffer(s,4);
}*/
void playerReady(){
	char command[256]="pred";

	iToc(command+4,playerId);
	for(int i=0;i<name[playerId].size();i++){
		command[i+8]=(char)name[playerId][i];
	}
	command[name[playerId].size()+8] = '\0';
	addsendBuffer(command,9+name[playerId].size()+ (4 - (name[playerId].size() + 9) % 4) % 4);
}
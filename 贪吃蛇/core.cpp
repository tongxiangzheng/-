#include "core.h"
#include<cmath>
#include<iostream>
#include<ctime>
#include<fstream>
#include<cstring>
std::map<std::vector<wchar_t>,int> user;
std::set<std::tuple<int,std::vector<wchar_t> > > rank;
int c=0;
bool check(float x,float y,float xx,float yy){
	xx-=x;
	yy-=y;
	float d=sqrt(xx*xx+yy*yy);
	return d<circleR*2;
}
int foodCount=0;
long long foodIdCount=0;
void add_food(Point_AND_Color d){
	/*if(foodIdCount){
		return;
	}*/
	food[++foodIdCount]=d;
	insert_food(foodIdCount,d);
}
void add_food(){
	if(foodCount==FOOD_MAX){
		return;
	}
	foodCount++;
	float r=rand()*COLOR_LIMIT/RAND_MAX;
	float g=rand()*COLOR_LIMIT/RAND_MAX;
	float b=rand()*COLOR_LIMIT/RAND_MAX;
	float a=(r+g+b);
	r/=a;
	g/=a;
	b/=a;
	r+=0.1;
	g+=0.1;
	b+=0.1;
	Point_AND_Color d{rand()*SIZE_WIDTH/RAND_MAX,rand()*SIZE_WIDTH/RAND_MAX,r,g,b};
	add_food(d);
}
void createTXT(){
	std::ofstream data("usrdata.txt");
	data<<'0'<<std::endl;
	data.close();
}
int max(int a,int b){
	return a>b?a:b;
}
void writeTXT(const std::map<std::vector<wchar_t>,int> &m){
	std::ofstream data("usrdata.txt",std::ios::out|std::ios::trunc);
	data<<m.size()<<std::endl;
	for(auto [v,sc]:m){
		std::string s{v.begin(),v.end()};
		data<<s<<' '<<sc<<std::endl;
	}
	data.close();
}
void readTXT(){
	std::ifstream data("usrdata.txt");
	int n,sc;
	if(!data.is_open()){
		createTXT();
		data.open("usrdata.txt");
		n=0;
	}else{
		data>>n;
	}
	std::string s;
	for(int i=0;i<n;i++){
		data>>s>>sc;
		std::vector<wchar_t> v{s.begin(),s.end()};
		user[v]=sc;
	}
	data.close();
	for(auto [s,sc]:user){
		rank.insert({-sc,s});
	}
}
int score[PLAYER_LIMIT];
bool is_dead[PLAYER_LIMIT];
int deadCount;
void beforeGame(){
	deadCount=0;
	memset(is_dead,0,sizeof is_dead);
	for(int i=0;i<playerCount;i++){
		directionX[i]=0;
		directionY[i]=1;
		score[i]=0;
	}
	if(playerId){
		return;
	}
	for(int i=0;i<playerCount;i++){
		float x=(rand()*(SIZE_WIDTH-2))/RAND_MAX+1,y=(rand()*(SIZE_WIDTH-2))/RAND_MAX+1;
		bug_begin[i]=new Node<Point>({x,y});
		bug_end[i]=bug_begin[i];
		bug_inset_node(i,x,y);
		bug_push_back(i,{0.8,0.8,0.8});
		bug_push_back(i, {0.8,0.8,0.8});
		setDeltaXY(i,x,y);
	}
	for (int i=0;i<50;i++) {
		add_food();
	}
	setReadyCount(0);
}
void setmode(int m);
void dead(int id){
	user[name[id]]=max(user[name[id]],score[id]);
	writeTXT(user);
	rank.clear();
	for(auto [s,sc]:user){
		std::vector<wchar_t> v;
		for(auto c:s){
			v.push_back(c);
		}
		rank.insert({-sc,v});
	}
	is_dead[id]=true;
	if(id==playerId){
		setmode(2);
	}
}
void core_dead(int id){
	deadCount++;
	Node<Point> *p=bug_end[id];
	int ii=0;
	while(p){
		float x=p->data.x+rand()*0.1/RAND_MAX-0.05;
		float y=p->data.y+rand()*0.1/RAND_MAX-0.05;
		if (rand() & 1) {
			add_food({ x,y,bug[id][ii].r,bug[id][ii].g,bug[id][ii].b });
		}
		p=p->last;
		ii++;
	}
	dead(id);
	player_dead(id);
}
extern int fps;
int lasttime;
void core(){
	if(deadCount==playerCount){
		return;
	}
	if (lasttime != time(NULL)) {
		lasttime = time(NULL);
		++c;
		if (c == 1) {
			c = 0;
			add_food();
		}
	}

	for(int i=0;i<playerCount;i++){
		if(is_dead[i]){
			continue;
		}
		if (fps) {
			deltaX[i] += directionX[i] * speed / fps;
			deltaY[i] += directionY[i] * speed / fps;
		}
		setDeltaXY(i,deltaX[i],deltaY[i]);
		if(deltaX[i]<=circleR||deltaY[i]<=circleR||deltaX[i]>=SIZE_WIDTH-circleR||deltaY[i]>=SIZE_HEIGHT-circleR){
			core_dead(i);
			return;
		}
	}
	for(int ch=0;ch<playerCount;ch++){
		if(is_dead[ch]){
			continue;
		}
		for(auto it=food.begin();it!=food.end();it++){
			Point_AND_Color foodData=it->second;
			if(check(foodData.x,foodData.y,deltaX[ch],deltaY[ch])){
				foodCount--;
				bug_push_back(ch,{foodData.r,foodData.g,foodData.b});
				//bug[ch].push_back(p->data);
				remove_food(it->first);
				it=food.erase(it);
				bug_begin[ch]=bug_begin[ch]->add_last(new Node<Point>({deltaX[ch],deltaY[ch]}));
				bug_inset_node(ch,deltaX[ch],deltaY[ch]);
				score[ch]++;
				set_player_score(ch,score[ch]);
				return;
			}
		}
	}
	for(int i=0;i<playerCount;i++){
		if(is_dead[i]){
			continue;
		}
		for(int j=0;j<playerCount;j++){
			if(is_dead[j]||i==j){
				continue;
			}
			if(is_dead[i]){
				break;
			}
			Node<Point> *p=bug_begin[j];
			while(p){
				if(check(p->data.x,p->data.y,deltaX[i],deltaY[i])){
					core_dead(i);
					break;
				}
				p=p->next;
			}
		}
	}
	for(int i=0;i<playerCount;i++){
		if(is_dead[i]){
			continue;
		}
		bug_begin[i]=bug_begin[i]->add_last(new Node<Point>{{deltaX[i],deltaY[i]}});
		bug_inset_node(i,deltaX[i],deltaY[i]);
		bug_end[i]=remove(bug_end[i]);
		bug_remove_node(i);
	}
}
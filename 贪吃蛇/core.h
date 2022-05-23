#include "sync.h"
#include <set>
#include <tuple>
#include <string>
void core();
void add_food();
void readTXT();
void beforeGame();
extern int score[PLAYER_LIMIT];
extern std::map<std::vector<wchar_t>,int> user;
extern std::set<std::tuple<int,std::vector<wchar_t> > > rank;
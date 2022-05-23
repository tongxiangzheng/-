#include"shell.h"
#include <iostream>
#include <cmath>
#include <algorithm>
int mode = 0, submode = 0;
/*
0 菜单
1 游戏
2 死
3 排行榜
4 是否连接服务器
5 连接服务器选项
*/
float nameLenth[PLAYER_LIMIT];
float connectIpLenth;
std::vector<wchar_t>* input;
float* inputLenth;
void setmode(int m) {
	submode = 0;
	if (mode == 0) {
		input = nullptr;
		inputLenth = nullptr;
	}
	else if (mode == 3) {
		food.clear();
		for (int i = 0; i < playerCount; i++) {
			bug_begin[i]->clear();
			delete bug_begin[i];
			bug_begin[i] = nullptr;
			bug_end[i] = nullptr;
			bug[i].clear();
		}
	}
	else if (mode == 5) {
		input = nullptr;
		inputLenth = nullptr;
	}
	mode = m;
	if (mode == 0) {
		input = name + playerId;
		inputLenth = nameLenth + playerId;
	}
	else if (mode == 1) {
		beforeGame();
	}
	else if (mode == 5) {
		input = &connectIp;
		inputLenth = &connectIpLenth;
	}
}
void ready() {
	if (submode == 2) {
		return;
	}
	submode = 2;
	input = nullptr;
	inputLenth = nullptr;
	if (playerId) {
		playerReady();
	}
	else {
		char s[256];
		for (int i = 0; i < name[playerId].size(); i++) {
			s[i] = (char)name[playerId][i];
		}
		s[name[playerId].size()] = '\0';
		server_a_player_ready(0, s, name[playerId].size() + 1+(4-(name[playerId].size()+1)%4)%4);
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//std::cout<<key<<std::endl;
	if (action == GLFW_PRESS) {
		wchar_t c = 0;
		if (mode == 1) {
			if (key == GLFW_KEY_SPACE) {
				game_pause();
			}
		}
		if (key == GLFW_KEY_ENTER) {
			if (mode == 0) {
				if (!(*input).empty()) {
					ready();
				}
				else {
					submode |= 1;
				}
			}
			if(mode == 5) {
				char s[1024];
				int l = 0;
				for (wchar_t c : connectIp) {
					s[l] = c;
					l++;
				}
				s[l] = '\0';
				if (userPlayer(s)) {
					setmode(0);
				}
				else {
					submode = 120;
				}
			}
		}
		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
			c = '0' + key - GLFW_KEY_0;
		}
		else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
			c = '0' + key - GLFW_KEY_KP_0;
		}
		else if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
			c = 'a' + key - GLFW_KEY_A;
			if ((bool(mods & GLFW_MOD_SHIFT)) ^ (bool(mods & GLFW_MOD_CAPS_LOCK))) {
				c -= 'a' - 'A';
			}
		}
		else if (key == GLFW_KEY_PERIOD || key == GLFW_KEY_KP_DECIMAL) {
			c = '.';
		}/*else if(key==GLFW_KEY_SPACE){ //freetype中空格宽度为0
			wchar_t c=' ';
		}*/else if (key == GLFW_KEY_BACKSPACE) {
			if (input && !(*input).empty()) {
				wchar_t c2 = (*input).back();
				(*input).pop_back();
				(*inputLenth) -= Characters[c2].Advance * 1.0 / FT_SIZE / 64;
			}
		}
		if (c && input) {
			(*input).push_back(c);
			(*inputLenth) += Characters[c].Advance * 1.0 / FT_SIZE / 64;
			submode = 0;
		}
		if (mode == 2) {
			if (c == L'a') {
				submode = -1;
			}else if (c == L'd') {
				submode = -2;
			}else if (c == L'w') {
				submode = -3;
			}else if (c == L's') {
				submode = -4;
			}
			if (c >= L'1' && c <= L'9') {
				int p = c - L'0'-1;
				if (p < playerCount) {
					submode = p + 1;
				}
			}
		}
	}
	else if (action == GLFW_REPEAT) {
		if (key == GLFW_KEY_BACKSPACE) {
			if (input && !(*input).empty()) {
				wchar_t c = (*input).back();
				(*input).pop_back();
				(*inputLenth) -= Characters[c].Advance * 1.0 / FT_SIZE / 64;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		if (mode == 2) {
			wchar_t c = 0;
			if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
				c = 'a' + key - GLFW_KEY_A;
				if ((bool(mods & GLFW_MOD_SHIFT)) ^ (bool(mods & GLFW_MOD_CAPS_LOCK))) {
					c -= 'a' - 'A';
				}
			}
			if (c == L'a' && submode == -1) {
				submode = 0;
			}
			else if (c == L'd' && submode == -2) {
				submode = 0;
			}
			else if (c == L'w' && submode == -3) {
				submode = 0;
			}
			else if (c == L's' && submode == -4) {
				submode = 0;
			}
		}
	}
}

float strlenth(const std::vector<wchar_t>& str) {
	float lenth = 0;
	for (auto c : str) {
		if (Characters.find(c) == Characters.end()) {
			add_type(c);
		}
		lenth += Characters[c].Advance * 1.0 / FT_SIZE / 64;
	}
	return lenth;
}
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos /= SCR_WIDTH;
		ypos /= SCR_HEIGHT;
		xpos = xpos * 2 - 1.0;
		ypos = -ypos * 2 + 1.0;
		//std::cout<<xpos<<' '<<ypos<<std::endl;
		if (mode == 0) {
			static const float l3 = strlenth({ L'开',L'始',L'游',L'戏' });
			if (xpos >= -l3 * 0.5 * 0.2 && xpos <= l3 * 0.5 * 0.2 && ypos >= -0.31 && ypos <= -0.136) {
				//std::cout<<"YES"<<std::endl;
				if (!name[playerId].empty()) {
					ready();
				}
				else {
					submode |= 1;
				}
				return;
			}
		}
		if (mode == 2) {
			static const float l2 = strlenth({ L'确',L'认' });
			bool all_dead = true;
			for (int i = 0; i < playerCount; i++) {
				all_dead &= is_dead[i];
			}
			if (all_dead) {
				if (xpos >= -l2 * 0.5 * 0.15 && xpos <= l2 * 0.5 * 0.15 && ypos >= -0.724 && ypos <= -0.578) {
					setmode(3);
					return;
				}
			}
		}
		if (mode == 3) {
			static const float l2 = strlenth({ L'确',L'认' });
			if (xpos >= -l2 * 0.5 * 0.15 && xpos <= l2 * 0.5 * 0.15 && ypos >= -0.724 && ypos <= -0.578) {
				setmode(0);
				return;
			}
		}
		if (mode == 4) {
			static const float l2 = strlenth({ L'连',L'接',L'到',L'服',L'务',L'器' }), l3 = strlenth({ L'将',L'本',L'机',L'作',L'为',L'服',L'务',L'器' });
			if (xpos >= -l2 * 0.5 * 0.11 - 0.5 && xpos <= l2 * 0.5 * 0.11 - 0.5 && ypos >= 0 && ypos <= 0.11) {
				setmode(5);
			}
			if (xpos >= -l2 * 0.5 * 0.11 + 0.5 && xpos <= l3 * 0.5 * 0.11 + 0.5 && ypos >= 0 && ypos <= 0.11) {
				serverPlayer();
				setmode(0);
			}
		}
		if (mode == 5) {
			static const float l3 = strlenth({ L'连',L'接' }), l5 = strlenth({ L'返',L'回' });
			if (xpos >= -l3 * 0.5 * 0.15 && xpos <= l3 * 0.5 * 0.15 && ypos >= -0.33 && ypos <= -0.136) {
				char s[1024];
				int l = 0;
				for (wchar_t c : connectIp) {
					s[l] = c;
					l++;
				}
				s[l] = '\0';
				if (userPlayer(s)) {
					setmode(0);
				}
				else {
					submode = 120;
				}
			}
			else if (xpos >= -l3 * 0.5 * 0.15 && xpos <= l3 * 0.5 * 0.15 && ypos >= -0.63 && ypos <= -0.436) {
				setmode(4);
			}
		}
	}
}
int vertexUniformColor1, vertexUniformColor4, vertexUniformPosDelta, vertexUniformR, vertexUniformText;

unsigned int shaderProgram1, shaderProgram2, shaderProgram3, shaderProgram4;
unsigned int VAO;

void drawstr(const std::vector<wchar_t>& str, float x, float y, float scale, float r, float g, float b) {
	glUseProgram(shaderProgram4);
	glBindVertexArray(VAO);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform3f(vertexUniformColor4, r, g, b);
	glActiveTexture(GL_TEXTURE0);
	for (auto c : str) {
		if (Characters.find(c) == Characters.end()) {
			add_type(c);
		}
		Character ch = Characters[c];
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		GLfloat xpos = x + ch.Bearing_x * scale / FT_SIZE;
		GLfloat ypos = y - (ch.Size_y - ch.Bearing_y) * scale / FT_SIZE;
		GLfloat w = ch.Size_x * scale / FT_SIZE;
		GLfloat h = ch.Size_y * scale / FT_SIZE;
		//std::cout<<(char)str[i]<<' '<<x<<std::endl<<xpos<<' '<<ypos<<std::endl<<w<<' '<<h<<std::endl;
		glUniform2f(vertexUniformPosDelta, (xpos + w * 0.5) * H_W, (ypos + h * 0.5));
		glUniform2f(vertexUniformR, w * 0.5 * H_W, h * 0.5);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		x += ch.Advance * scale / FT_SIZE / 64;
		//std::cout<<(char)str[i]<<ch.Advance<<' ';
	}
	//std::cout<<std::endl;
}

std::vector<wchar_t> iTov(int num) {
	std::vector<wchar_t> v;
	if (num == 0) {
		v.push_back(L'0');
	}
	while (num) {
		wchar_t c = num % 10 + '0';
		v.push_back(c);
		num /= 10;
	}
	return { v.rbegin(),v.rend() };
}
void menu() {
	static const float l1 = strlenth({ L'贪',L'吃',L'蛇' }), l2 = strlenth({ L'你',L'的',L'名',L'字',L':' }), l3 = strlenth({ L'开',L'始',L'游',L'戏' }),
		l4 = strlenth({ L'请',L'输',L'入',L'名',L'字' }), l5 = strlenth({ L'准',L'备',L'开',L'始' }), l6 = strlenth({ L'等',L'待',L'其',L'他',L'玩',L'家',L'准',L'备' });
	drawstr({ L'贪',L'吃',L'蛇' }, -l1 * 0.5 * 0.3, 0.6, 0.3, 1.0, 0.5, 0.3);
	drawstr({ L'你',L'的',L'名',L'字',L':' }, -l2 * 0.5 * 0.18, 0.3, 0.18, 0.8, 0.8, 0.8);
	if ((submode & 2) == 0) {
		if (readyCount >= playerCount - 1) {
			drawstr({ L'开',L'始',L'游',L'戏' }, -l3 * 0.5 * 0.2, -0.3, 0.2, 1, 1, 1);
		}
		else {
			drawstr({ L'准',L'备',L'开',L'始' }, -l5 * 0.5 * 0.2, -0.3, 0.2, 1, 1, 1);
		}
	}
	else {
		drawstr({ L'等',L'待',L'其',L'他',L'玩',L'家',L'准',L'备' }, -l6 * 0.5 * 0.2, -0.3, 0.2, 1, 1, 1);
	}
	if (submode & 1) {
		drawstr({ L'请',L'输',L'入',L'名',L'字' }, -l4 * 0.1 * 0.5, 0.1, 0.1, 0.8, 0.4, 0.4);
	}
	drawstr(name[playerId], -nameLenth[playerId] * 0.1 * 0.5, 0.1, 0.1, 0.7, 0.8, 1);
	std::vector<wchar_t> v = iTov(playerCount);
	v.push_back(L'人'); v.push_back(L'已'); v.push_back(L'加'); v.push_back(L'入');
	drawstr(v, -strlenth(v) * 0.5 * 0.08, -0.8, 0.08, 0.6, 0.7, 0.8);
	v = iTov(readyCount);
	v.push_back(L'人'); v.push_back(L'已'); v.push_back(L'准'); v.push_back(L'备');
	drawstr(v, -strlenth(v) * 0.5 * 0.08, -0.88, 0.08, 0.6, 0.7, 0.8);
}
extern std::set<std::tuple<int, std::vector<wchar_t> > > rank;
void gameDisplay() {
	glUseProgram(shaderProgram1);
	glBindVertexArray(VAO);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUniform2f(vertexUniformR, 0.01 * H_W, 0.01);
	glUniform3f(vertexUniformColor1, 1, 1, 1);
	for (int i = 0; i < playerCount; i++) {
		if (is_dead[i]) {
			continue;
		}
		glUniform2f(vertexUniformPosDelta, ((directionX[i] + directionY[i]) * circleR * 0.5 + bug_begin[i]->data.x - deltaX[playerId]) * H_W, (directionY[i] - directionX[i]) * circleR * 0.5 + bug_begin[i]->data.y - deltaY[playerId]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUniform2f(vertexUniformPosDelta, ((directionX[i] - directionY[i]) * circleR * 0.5 + bug_begin[i]->data.x - deltaX[playerId]) * H_W, (directionY[i] + directionX[i]) * circleR * 0.5 + bug_begin[i]->data.y - deltaY[playerId]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//printf("%dxy %f %f\n",i,directionX[i],directionY[i]);

	}
	//std::cout<<deltaY[playerId]<<std::endl;
	glUniform2f(vertexUniformR, circleR * H_W, circleR);
	std::unique_lock<std::mutex> lck_bug{ m_bug };
	for (int i = 0; i < playerCount; i++) {
		if (is_dead[i]) {
			continue;
		}
		Node<Point>* p2 = bug_end[i];
		int ii = 0;
		while (p2) {
			glUniform3f(vertexUniformColor1, bug[i][ii].r, bug[i][ii].g, bug[i][ii].b);
			glUniform2f(vertexUniformPosDelta, (p2->data.x - deltaX[playerId]) * H_W, p2->data.y - deltaY[playerId]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			p2 = p2->last;
			ii++;
		}
		//printf("id %d node %d bugsize %d score %d\n",i,ii,bug[i].size(),score[i]);
	}
	lck_bug.unlock();
	glUniform2f(vertexUniformR, circleR * H_W, circleR);

	std::unique_lock<std::mutex> lck_food{ m_food };
	for (auto [foodId, foodData] : food) {
		glUniform3f(vertexUniformColor1, foodData.r, foodData.g, foodData.b);
		glUniform2f(vertexUniformPosDelta, (foodData.x - deltaX[playerId]) * H_W, foodData.y - deltaY[playerId]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	lck_food.unlock();
	glUseProgram(shaderProgram3);
	glBindVertexArray(VAO);
	glUniform3f(vertexUniformColor1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
	glUniform2f(vertexUniformPosDelta, (SIZE_WIDTH / 2 - deltaX[playerId]) * H_W, SIZE_HEIGHT / 2 - deltaY[playerId]);
	glUniform2f(vertexUniformR, SIZE_WIDTH / 2 * H_W, SIZE_HEIGHT / 2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	std::tuple<int, int> rank[PLAYER_LIMIT];
	float lmax = 0;
	for (int i = 0; i < playerCount; i++) {
		rank[i] = { score[i],i };
		lmax = std::max(lmax, nameLenth[i]);
	}
	std::sort(rank, rank + playerCount,
		[](auto a, auto b) {return std::get<0>(a) > std::get<0>(b); }
	);
	float y = 0.95;
	for (int i = 0; i < playerCount; i++) {
		int id = std::get<1>(rank[i]);
		std::vector<wchar_t> v = name[id];
		v.push_back(' ');
		std::vector<wchar_t> v2 = iTov(score[id]);
		for (auto c : v2) {
			v.push_back(c);
		}
		drawstr(v, -1 / H_W + (lmax - nameLenth[id]) * 0.05, y, 0.05, 0.7, 0.7, 0.8);
		y -= 0.05;
	}
}
void end() {
	static const float l1 = strlenth({ L'死' });
	//std::cout<<l1*0.5*0.3<<std::endl;
	drawstr({ L'死' }, -l1 * 0.5 * 0.3, 0.2, 0.3, 1.0, 0, 0);
	std::vector<wchar_t> v = iTov(score[playerId]);
	v.push_back(L'分');
	drawstr(v, -strlenth(v) * 0.5 * 0.1, -0.2, 0.1, 0.9, 0.9, 0.9);
	static float l2 = strlenth({ L'确',L'认' }), l3 = strlenth({ L'请',L'等',L'待',L'其',L'他',L'人',L'_',L'_' });
	bool all_dead = true;
	for (int i = 0; i < playerCount; i++) {
		all_dead &= is_dead[i];
	}
	if (all_dead) {
		drawstr({ L'确',L'认' }, -l2 * 0.5 * 0.15, -0.7, 0.15, 1.0, 1.0, 1.0);
	}
	else {
		drawstr({ L'请',L'等',L'待',L'其',L'他',L'人',L'_',L'_' }, -l3 * 0.5 * 0.15, -0.7, 0.15, 1.0, 1.0, 1.0);
	}
}
void ranklist() {
	static float l1 = strlenth({ L'排',L'行',L'榜' });
	drawstr({ L'排',L'行',L'榜' }, -l1 * 0.5 * 0.3, 0.7, 0.3, 1.0, 1.0, 1.0);
	int i = 0;
	float y = 0.5;
	for (auto [sc, v] : rank) {
		i++;
		if (i == 10) {
			break;
		}
		int playerP = -1;
		for (int i = 0; i < playerCount; i++) {
			if (v == name[i]) {
				playerP = i;
				break;
			}
		}
		float l = strlenth(v);
		v.push_back(L' ');
		auto v2 = iTov(-sc);
		for (auto c : v2) {
			v.push_back(c);
		}
		drawstr({ L'历',L'史',L'最',L'高' }, 0.0, 0.6, 0.05, 0.9, 0.9, 0.9);
		drawstr({ L'本',L'次',L'成',L'绩' }, 0.3, 0.6, 0.05, 0.9, 0.9, 0.9);
		if (playerP >= 0) {
			if (playerP == playerId) {
				drawstr(v, -l * 0.1, y, 0.1, 0.9, 0.4, 0.5);
				drawstr(iTov(score[playerP]), 0.3, y, 0.1, 0.9, 0.4, 0.5);
			}
			else {
				drawstr(v, -l * 0.1, y, 0.1, 0.8, 0.7, 0.9);
				drawstr(iTov(score[playerP]), 0.3, y, 0.1, 0.8, 0.7, 0.9);
			}
		}
		else {
			drawstr(v, -l * 0.1, y, 0.1, 0.8, 0.8, 0.8);
		}
		y -= 0.1;
	}
	static float l2 = strlenth({ L'确',L'认' });
	drawstr({ L'确',L'认' }, -l2 * 0.5 * 0.15, -0.7, 0.15, 1.0, 1.0, 1.0);
}
void selectMenu() {
	static const float l1 = strlenth({ L'贪',L'吃',L'蛇' }), l2 = strlenth({ L'连',L'接',L'到',L'服',L'务',L'器' }),
		l3 = strlenth({ L'将',L'本',L'机',L'作',L'为',L'服',L'务',L'器' });
	drawstr({ L'贪',L'吃',L'蛇' }, -l1 * 0.5 * 0.3, 0.6, 0.3, 1.0, 0.5, 0.3);
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	xpos /= SCR_WIDTH;
	ypos /= SCR_HEIGHT;
	xpos = xpos * 2 - 1.0;
	ypos = -ypos * 2 + 1.0;
	if (xpos >= -l2 * 0.5 * 0.11 - 0.5 && xpos <= l2 * 0.5 * 0.11 - 0.5 && ypos >= 0 && ypos <= 0.11) {
		drawstr({ L'连',L'接',L'到',L'服',L'务',L'器' }, -l2 * 0.5 * 0.11 - 0.5, 0.0, 0.11, 1.0, 0.5, 0.3);
	}
	else {
		drawstr({ L'连',L'接',L'到',L'服',L'务',L'器' }, -l2 * 0.5 * 0.1 - 0.5, 0.0, 0.1, 1.0, 0.5, 0.3);
	}
	if (xpos >= -l2 * 0.5 * 0.11 + 0.5 && xpos <= l3 * 0.5 * 0.11 + 0.5 && ypos >= 0 && ypos <= 0.11) {
		drawstr({ L'将',L'本',L'机',L'作',L'为',L'服',L'务',L'器' }, -l3 * 0.5 * 0.11 + 0.5, 0.0, 0.11, 1.0, 0.5, 0.3);
	}
	else {
		drawstr({ L'将',L'本',L'机',L'作',L'为',L'服',L'务',L'器' }, -l3 * 0.5 * 0.1 + 0.5, 0.0, 0.1, 1.0, 0.5, 0.3);

	}
}
void connectMenu() {
	static const float l2 = strlenth({ L'i',L'p',L'地',L'址',L':' }),
		l3 = strlenth({ L'连',L'接' }), l4 = strlenth({ L'连',L'接',L'失',L'败' }),l5=strlenth({L'返',L'回'});
	drawstr({ L'i',L'p',L'地',L'址',L':' }, -l2 * 0.5 * 0.18, 0.3, 0.18, 0.8, 0.8, 0.8);
	drawstr({ L'连',L'接' }, -l3 * 0.5 * 0.2, -0.3, 0.2, 1, 1, 1);
	drawstr({ L'返',L'回' }, -l5 * 0.5 * 0.2, -0.6, 0.2, 1, 1, 1);
	if (submode) {
		drawstr({ L'连',L'接',L'失',L'败' }, -l4 * 0.1 * 0.5, -0.1, 0.1, 0.8, 0.4, 0.4);
		submode--;
	}
	drawstr(connectIp, -connectIpLenth * 0.1 * 0.5, 0.1, 0.1, 0.7, 0.8, 1);

}
float col_bk = pow(0.01f, 1 / 2.2);
int c2 = 0, fps = 60;
time_t lasttime, lltime;
void display() {
	time_t ti = time(NULL);
	if (lasttime != ti) {
		if (lltime == lasttime - 1) {
			fps = (fps+c2)/2;
		}
		lltime = lasttime;
		lasttime = ti;
		c2 = 0;
	}
	++c2;
	glClearColor(col_bk, col_bk, col_bk, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (mode == 0) {
		menu();
	}
	if (mode == 1 || mode == 2) {
		if (mode == 2) {
			if (submode > 0) {
				deltaX[playerId] = bug_begin[submode - 1]->data.x;
				deltaY[playerId] = bug_begin[submode - 1]->data.y;
			}
			else if (submode < 0) {
				if (submode == -1) {
					deltaX[playerId] -= speed/fps;
				}
				else if (submode == -2) {
					deltaX[playerId] += speed / fps;
				}
				else if (submode == -3) {
					deltaY[playerId] += speed / fps;
				}
				else {
					deltaY[playerId] -= speed / fps;
				}
			}
		}
		gameDisplay();
		if ((submode & 1) == 0) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			sendXY(xpos, ypos);
			if (playerId == 0) {
				core();
			}
		}
	}
	if (mode == 2) {
		end();
	}
	if (mode == 3) {
		ranklist();
	}
	if (mode == 4) {
		selectMenu();
	}
	if (mode == 5) {
		connectMenu();
	}
}

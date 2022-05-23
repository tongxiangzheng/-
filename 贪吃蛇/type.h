#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
//#pragma comment(lib,"freetype.lib")
#include <map>
extern const int FT_SIZE;
struct Character {
    GLuint     textureID;  // ���������ID
    unsigned int Size_x, Size_y;       // ���δ�С
    int Bearing_x, Bearing_y;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
    FT_Pos     Advance;    // ԭ�����һ������ԭ��ľ���
};
extern std::map<wchar_t, Character> Characters;
void initFont();
void add_type(FT_ULong c);
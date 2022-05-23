#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
//#pragma comment(lib,"freetype.lib")
#include <map>
extern const int FT_SIZE;
struct Character {
    GLuint     textureID;  // 字形纹理的ID
    unsigned int Size_x, Size_y;       // 字形大小
    int Bearing_x, Bearing_y;    // 从基准线到字形左部/顶部的偏移值
    FT_Pos     Advance;    // 原点距下一个字形原点的距离
};
extern std::map<wchar_t, Character> Characters;
void initFont();
void add_type(FT_ULong c);
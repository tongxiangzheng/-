#include "type.h"
#include <iostream>
const int FT_SIZE = 284;

std::map<wchar_t, Character> Characters;
FT_Face face;
FT_Library ft;
void add_type(FT_ULong c) {
	// 加载字符的字形 
	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		return;
	}
	// 生成纹理
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		face->glyph->bitmap.buffer
	);
	// 设置纹理选项
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// 储存字符供之后使用

	Character character = {
		texture,
		face->glyph->bitmap.width, face->glyph->bitmap.rows,
		face->glyph->bitmap_left, face->glyph->bitmap_top,
		face->glyph->advance.x
	};
	//std::cout<<(char)c<<' '<<face->glyph->bitmap.width<<' '<<face->glyph->bitmap.rows
	//<<' '<<face->glyph->bitmap_left<<' '<<face->glyph->bitmap_top<<std::endl;

	Characters.insert(std::pair<wchar_t, Character>(c, character));

}

void initFont() {
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	if (FT_New_Face(ft, "C:\\Windows\\Fonts\\msyh.ttc", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(face, 0, FT_SIZE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (wchar_t c = 0; c < 128; c++) {
		add_type(c);
	}
	add_type(L'分');
}
#include "Font.h"

// Initiate the FreeType library
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <freetype/freetype.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

Font::Font(const char* path, int size)
{
    Load(path, size);
}

Font::~Font()
{
}

void Font::Load(const char* path, int size)
{
    FT_Library m_Library;
    FT_Face m_Face;

    FT_Init_FreeType(&m_Library);
    if(!m_Library)
    {
        std::cout << "Failed to initialize FreeType library." << std::endl;
        return;
    }

    FT_New_Face(m_Library, path, 0, &m_Face);
    if(!m_Face)
    {
        std::cout << "Failed to load font." << std::endl;
        return;
    }

    // size is in pixels (both width and height are the same value)
    FT_Set_Pixel_Sizes(m_Face, 0, size);


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

#ifdef __EMSCRIPTEN__

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_R8,
            m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            m_Face->glyph->bitmap.buffer
        );
#else 
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            m_Face->glyph->bitmap.buffer
        );
#endif
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
            glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
             static_cast<unsigned int>(m_Face->glyph->advance.x)
        };
        characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(m_Face);
    FT_Done_FreeType(m_Library);

    m_Size = size;
}


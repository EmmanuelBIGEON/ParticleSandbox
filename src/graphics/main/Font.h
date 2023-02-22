#pragma once

#include <glm/glm.hpp>

#include <map>

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

//! \class Font
//! \brief A class to load fonts using FreeType.
class Font
{
    public:
        Font(const char* path, int size);
        virtual ~Font();

        //! \brief Load a font 
        //! Automatically called by the constructor.
        void Load(const char* path, int size);

        int GetSize() const { return m_Size; }

        std::map<char, Character> characters;
    private:
        int m_Size;        
};
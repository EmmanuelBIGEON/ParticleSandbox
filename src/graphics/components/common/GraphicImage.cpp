#include "GraphicImage.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

#include "../../main/Shader.h"

GraphicImage::GraphicImage(GraphicContext* context, const std::string& path, float x, float y, float width, float height) 
    : GraphicObject(context, SHADER_TEXTURE), m_Path(path), m_x(x), m_y(y), m_width(width), m_height(height), m_VAO(0), m_VBO(0), m_EBO(0), m_Texture(0)
{
    m_Shader = context->GetShader(SHADER_TEXTURE);
}

GraphicImage::~GraphicImage()
{
    
}

void GraphicImage::Update()
{
    m_vertices[0] = m_x + m_width;
    m_vertices[1] = m_y + m_height;
    m_vertices[2] = 1.0f; // z
    m_vertices[3] = 1.0f; // r
    m_vertices[4] = 0.0f; // g
    m_vertices[5] = 0.0f; // b
    m_vertices[6] = 1.0f; // texture x
    m_vertices[7] = 1.0f; // texture y
    
    m_vertices[8] = m_x + m_width;
    m_vertices[9] = m_y;
    m_vertices[10] = 1.0f; // z
    m_vertices[11] = 0.0f; // r
    m_vertices[12] = 1.0f; // g
    m_vertices[13] = 0.0f;  // b
    m_vertices[14] = 1.0f; // texture x
    m_vertices[15] = 0.0f; // texture y

    m_vertices[16] = m_x;
    m_vertices[17] = m_y;
    m_vertices[18] = 1.0f; // z
    m_vertices[19] = 0.0f; // r
    m_vertices[20] = 0.0f; // g
    m_vertices[21] = 1.0f; // b
    m_vertices[22] = 0.0f; // texture x
    m_vertices[23] = 0.0f; // texture y

    m_vertices[24] = m_x;
    m_vertices[25] = m_y + m_height;
    m_vertices[26] = 1.0f; // z
    m_vertices[27] = 1.0f; // r
    m_vertices[28] = 1.0f; // g
    m_vertices[29] = 0.0f; // b
    m_vertices[30] = 0.0f; // texture x
    m_vertices[31] = 1.0f; // texture y

    
    m_indices[0] = 0;
    m_indices[1] = 1;
    m_indices[2] = 3;
    m_indices[3] = 1;
    m_indices[4] = 2;
    m_indices[5] = 3;

    // int indices[] = {
    //     0, 1, 3, // first triangle
    //     1, 2, 3  // second triangle
    // };
    

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    std::cout << "Texture ID : " << m_Texture << std::endl;
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // flip the image vertically
    unsigned char *data = stbi_load(m_Path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        
        // check if png or jpg
        std::string extension = m_Path.substr(m_Path.find_last_of(".") + 1);
        if(extension == "jpg")
        {
#ifdef __EMSCRIPTEN__
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB8, GL_UNSIGNED_BYTE, data); // GL_RGB
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // GL_RGB
#endif
            glGenerateMipmap(GL_TEXTURE_2D);
        }else if(extension == "png")
        {

#ifdef __EMSCRIPTEN__
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, data); // GL_RGBA
#else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // GL_RGBA
#endif
            glGenerateMipmap(GL_TEXTURE_2D);
        }

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    // glUniform1i(glGetUniformLocation(m_Shader->shaderID, "texture1"), 0);
    // m_Shader->SetInt("mainTexture", 0);

    m_IsUpdated = true;
    std::cout << "GraphicImage::Update() : " << m_Path << std::endl;

    glBindVertexArray(0);
}   

void GraphicImage::Draw() 
{
    // std::cout << "GraphicImage::Draw() : " << m_Path << std::endl;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

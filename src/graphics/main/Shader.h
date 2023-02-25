#pragma once

#include <glm/ext.hpp>

class Shader
{
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        virtual ~Shader();

        void Use();

        void SetBool(const char* name, bool value) const;
        void SetInt(const char* name, int value) const;
        void SetFloat(const char* name, float value) const;
        void SetVec2(const char* name, const glm::vec2& value) const;
        void SetVec3(const char* name, const glm::vec3& value) const;
        void SetMat2(const char* name, const glm::mat2& value) const;
        void SetMat3(const char* name, const glm::mat3& value) const;
        void SetMat4(const char* name, const glm::mat4& value) const;

        unsigned int shaderID;

};
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    Shader(char *name);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
};

void getfile(char *s,char *path) {
    // puts(path);
    FILE* fp = fopen(path, "r");
    for(char c = fgetc(fp); c != EOF; c=fgetc(fp)) *(s++) = c;
    *s = '\0';
}

Shader::Shader(char *name) {
    static char Shaderpath[512];
    static char vertexSource[1024];
    static char fragmentSource[1024];
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    sprintf(Shaderpath, "./shader/%s.vs",name);
    getfile(vertexSource, Shaderpath); const char* vertex = vertexSource;
    // puts(vertex);
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glCompileShader(vertexShader);
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    sprintf(Shaderpath, "./shader/%s.fs",name);
    getfile(fragmentSource, Shaderpath); const char* fragment = fragmentSource;
    // puts(fragment);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);
    // link shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // use();
};

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name,bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}

void Shader::setInt(const std::string &name,int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}

void Shader::setFloat(const std::string &name,float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}

#endif
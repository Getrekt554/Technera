#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glad/glad.h>

const char* get_shader(char* name) {
    const char* base = "../src/shaders/";
    size_t len = strlen(base) + strlen(name) + 1;
    char* path = (char *)malloc(len);
    if (!path) {
        printf("couldn't open shader file");
        return NULL;
    }
    memcpy(path, base, strlen(base));
    memcpy(path+strlen(base), name, strlen(name) + 1);

    FILE* shader_file = fopen(path, "r");
    if (shader_file == NULL) {
        printf("couldn't open shader file");
        return NULL;
    }

    fseek(shader_file, 0,  SEEK_END);
    long size = ftell(shader_file);
    fseek(shader_file, 0, SEEK_SET);

    char* shader_code = (char *)malloc(size+1);

    fread(shader_code, 1, size, shader_file);
    fclose(shader_file);
    
    return shader_code;
}

unsigned int init_shader(GLenum shader_type, const char* src_code) {
    unsigned int shader;
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src_code, NULL);
    glCompileShader(shader);

    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("ERROR::SHADER::COMPILATION_FAILED\n %s\n", info_log);
    }

    return shader;
}

unsigned int init_shader_program(unsigned int &vertex_shader, unsigned int &fragment_shader) {
    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s\n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}


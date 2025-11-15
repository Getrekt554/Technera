#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


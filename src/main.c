#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "gl_ext.h"
#include "shader_utils.h"

int current_anim_mode = 0; // 0 = Cube, 1 = Wave

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) {
            current_anim_mode = 0;
            printf("Selected Animation: 3D Cube\n");
        } else if (key == GLFW_KEY_2) {
            current_anim_mode = 1;
            printf("Selected Animation: Wave Grid\n");
        }
    }
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

// Generate wave grid
#define GRID_SIZE 40
int generate_wave_grid(GLfloat** vertices, GLfloat** colors) {
    int num_vertices = (GRID_SIZE - 1) * (GRID_SIZE - 1) * 6;
    *vertices = (GLfloat*)malloc(num_vertices * 3 * sizeof(GLfloat));
    *colors = (GLfloat*)malloc(num_vertices * 3 * sizeof(GLfloat));
    
    int index = 0;
    for (int z = 0; z < GRID_SIZE - 1; z++) {
        for (int x = 0; x < GRID_SIZE - 1; x++) {
            float x0 = (x / (float)(GRID_SIZE - 1)) * 4.0f - 2.0f;
            float z0 = (z / (float)(GRID_SIZE - 1)) * 4.0f - 2.0f;
            float x1 = ((x + 1) / (float)(GRID_SIZE - 1)) * 4.0f - 2.0f;
            float z1 = ((z + 1) / (float)(GRID_SIZE - 1)) * 4.0f - 2.0f;
            
            // Triangle 1
            (*vertices)[index*3+0] = x0; (*vertices)[index*3+1] = 0.0f; (*vertices)[index*3+2] = z0;
            (*vertices)[index*3+3] = x0; (*vertices)[index*3+4] = 0.0f; (*vertices)[index*3+5] = z1;
            (*vertices)[index*3+6] = x1; (*vertices)[index*3+7] = 0.0f; (*vertices)[index*3+8] = z0;
            index += 3;
            
            // Triangle 2
            (*vertices)[index*3+0] = x1; (*vertices)[index*3+1] = 0.0f; (*vertices)[index*3+2] = z0;
            (*vertices)[index*3+3] = x0; (*vertices)[index*3+4] = 0.0f; (*vertices)[index*3+5] = z1;
            (*vertices)[index*3+6] = x1; (*vertices)[index*3+7] = 0.0f; (*vertices)[index*3+8] = z1;
            index += 3;
        }
    }
    
    // Fill dummy colors, shader handles wave color
    for (int i = 0; i < num_vertices * 3; i++) {
        (*colors)[i] = 1.0f; 
    }
    
    return num_vertices;
}

int main(void) {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Animated 3D Viewer", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL core profile extensions dynamically
    LoadOpenGLExtensions();

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    GLuint programID = LoadShaders("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint TimeID = glGetUniformLocation(programID, "time");
    GLuint AnimModeID = glGetUniformLocation(programID, "anim_mode");

    // Cube vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f, -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f
    };

    static const GLfloat g_color_buffer_data[] = {
        0.58f,0.77f,0.01f, 0.60f,0.11f,0.43f, 0.32f,0.48f,0.84f,
        0.82f,0.56f,0.20f, 0.43f,0.60f,0.22f, 0.31f,0.74f,0.18f,
        0.59f,0.77f,0.76f, 0.55f,0.43f,0.73f, 0.35f,0.58f,0.15f,
        0.48f,0.59f,0.78f, 0.55f,0.86f,0.63f, 0.19f,0.54f,0.85f,
        0.01f,0.18f,0.57f, 0.77f,0.32f,0.97f, 0.40f,0.61f,0.11f,
        0.67f,0.97f,0.13f, 0.97f,0.57f,0.83f, 0.14f,0.61f,0.48f,
        0.99f,0.51f,0.06f, 0.94f,0.71f,0.59f, 0.54f,0.02f,0.97f,
        0.27f,0.31f,0.50f, 0.16f,0.62f,0.07f, 0.34f,0.85f,0.13f,
        0.05f,0.95f,0.04f, 0.71f,0.50f,0.34f, 0.78f,0.29f,0.73f,
        0.72f,0.64f,0.17f, 0.30f,0.45f,0.84f, 0.22f,0.58f,0.04f,
        0.51f,0.71f,0.33f, 0.05f,0.95f,0.12f, 0.39f,0.62f,0.36f,
        0.67f,0.21f,0.45f, 0.82f,0.88f,0.37f, 0.98f,0.09f,0.87f
    };

    GLuint VertexArrayID_Cube, vertexbuffer_cube, colorbuffer_cube;
    glGenVertexArrays(1, &VertexArrayID_Cube);
    glBindVertexArray(VertexArrayID_Cube);
    glGenBuffers(1, &vertexbuffer_cube);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glGenBuffers(1, &colorbuffer_cube);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    // Wave grid
    GLfloat* wave_vertices;
    GLfloat* wave_colors;
    int num_wave_vertices = generate_wave_grid(&wave_vertices, &wave_colors);
    
    GLuint VertexArrayID_Wave, vertexbuffer_wave, colorbuffer_wave;
    glGenVertexArrays(1, &VertexArrayID_Wave);
    glBindVertexArray(VertexArrayID_Wave);
    glGenBuffers(1, &vertexbuffer_wave);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_wave);
    glBufferData(GL_ARRAY_BUFFER, num_wave_vertices * 3 * sizeof(GLfloat), wave_vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &colorbuffer_wave);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_wave);
    glBufferData(GL_ARRAY_BUFFER, num_wave_vertices * 3 * sizeof(GLfloat), wave_colors, GL_STATIC_DRAW);

    printf("========================================\n");
    printf("Controls:\n");
    printf(" [1] View 3D Cube (Rotating)\n");
    printf(" [2] View 3D Wave (Procedural Grid)\n");
    printf(" [ESC] Quit\n");
    printf("========================================\n");

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        float time = glfwGetTime();
        glUniform1f(TimeID, time);
        glUniform1i(AnimModeID, current_anim_mode);

        mat4 Projection, View, Model, MVP;
        glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, Projection);
        
        vec3 eye = {4.0f, 3.0f, -3.0f};
        if (current_anim_mode == 1) {
            eye[0] = 5.0f * sin(time * 0.2f);
            eye[1] = 4.0f;
            eye[2] = 5.0f * cos(time * 0.2f);
        }
        
        vec3 center = {0.0f, 0.0f, 0.0f};
        vec3 up = {0.0f, 1.0f, 0.0f};
        glm_lookat(eye, center, up, View);
        
        glm_mat4_identity(Model);

        if (current_anim_mode == 0) {
            vec3 axis = {0.5f, 1.0f, 0.2f};
            glm_normalize(axis);
            glm_rotate(Model, time * 1.5f, axis);
        }

        glm_mat4_mul(Projection, View, MVP);
        glm_mat4_mul(MVP, Model, MVP);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (const GLfloat *)MVP);

        if (current_anim_mode == 0) {
            glBindVertexArray(VertexArrayID_Cube);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cube);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_cube);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glDrawArrays(GL_TRIANGLES, 0, 12*3);
        } else {
            glBindVertexArray(VertexArrayID_Wave);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_wave);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_wave);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glDrawArrays(GL_TRIANGLES, 0, num_wave_vertices);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    glDeleteBuffers(1, &vertexbuffer_cube);
    glDeleteBuffers(1, &colorbuffer_cube);
    glDeleteVertexArrays(1, &VertexArrayID_Cube);
    
    glDeleteBuffers(1, &vertexbuffer_wave);
    glDeleteBuffers(1, &colorbuffer_wave);
    glDeleteVertexArrays(1, &VertexArrayID_Wave);
    
    free(wave_vertices);
    free(wave_colors);
    
    glDeleteProgram(programID);

    glfwTerminate();
    return 0;
}
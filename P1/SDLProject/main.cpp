#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, playerMatrix, cake1Matrix, cake2Matrix, cake3Matrix, projectionMatrix;


float player_x = 0, player_y = 0;
bool right;
float cake1_x = -2, cake1_y = 1.5;
float cake2_x = 0, cake2_y = -1;
float cake3_x = 1.5, cake3_y = 1.3;
float cake_rotate1 = 0, cake_rotate2 = 0, cake_rotate3 = 0;


GLuint playerTextureID;
GLuint cakeTextureID;


GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}



void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);


    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    

    viewMatrix = glm::mat4(1.0f);
    playerMatrix = glm::mat4(1.0f);
    cake1Matrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);


    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    playerTextureID = LoadTexture("monster.png");
    cakeTextureID = LoadTexture("cupcake.png");
    
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}


float lastTicks = 0.0f;

void Update() {
    
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    if (player_x > 2) right = false;
    else if (player_x < -2) right = true;

    if (right) player_x += 1.0f * deltaTime;
    else player_x += -1.0f * deltaTime;

    cake_rotate1 += 45.0f * deltaTime;
    cake_rotate2 -= 30.0f * deltaTime;
    cake_rotate3 += 60.0f * deltaTime;

    playerMatrix = glm::mat4(1.0f);
    cake1Matrix = glm::mat4(1.0f);
    cake2Matrix = glm::mat4(1.0f);
    cake3Matrix = glm::mat4(1.0f);
    playerMatrix = glm::scale(playerMatrix, glm::vec3(2.5f, 2.5f, 1.0f));
    cake1Matrix = glm::scale(cake1Matrix, glm::vec3(1.5f, 1.5f, 1.0f));
    cake2Matrix = glm::scale(cake1Matrix, glm::vec3(1.8f, 1.8f, 1.0f));
    cake3Matrix = glm::scale(cake1Matrix, glm::vec3(1.2f, 1.2f, 1.0f));

    playerMatrix = glm::translate(playerMatrix, glm::vec3(player_x, player_y, 0.0f));
    cake1Matrix = glm::translate(cake1Matrix, glm::vec3(cake1_x, cake1_y, 0.0f));
    cake2Matrix = glm::translate(cake2Matrix, glm::vec3(cake2_x, cake2_y, 0.0f));
    cake3Matrix = glm::translate(cake3Matrix, glm::vec3(cake3_x, cake3_y, 0.0f));

    cake1Matrix = glm::rotate(cake1Matrix, glm::radians(cake_rotate1), glm::vec3(0.0f, 0.0f, 1.0f));
    cake2Matrix = glm::rotate(cake2Matrix, glm::radians(cake_rotate2), glm::vec3(0.0f, 0.0f, 2.0f));
    cake3Matrix = glm::rotate(cake3Matrix, glm::radians(cake_rotate3), glm::vec3(0.0f, 0.0f, 1.0f));
}


void drawPlayer() {
    program.SetModelMatrix(playerMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void drawCakes() {
    program.SetModelMatrix(cake1Matrix);
    glBindTexture(GL_TEXTURE_2D, cakeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(cake2Matrix);
    glBindTexture(GL_TEXTURE_2D, cakeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(cake3Matrix);
    glBindTexture(GL_TEXTURE_2D, cakeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    drawPlayer();
    drawCakes();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}



void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}

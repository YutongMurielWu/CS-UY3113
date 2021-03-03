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

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameStarts = false;
bool gameOver();

ShaderProgram program;
glm::mat4 viewMatrix, ballMatrix, projectionMatrix, player1Matrix, player2Matrix;

glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
glm::vec3 player1_position = glm::vec3(-5, 0, 0);
glm::vec3 player1_movement = glm::vec3(0, 0, 0);
glm::vec3 player2_position = glm::vec3(5, 0, 0);
glm::vec3 player2_movement = glm::vec3(0, 0, 0);

float player_speed = 5.0f;
float ball_speed = 3.0f;
float pw = 0.5f, ph = 2.0f;
float bw = 0.5f, bh = 0.5f;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}


void ProcessInput() {
    player1_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                if (!gameStarts) {
                    gameStarts = true;
                    ball_movement.x = 0.8f;
                    ball_movement.y = 1.0f;
                    break;
                }
            }
        }
    }

    if (!gameOver()) {
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_W]) {
            player1_movement.y = 1.0f;
        }
        else if (keys[SDL_SCANCODE_S]) {
            player1_movement.y = -1.0f;
        }
        if (keys[SDL_SCANCODE_UP]) {
            player2_movement.y = 1.0f;
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            player2_movement.y = -1.0f;
        }

        if (glm::length(player1_movement) > 1.0f) {
            player1_movement = glm::normalize(player1_movement);
        }
        if (glm::length(player2_movement) > 1.0f) {
            player2_movement = glm::normalize(player2_movement);
        }
    }
}

void paddleMeetsWall() {
    if (player1_position.y + ph / 2.0f > 3.75f)
        player1_position.y = 2.75;
    if (player1_position.y - ph / 2.0f < -3.75f)
        player1_position.y = -2.75;

    if (player2_position.y + ph / 2.0f > 3.75f)
        player2_position.y = 2.75;
    if (player2_position.y - ph / 2.0f < -3.75f)
        player2_position.y = -2.75;
}

void ballMeetsWall() {
    if (ball_position.y + bh / 2.0f > 3.75f) {
        ball_position.y = 3.5;
        ball_movement.y = -(ball_movement.y);
    }
    if (ball_position.y - bh / 2.0f < -3.75f) {
        ball_position.y = -3.5;
        ball_movement.y = -(ball_movement.y);
    }
}

void ballMeetsPaddle() {

    float xdist1 = fabs(ball_position.x - player1_position.x) - ((pw + bw) / 2.0f);
    float ydist1 = fabs(ball_position.y - player1_position.y) - ((ph + bh) / 2.0f);

    float xdist2 = fabs(ball_position.x - player2_position.x) - ((pw + bw) / 2.0f);
    float ydist2 = fabs(ball_position.y - player2_position.y) - ((ph + bh) / 2.0f);

    if ((xdist1 < 0 && ydist1 < 0) || (xdist2 < 0 && ydist2 <0)) {
        ball_movement.x = -ball_movement.x;
        //ball_movement.y = -ball_movement.y;
    }

}

bool gameOver() {
    if ((ball_position.x + bw / 2.0f > 5) || (ball_position.x - bw / 2.0f < -5))
        return true;
    else return false;
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    

    ballMatrix = glm::mat4(1.0f);
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);

    paddleMeetsWall();
    ballMeetsWall();
    
    // Add (direction * units per second * elapsed time)
    player1_position += player1_movement * player_speed * deltaTime;
    player2_position += player2_movement * player_speed * deltaTime;
    ball_position += ball_movement * ball_speed * deltaTime;

    player1Matrix = glm::translate(player1Matrix, player1_position);
    player2Matrix = glm::translate(player2Matrix, player2_position);
    ballMatrix = glm::translate(ballMatrix, ball_position);

    player1Matrix = glm::scale(player1Matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    player2Matrix = glm::scale(player2Matrix, glm::vec3(1.0f, 4.0f, 1.0f));

    ballMeetsPaddle();

    if (gameOver()) {
        //gameIsRunning = false;
        ball_movement = glm::vec3(0);
    }
    
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    program.SetModelMatrix(ballMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(player2Matrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    program.SetModelMatrix(player1Matrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);

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

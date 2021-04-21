#include "Level3.h"
#include <SDL_mixer.h>
#define LEVEL3_WIDTH 27
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 3

Mix_Chunk* failure2;
Mix_Chunk* success;

unsigned int level3_data[] =
{
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 63,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 63,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 63,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16, 16, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 63,
     12, 12, 12, 12, 12, 12, 12, 12, 16, 16, 12, 12, 17, 17, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 63,
     12, 12, 12, 12, 12, 12, 12, 12, 17, 17, 12, 12, 17, 17, 16, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16, 16, 16,
     16, 16, 12, 16, 12, 12, 16, 16, 17, 17, 12, 12, 17, 17, 17, 16, 16, 16, 16, 12, 12, 12, 12, 12, 17, 17, 17,
     17, 17, 12, 17, 12, 12, 17, 17, 17, 17, 12, 12, 17, 17, 17, 17, 17, 17, 17, 16, 12, 12, 12, 16, 17, 17, 17
};


void Level3::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    
    
    
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 14, 7);


    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->PlayerInitialize();

    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
    }

    state.enemies[0].position = glm::vec3(13, 0, 0);
    state.enemies[0].initialPosition = glm::vec3(13, 0, 0);
    state.enemies[0].speed = 0.5;
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].jumpPower = 3.5;
    state.enemies[0].movement = glm::vec3(1, 0, 0);

    state.enemies[1].position = glm::vec3(16.5, 0, 0);
    state.enemies[1].initialPosition = glm::vec3(16.5, 0, 0);
    state.enemies[1].speed = 0.5;
    state.enemies[1].aiType = PATROLLER;
    state.enemies[1].movement = glm::vec3(-1, 0, 0);

    state.enemies[2].position = glm::vec3(3, 0, 0);
    state.enemies[2].initialPosition = glm::vec3(3, 0, 0);
    state.enemies[2].aiType = JUMPER;
    state.enemies[2].jumpPower = 3;

    failure2 = Mix_LoadWAV("failure.wav");
    success = Mix_LoadWAV("success.wav");
}

void Level3::Update(float deltaTime) {
    if (state.lose) return;
    if (state.win) return;

    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++)
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

    if (state.player->position.x >= 26) {
        state.win = true;
        Mix_PlayChannel(-1, success, 0);
    }
    if (state.player->position.y <= -15 && state.player->lives > 0) {
        state.player->lives -= 1;
        if (state.player->lives > 0) {
            state.player->PlayerInitialize();
            Mix_PlayChannel(-1, failure2, 0);
        }
    }
    if (state.player->lives == 0) {
        state.lose = true;
        Mix_PlayChannel(-1, failure2, 0);
    }
}

void Level3::Render(ShaderProgram* program) {
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++)
        state.enemies[i].Render(program);
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "lives: " + std::to_string(state.player->lives), 0.5f, -0.3f,
        glm::vec3(state.player->position.x - 0.75, state.player->position.y + 1, 0));
    if (state.lose) {
        if(state.player->position.x >= 5)
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.8f, -0.25f, glm::vec3(state.player->position.x - 2, -3, 0));
        else
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.8f, -0.25f, glm::vec3(3, -3, 0));
    }
        
    if (state.win)
        Util::DrawText(program, fontTextureID, "YOU WIN!", 0.8f, -0.25f, glm::vec3(state.player->position.x - 2, -3, 0));
}

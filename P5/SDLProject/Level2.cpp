#include "Level2.h"
#include <SDL_mixer.h>
#define LEVEL2_WIDTH 20
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 2

Mix_Chunk* failure;

unsigned int level2_data[] =
{
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 64,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 64,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 64,
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 64,
     12, 12, 12, 12, 12, 12, 12, 12, 12,  1, 12, 12, 12, 12, 12, 12, 12, 12, 12, 64,
     12, 12, 12, 12, 12, 12, 12, 12,  1,  3, 12,  1,  1,  1,  1, 12, 12, 12, 12, 64,
      1,  1,  1,  1, 12, 12,  1,  1,  3,  3, 12,  3,  3,  3,  3,  1,  1,  1,  1, 64,
      3,  3,  3,  3, 12, 12,  3,  3,  3,  3, 12,  3,  3,  3,  3,  3,  3,  3,  3,  1,
};


void Level2::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 14, 7);

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->PlayerInitialize();

    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
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

    state.enemies[1].position = glm::vec3(7.5, 0, 0);
    state.enemies[1].initialPosition = glm::vec3(7.5, 0, 0);
    state.enemies[1].speed = 0.5;
    state.enemies[1].aiType = PATROLLER;
    state.enemies[1].movement = glm::vec3(-1, 0, 0);

    failure = Mix_LoadWAV("failure.wav");

    
}
void Level2::Update(float deltaTime) {
    if (state.lose) return;

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++)
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);

    if (state.player->position.x <= 0.5) state.player->position.x = 0.5;
    if (state.player->position.x >= 19)
        state.nextScene = 3;
    if (state.player->position.y <= -15 && state.player->lives > 0) {
        state.player->lives -= 1;
        if (state.player->lives > 0) {
            state.player->PlayerInitialize();
            Mix_PlayChannel(-1, failure, 0);
        }
    }
    if (state.player->lives == 0) {
        state.lose = true;
        Mix_PlayChannel(-1, failure, 0);
    }
}

void Level2::Render(ShaderProgram* program) {
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++)
        state.enemies[i].Render(program);
    state.map->Render(program);
    state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "lives: " + std::to_string(state.player->lives), 0.5f, -0.3f,
        glm::vec3(state.player->position.x - 0.75, state.player->position.y + 1, 0));
    if (state.lose) {
        if (state.player->position.x >= 5)
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.8f, -0.25f, glm::vec3(state.player->position.x - 2, -3, 0));
        else
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.8f, -0.25f, glm::vec3(3, -3, 0));
    }

}

#include "Level1.h"
#include <SDL_mixer.h>
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

Mix_Chunk* failure0;

unsigned int level1_data[] =
{
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 65,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 65,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 65,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 65,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 65,
	 12, 12, 12, 12, 12, 12, 12, 12, 12,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3
};


void Level1::Initialize() {

    state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 14, 7);

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->PlayerInitialize();
    state.player->lives = 3;


    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");

    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(10, 0, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].speed = 0.5;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    failure0 = Mix_LoadWAV("failure.wav");
    
}

void Level1::Update(float deltaTime) {
    if (state.lose) return;
    state.enemies->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
	state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    
    if (state.player->position.x <= 0.5) state.player->position.x = 0.5;
    if (state.player->position.x >= 12.7)
        state.nextScene = 2;
    if (state.player->position.y <= -15 && state.player->lives > 0) {
        state.player->lives -= 1;
        if (state.player->lives > 0) {
            state.player->PlayerInitialize();
            Mix_PlayChannel(-1, failure0, 0);
        }
    }
    if (state.player->lives == 0) {
        state.lose = true;
        Mix_PlayChannel(-1, failure0, 0);
    }

}
void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);
    state.enemies->Render(program);
	state.player->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "lives: " + std::to_string(state.player->lives), 0.5f, -0.3f, 
        glm::vec3(state.player->position.x-0.75, state.player->position.y +1, 0));
    if (state.lose) {
        if (state.player->position.x >= 5)
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.8f, -0.25f, glm::vec3(state.player->position.x - 2, -3, 0));
        else
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.8f, -0.25f, glm::vec3(3, -3, 0));
    }
}

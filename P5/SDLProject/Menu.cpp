#include "Menu.h"

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

unsigned int menu_data[] =
{
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 65, 64, 63, 65, 64, 63, 65, 64, 63, 65, 64, 12, 12, 12,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3
};

void Menu::Initialize() {

    state.nextScene = -1;
	GLuint mapTextureID = Util::LoadTexture("platformPack_tilesheet.png");
	state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 14, 7);
	state.player = new Entity();
}
void Menu::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, NULL, 0, state.map);
}
void Menu::Render(ShaderProgram* program) {

	state.map->Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");
	Util::DrawText(program, fontTextureID, "'LESS SUPER THAN MARIO'", 0.7f, -0.3f, glm::vec3(0.6, -2, 0));
    Util::DrawText(program, fontTextureID, "PRESS ENTER TO START", 0.5f, -0.25f, glm::vec3(2.5, -4, 0));
}

#include "Demo.h"

using namespace Engine;

Demo::Demo(Setting* setting) :Game(setting)
{

}


Demo::~Demo()
{
	delete platformTexture;
	delete playerTexture;
	//delete dotTexture;
	delete platformSprite1;	
	delete platformSprite2;
	delete platformSprite3;
	delete playerSprite;
	//delete dotSprite1;
	//delete dotSprite2;
	//delete dotSprite3;
	//delete dotSprite4;
	delete music;
	delete text;
}

void Demo::Init()
{
	backgroundTexture = new Texture("Background.png");
	backgroundSprite = new Sprite(backgroundTexture, defaultSpriteShader, defaultQuad);
	backgroundSprite->SetPosition(0, 0);

	platformTexture = new Texture("Platform.png");
	platformSprite1 = new Sprite(platformTexture, defaultSpriteShader, defaultQuad);
	platformSprite1->SetSize(300, 30);
	platformSprite1->SetPosition((setting->screenWidth * 0.5f - 100) - (platformSprite1->GetScaleWidth() * 0.5f),
		(setting->screenHeight * 0.5f + 100) - (platformSprite1->GetScaleHeight() * 0.5f));

	platformSprite2 = new Sprite(platformTexture, defaultSpriteShader, defaultQuad);
	platformSprite2->SetSize(700, 30);
	platformSprite2->SetPosition((setting->screenWidth * 0.5f),
		(setting->screenHeight * 0.5 - 100) - (platformSprite2->GetScaleHeight() * 0.5f));

	platformSprite3 = new Sprite(platformTexture, defaultSpriteShader, defaultQuad);
	platformSprite3->SetSize(500, 30);
	platformSprite3->SetPosition((setting->screenWidth * 0.5f - 100) - (platformSprite3->GetScaleWidth()),
		(setting->screenHeight * 0.5f - 225) - (platformSprite3->GetScaleHeight() * 0.5f));

	// Create Player
	playerTexture = new Texture("player.png");
	playerSprite = new Sprite(playerTexture, defaultSpriteShader, defaultQuad);

	// Set player sprite position 
	// 0,0 is at bottom left of the screen and width, height is at top right	
	playerSprite->SetPosition(900,0);
	// set num cols in spritesheet
	playerSprite->SetNumXFrames(8);
	// set num rows in spritesheet
	playerSprite->SetNumYFrames(1);
	// Set animation speed more duration the speed will become slower
	playerSprite->SetAnimationDuration(150);
	playerSprite->SetScale(0.3f);
	// Add Animation name, start frame index, end frame index
	playerSprite->AddAnimation("idle", 0, 4);
	playerSprite->AddAnimation("walk", 5, 8);
	// If frame size / sprite size is contain blank space or padding then you must resize the bounding box
	// or collision shape to improve collision detection accuracy
	playerSprite->SetBoundingBoxSize(playerSprite->GetScaleWidth() - (16 * playerSprite->GetScale()),
		playerSprite->GetScaleHeight() - (4 * playerSprite->GetScale()));
	
	// This dot sprite is for visual debugging (to see the actual bounding box) 
	/*dotTexture = new Texture("dot.png");
	dotSprite1 = new Sprite(dotTexture, defaultSpriteShader, defaultQuad);
	dotSprite2 = new Sprite(dotTexture, defaultSpriteShader, defaultQuad);
	dotSprite3 = new Sprite(dotTexture, defaultSpriteShader, defaultQuad);
	dotSprite4 = new Sprite(dotTexture, defaultSpriteShader, defaultQuad);*/

	// Add input mapping 
	inputManager->AddInputMapping("Jump", SDLK_w);
	inputManager->AddInputMapping("Walk Right", SDLK_d);
	inputManager->AddInputMapping("Walk Left", SDLK_a);
	inputManager->AddInputMapping("Walk Right", SDLK_RIGHT);
	inputManager->AddInputMapping("Walk Left", SDLK_LEFT);
	inputManager->AddInputMapping("Jump", SDLK_SPACE);
	inputManager->AddInputMapping("Quit", SDLK_ESCAPE);

	// Playing music
	music = new Music("bgm.ogg");
	music->SetVolume(30);
	music->Play(true);

	sound = new Sound("stepwood_1.wav");
	sound->SetVolume(80);

	text = new Text("lucon.ttf", 14, defaultTextShader);
	text->SetScale(1.0f);
	text->SetColor(255, 255, 255);
	text->SetPosition(0, setting->screenHeight - (text->GetFontSize() * text->GetScale()));

}

void Demo::Update()
{
	// If user press "Quit" key then exit
	if (inputManager->IsKeyReleased("Quit")) {
		state = State::EXIT;
		return;
	}

	float xspeed = 0, yspeed = 0, gravity = 0.1;

	playerSprite->PlayAnim("idle");

	// Move player sprite using keyboard
	vec2 playerPos = playerSprite->GetPosition();
	
	// Set ground position
	float static groundPos = playerPos.y;

	if (inputManager->IsKeyPressed("Walk Right")) {
		xspeed = 0.1f;
		playerSprite->PlayAnim("walk");
		playerSprite->SetPosition(playerPos.x + xspeed * GetGameTime(),
			playerPos.y);
		playerSprite->SetFlipHorizontal(false);
		if (!sound->IsPlaying())
			sound->Play(false);
	}

	if (inputManager->IsKeyPressed("Walk Left")) {
		xspeed = -0.1f;
		playerSprite->PlayAnim("walk");
		playerSprite->SetPosition(playerPos.x + xspeed * GetGameTime(),
			playerPos.y);
		playerSprite->SetFlipHorizontal(true);
		if (!sound->IsPlaying())
			sound->Play(false);
	}

	if (inputManager->IsKeyReleased("Jump")) {
		yspeed = 200.0f;
		bool flip = playerSprite->GetFlipHorizontal();

		// Move forward when jumping
		/*if (flip = false) {
			xspeed = yspeed / 2;
		}
		if (flip = true) {
			xspeed = -(yspeed / 2);
		}*/
		
		playerSprite->SetPosition(playerPos.x,
			playerPos.y + yspeed + gravity * GetGameTime());
	}

	// Set Player back to ground after jumping
	if (playerPos.y > groundPos) {
		SDL_Delay(100);
		playerPos.y = groundPos;
		playerSprite->SetPosition(playerPos.x,
			playerPos.y);
	}

	// Update player sprite animation
	playerSprite->Update(GetGameTime());

	BoundingBox* bb = playerSprite->GetBoundingBox();

	//Collision still not detected
	/*if (platformSprite1->GetBoundingBox()->CollideWith(bb) ||
		platformSprite2->GetBoundingBox()->CollideWith(bb) ||
		platformSprite3->GetBoundingBox()->CollideWith(bb)) {
		//revert position if collided
		playerSprite->SetPosition(playerPos.x,
			playerPos.y);
	}*/

	// Move player to right side after moving out the left screen
	if (playerPos.x < 0) {
		playerSprite->SetPosition(setting->screenWidth - playerSprite->GetScaleWidth(),
			playerPos.y);
	}
	
	// Move player to left side  after moving out the right screen
	if (playerPos.x > setting->screenWidth - playerSprite->GetScaleWidth()) {
		playerSprite->SetPosition(0,
			playerPos.y);
	}
		
	bool isCollided = platformSprite1->GetBoundingBox()->CollideWith(playerSprite->GetBoundingBox()) ||
		platformSprite2->GetBoundingBox()->CollideWith(playerSprite->GetBoundingBox()) || 
		platformSprite3->GetBoundingBox()->CollideWith(playerSprite->GetBoundingBox());

	text->SetText((isCollided ? "[Collision = Yes]" : "[Collision = No]") +
		("[FPS = " + to_string(currentFrameRate)) +
		"] Press A-D-Left-Right to Move Player & W-Space to Jump, Escape to exit" );


	// Move automatically
	// Not working, only walk 1 time

	/*if (playerPos.x >= 0) {
		playerSprite->PlayAnim("walk");
		playerSprite->MoveLeft(playerPos.x, GetGameTime());
		playerSprite->SetPosition(playerPos.x,
			playerPos.y);
		if (!sound->IsPlaying())
			sound->Play(false);
	}

	if (playerPos.x <= setting->screenWidth - playerSprite->GetScaleWidth()) {
		playerSprite->PlayAnim("walk");
		playerSprite->MoveRight(playerPos.x, GetGameTime());
		playerSprite->SetPosition(playerPos.x,
			playerPos.y);
		if (!sound->IsPlaying())
			sound->Play(false);
	}*/

	//Render shape for debug
	/*dotSprite1->SetPosition(bb->GetVertices()[0].x - (0.5f * dotSprite1->GetScaleWidth()),
		bb->GetVertices()[0].y - (0.5f * dotSprite1->GetScaleHeight()));
	dotSprite2->SetPosition(bb->GetVertices()[1].x - (0.5f * dotSprite2->GetScaleWidth()), 
		bb->GetVertices()[1].y - (0.5f * dotSprite2->GetScaleHeight()));
	dotSprite3->SetPosition(bb->GetVertices()[2].x - (0.5f * dotSprite3->GetScaleWidth()), 
		bb->GetVertices()[2].y - (0.5f * dotSprite3->GetScaleHeight()));
	dotSprite4->SetPosition(bb->GetVertices()[3].x - (0.5f * dotSprite4->GetScaleWidth()), 
		bb->GetVertices()[3].y - (0.5f * dotSprite3->GetScaleHeight()));*/
}

void Demo::Render()
{
	backgroundSprite->Draw();	
	platformSprite1->Draw();
	platformSprite2->Draw();
	platformSprite3->Draw();
	playerSprite->Draw();
	/*dotSprite1->Draw();
	dotSprite2->Draw();
	dotSprite3->Draw();
	dotSprite4->Draw();*/
	text->Draw();
}

int main(int argc, char** argv) {
	Setting* setting = new Setting();
	setting->windowTitle = "Amog Us";
	setting->screenWidth = 1024;
	setting->screenHeight = 768;
	setting->windowFlag = WindowFlag::WINDOWED;
	Game* game = new Demo(setting);
	game->Run();
	delete setting;
	delete game;

	return 0;
}

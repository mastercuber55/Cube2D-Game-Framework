/********************************************************************************************************************************************************
*
*   Cube2D Game Framework v1.2 - A simple and easy-to-use game emgine.
*
*   FEATURES:
*       - NO forced external dependencies, expect raylib.
*       - Multiplatform: My guess is that its just as good as raylib.
*       - Written in plain C code (C99) in PascalCase notation.
*
*   NOTES:
*		- In just a SINGLE file before including #define CGE_IMPLEMENTATION
*		- Macros a prefixed with CGE while all functions are prefixed with r
* 		
*   DEPENDENCIES:
*       [raylib] for almost everything. 
*
*   OPTIONAL DEPENDENCIES:
*		[raygui] for GuiWIndow
* 	
*   LICENSE: MIT
*
* 	MIT License
*
* 	Copyright (c) 2023 Cube Nerd (@mastercuber55)
* 
* 	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
* 	files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
* 	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
* 	is furnished to do so, subject to the following conditions:
* 
* 	The above copyright notice, this permission notice shall be included in all copies or substantial portions of the Software.
* 
* 	THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
* 	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
* 	BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT 
* 	OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
********************************************************************************************************************************************************/

#ifndef CUBE2D_GAME_FRAMEWORK
#define CUBE2D_GAME_FRAMEWORK "1.2"

#ifndef CGE_DEFAULT_SRC
#	define CGE_DEFAULT_SRC { 0, 0, -1, -1 }
#endif // CGE_SRC_DEFAULT

#ifndef CGE_SCENE_DEFAULT_COLOR
#	define CGE_SCENE_DEFAULT_COLOR BLACK
#endif

#ifndef CGE_DEFAULT_FPS
#	define CGE_DEFAULT_FPS 60
#endif

#include <raylib.h>

#ifdef CGE_RAYGUI
#	ifdef CGE_IMPL
#		define RAYGUI_IMPLEMENTATION
#	endif
#	include <raygui.h>
#endif // CGE_RAYGUI

#include <functional>
#include <string>

namespace Engine {

//----------------------------------------------------------------------------------
// Structures Definitions (Module: Structures)
//----------------------------------------------------------------------------------

// Scene, Used for scene management.
struct Scene {

	Color BackgroundColor;
	bool KeepRunning;
	int CodeToReturn;

	Scene(Color BackgroundColor = CGE_SCENE_DEFAULT_COLOR);
	virtual bool ShouldClose();
	virtual int Run();
	virtual void Close(int CodeToReturn = 0);
	virtual void HandleEventsAndLogics();
	virtual void Draw();

	~Scene();
};

// Rect, Used for easier rectangle manipulation
struct Rect {
	
	float x, y, w, h;								
	double Rotation;

	std::string TextureFile;						
	Rectangle Source;								
	Color Tint;										

	Rect(float x, float y, float w, float h, Color Tint);
	Rect(Rectangle Destination, Color Tint);	
	Rect(										
		Rectangle Destination, 
		std::string TextureFile, Rectangle Source = CGE_DEFAULT_SRC
	);
	~Rect();

	void SetTextureFile(std::string TextureFile);	
	void Draw();									

	bool Colliding(Rect * Other);

	void SetPosition(Vector2 NewPosition);
	void operator=(Vector2 NewPosition);

	Vector2 GetCenter();							
	operator Vector2() const;						
	operator Rectangle() const;						
};

#ifdef CGE_RAYGUI
struct GuiWindow {

	bool Drag;
	bool Hidden;

	float x, y, w, h;
	Camera2D Cam;
	std::string Title;

	GuiWindow(Rectangle Destination, std::string Title);

	void HandleEventsAndLogics();
	void Draw(std::function<void(Vector2 Offset)> Function);
};
#endif // CGE_RAYGUI

//------------------------------------------------------------------------------------
// Initialization And Closing Functions (Module: Core)
//------------------------------------------------------------------------------------
//
void Init(int WindowWidth, int WindowHeight, std::string WindowTitle);
void Close();

//------------------------------------------------------------------------------------
// Tool Functions (Module: Tools)
//------------------------------------------------------------------------------------

unsigned int GetRectsCount();	
void WASDMovement(Rect * Object, float Speed);
bool AreColorSame(Color A, Color B);
Vector2 GetRandomPosition(Camera2D Cam);

}
#endif // RAY_GAME_FRAMEWORK
//------------------------------------------------------------------------------------
// CGE Implementation (Module: Implementation)
// No need to go any further you know.
//------------------------------------------------------------------------------------

#ifdef CGE_IMPL
#undef CGE_IMPL
#define P2E_IMPL

#include <unordered_map>
#include <set>
#include <algorithm>

//------------------------------------------------------------------------------------
// Variables (Module: Variables)
//------------------------------------------------------------------------------------

namespace Engine {

std::unordered_map<std::string, Texture> Textures;
unsigned int RectsCount = 0;

//----------------------------------------------------------------------------------
// Structures Definitions (Module: Structures)
//----------------------------------------------------------------------------------

Scene::Scene(Color BackgroundColor) {
	this->BackgroundColor = BackgroundColor;
	this->CodeToReturn = 0;
	this->KeepRunning = true;
}
void Scene::HandleEventsAndLogics() {}
void Scene::Draw() {}
void Scene::Close(int CodeToReturn) {
	this->KeepRunning = false;
	this->CodeToReturn = CodeToReturn;
}
int Scene::Run() {

	while(!this->ShouldClose()) {
		
		//------------------------------------------------------------------------------------
		// Event And Logic Handling
		//------------------------------------------------------------------------------------
		this->HandleEventsAndLogics();
		//------------------------------------------------------------------------------------
		//  Drawing
		//------------------------------------------------------------------------------------		
		BeginDrawing();
			ClearBackground(this->BackgroundColor);
			this->Draw();
		EndDrawing();
	}
	return this->CodeToReturn;
}
bool Scene::ShouldClose() { 
	return !KeepRunning || WindowShouldClose();
}
Scene::~Scene() {}

Rect::Rect(float x, float y, float w, float h, Color Tint) : Rect::Rect({x, y, w, h}, Tint) {}
Rect::Rect(Rectangle Rec, Color Tint) {

	this->x = Rec.x, this->y = Rec.y,				
	this->w = Rec.width, this->h = Rec.height,		
	this->Rotation = 0.0,							
	// this->TextureFile = "NO NEED";					
	this->Source = { 0, 0, this->w, this->h },		
	this->Tint = Tint;								

}
Rect::Rect(Rectangle Destination, std::string TextureFile, Rectangle Source) : Rect(Destination, WHITE) {
	
	if(Source.width == -1 && Source.height == -1)   
		this->Source.width = Destination.width, this->Source.height = Destination.height;
	else
		this->Source = Source; 
	
	this->SetTextureFile(TextureFile);
}
Rect::~Rect() {}
void Rect::SetTextureFile(std::string TextureFile) {

	this->TextureFile = TextureFile; 						
	if(Textures.find(TextureFile) != Textures.end()) return;
	Textures[TextureFile] = LoadTexture(("Resources/GFX/" + TextureFile).c_str());
}
void Rect::Draw() {

	(this->TextureFile.empty()) ? 
		DrawRectangle(this->x, this->y, this->w, this->h, this->Tint)
	:	
		DrawTexturePro(
			Textures[this->TextureFile], 
			this->Source, 
			(Rectangle){ this->x + this->w / 2.0f, this->y + this->h / 2.0f, this->w, this->h }, 
			(Vector2){ this->w / 2.0f, this->h / 2.0f }, 
			this->Rotation, 
			this->Tint
		);
}
bool Rect::Colliding(Rect * Other) {
	return CheckCollisionRecs(*this, *Other);
}
void Rect::SetPosition(Vector2 Position) {
	this->x = Position.x, this->y = Position.y;
}
void Rect::operator=(Vector2 NewPosition) {
	this->SetPosition(NewPosition);
}
Vector2 Rect::GetCenter() {
	return {this->x + this->w / 2.0f, this->y + this->h / 2.0f};
}
Rect::operator Vector2() const {
	return { this->x, this->y };
}
Rect::operator Rectangle() const {
	return { this->x, this->y, this->w, this->h };
}

#ifdef RAYGUI_H

GuiWindow::GuiWindow(Rectangle Destination, std::string Title) {
	this->x = Destination.x,
	this->y = Destination.y,
	this->w = Destination.width,
	this->h = Destination.height;

	this->Title = Title;

	this->Hidden = true;
	this->Drag = false;
}

void GuiWindow::HandleEventsAndLogics() {
	if(this->Hidden) return;
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !Drag && CheckCollisionPointRec(GetMousePosition(), { x, y, w, 20.0f })) Drag = true;
	if(Drag) {

		Vector2 MouseDelta = GetMouseDelta();

		this->x += MouseDelta.x;
		this->y += MouseDelta.y;

		if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) Drag = false;
	}
}

void GuiWindow::Draw(std::function<void(Vector2 Offset)> Function) {
	if(this->Hidden) return;
	this->Hidden = GuiWindowBox((Rectangle){ x, y, w, h }, Title.c_str());
	
	Function({ this->x, this->y + 20.0f });
}
#endif

void Init(int WindowWidth, int WindowHeight, std::string Title) {

	TraceLog(LOG_INFO, "Initializing Cube2D %s", CUBE2D_GAME_FRAMEWORK); 

	InitWindow(WindowWidth, WindowHeight, Title.c_str());
	InitAudioDevice();				

	SetExitKey(KEY_NULL);							
	SetTargetFPS(CGE_DEFAULT_FPS);					
}
void Close() {


	CloseAudioDevice();
	CloseWindow();

	TraceLog(LOG_INFO, "Cube2D closed successfully");
}

//------------------------------------------------------------------------------------
// Tool Functions (Module: Tools)
//------------------------------------------------------------------------------------

void WASDMovement(Rect * Object, float Speed) {
	if(IsKeyDown(KEY_W)) Object->y += -Speed;
	if(IsKeyDown(KEY_A)) Object->x += -Speed;
	if(IsKeyDown(KEY_S)) Object->y += +Speed;
	if(IsKeyDown(KEY_D)) Object->x += +Speed;
}

unsigned int GetRectsCount() {
	return RectsCount;
}
bool AreColorSame(Color A, Color B) {
	return ColorToInt(A) == ColorToInt(B);
}
Vector2 GetRandomPosition(Camera2D Cam) {
	return {
		GetRandomValue(Cam.target.x - Cam.offset.x, Cam.target.x + Cam.offset.x),
		GetRandomValue(Cam.target.y - Cam.offset.y, Cam.target.y + Cam.offset.y)
	};
}

}

#endif // CGE_IMPLEMENTATION
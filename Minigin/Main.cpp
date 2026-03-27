#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"
#include "Debugger.h"
#include "EventManager.h"

#include <filesystem>
#include "Components/TextureRendererComponent.h"
#include "Components/TextRendererComponent.h"
#include "Components/FPSCounterComponent.h"
#include "Components/SpinnerComponent.h"
#include "Components/ScoreComponent.h"
#include "ResourceManager.h"
#include "Components/ObjectMoveComponent.h"
#include "Commands/MovementInputCommand.h"
#include "InputManager.h"

#include "Controller/GamepadButton.h"

namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	Debugger::GetInstance().AttachToScene(&scene);
	//Debugger::GetInstance().LogError("some arbitrary error!");
	//Debugger::GetInstance().LogWarning("this shouldn't happen - I'll take care of it");
	//Debugger::GetInstance().LogDebug("you've reached line 26! nice!!");

	/*auto go = std::make_unique<dae::GameObject>("Background");
	std::string BGName{ "background.png" };
	go->AddComponent<dae::TextureRenderComponent>(BGName);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>("Logo");
	BGName = "logo.png";
	go->AddComponent<dae::TextureRenderComponent>(BGName);
	go->SetPosition(358, 180);
	scene.Add(std::move(go));
	
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = std::make_unique<dae::GameObject>("Text_Assignment");
	SDL_Color col{ 255,255,255,255 };
	to->AddComponent<dae::TextRenderComponent>("Programming 4 Assignment", col, font);
	to->SetPosition(292, 20);
	scene.Add(std::move(to));

	to = std::make_unique<dae::GameObject>("Text_FPS");
	to->AddComponent<dae::TextRenderComponent>("FPS: 0", col, font);
	to->AddComponent<dae::FPSCounterComponent>();
	to->SetPosition(10, 10);
	scene.Add(std::move(to));

	auto ply1 = std::make_unique<dae::GameObject>("Player1");
	ply1->AddComponent<dae::TextureRenderComponent>("Fygar_Idle.png");
	ply1->SetPosition(200, 200);
	auto move1 = ply1->AddComponent<dae::ObjectMoveComponent>();
	auto health1 = ply1->AddComponent<dae::HealthComponent>(3);
	auto score1 = ply1->AddComponent<dae::ScoreComponent>();

	auto CommandUp = std::make_unique<dae::MovementInputCommand>(move1);
	CommandUp->SetDirection({ 0, -1 });
	CommandUp->SetSpeed(100);
	dae::InputManager::GetInstance().BindKey(SDLK_W, dae::InputManager::InputType::Down, "MoveUp_P1", std::move(CommandUp));

	auto CommandDown = std::make_unique<dae::MovementInputCommand>(move1);
	CommandDown->SetDirection({ 0, 1 });
	CommandDown->SetSpeed(100);
	dae::InputManager::GetInstance().BindKey(SDLK_S, dae::InputManager::InputType::Down, "MoveDown_P1", std::move(CommandDown));

	auto CommandLeft = std::make_unique<dae::MovementInputCommand>(move1);
	CommandLeft->SetDirection({ -1, 0 });
	CommandLeft->SetSpeed(100);
	dae::InputManager::GetInstance().BindKey(SDLK_A, dae::InputManager::InputType::Down, "MoveLeft_P1", std::move(CommandLeft));

	auto CommandRight = std::make_unique<dae::MovementInputCommand>(move1);
	CommandRight->SetDirection({ 1, 0 });
	CommandRight->SetSpeed(100);
	dae::InputManager::GetInstance().BindKey(SDLK_D, dae::InputManager::InputType::Down, "MoveRight_P1", std::move(CommandRight));

	// Exercise 1: Player death command (test with K key)
	auto DeathCommand1 = std::make_unique<dae::PlayerDeathCommand>(health1);
	dae::InputManager::GetInstance().BindKey(SDLK_K, dae::InputManager::InputType::Pressed, "Die_P1", std::move(DeathCommand1));

	// Exercise 2: Score command (test with P key)
	auto ScoreCommand1 = std::make_unique<dae::AddScoreCommand>(score1, 10);
	dae::InputManager::GetInstance().BindKey(SDLK_P, dae::InputManager::InputType::Pressed, "Score_P1", std::move(ScoreCommand1));

	scene.Add(std::move(ply1));

	auto ply2 = std::make_unique<dae::GameObject>("Player2");
	ply2->AddComponent<dae::TextureRenderComponent>("Pooka_Idle.png");
	ply2->SetPosition(200, 400);
	auto move2 = ply2->AddComponent<dae::ObjectMoveComponent>();
	auto health2 = ply2->AddComponent<dae::HealthComponent>(3);
	auto score2 = ply2->AddComponent<dae::ScoreComponent>();

	auto CommandUp2 = std::make_unique<dae::MovementInputCommand>(move2);
	CommandUp2->SetDirection({ 0, -1 });
	CommandUp2->SetSpeed(200);
	dae::InputManager::GetInstance().BindButton(0, dae::GamepadButton::DPadUp, dae::InputManager::InputType::Down, "MoveUp_P2", std::move(CommandUp2));

	auto CommandDown2 = std::make_unique<dae::MovementInputCommand>(move2);
	CommandDown2->SetDirection({ 0, 1 });
	CommandDown2->SetSpeed(200);
	dae::InputManager::GetInstance().BindButton(0, dae::GamepadButton::DPadDown, dae::InputManager::InputType::Down, "MoveDown_P2", std::move(CommandDown2));

	auto CommandLeft2 = std::make_unique<dae::MovementInputCommand>(move2);
	CommandLeft2->SetDirection({ -1, 0 });
	CommandLeft2->SetSpeed(200);
	dae::InputManager::GetInstance().BindButton(0, dae::GamepadButton::DPadLeft, dae::InputManager::InputType::Down, "MoveLeft_P2", std::move(CommandLeft2));

	auto CommandRight2 = std::make_unique<dae::MovementInputCommand>(move2);
	CommandRight2->SetDirection({ 1, 0 });
	CommandRight2->SetSpeed(200);
	dae::InputManager::GetInstance().BindButton(0, dae::GamepadButton::DPadRight, dae::InputManager::InputType::Down, "MoveRight_P2", std::move(CommandRight2));


	auto DeathCommand2 = std::make_unique<dae::PlayerDeathCommand>(health2);
	dae::InputManager::GetInstance().BindButton(0, dae::GamepadButton::X, dae::InputManager::InputType::Pressed, "Die_P2", std::move(DeathCommand2));

	auto ScoreCommand2 = std::make_unique<dae::AddScoreCommand>(score2, 10);
	dae::InputManager::GetInstance().BindButton(0, dae::GamepadButton::A, dae::InputManager::InputType::Pressed, "Score_P2", std::move(ScoreCommand2));

	scene.Add(std::move(ply2));

	auto smallFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 18);
	SDL_Color whiteCol{ 255,255,255,255 };

	// Lives Display 1 - shows if Player 1 lives and health
	auto LivesDisplay1 = std::make_unique<dae::GameObject>("LivesDisplay1");
	LivesDisplay1->AddComponent<dae::TextRenderComponent>("Player1 Lives: 3/3 Health: 3/3", whiteCol, smallFont);
	LivesDisplay1->AddComponent<dae::LivesDisplayComponent>("Player1", health1, 3);
	LivesDisplay1->SetPosition(10, 90);
	scene.Add(std::move(LivesDisplay1));

	// Lives Display 2 - shows if Player 2 lives and health
	auto LivesDisplay2 = std::make_unique<dae::GameObject>("LivesDisplay2");
	LivesDisplay2->AddComponent<dae::TextRenderComponent>("Player2 Lives: 3/3 Health: 3/3", whiteCol, smallFont);
	LivesDisplay2->AddComponent<dae::LivesDisplayComponent>("Player2", health2, 3);
	LivesDisplay2->SetPosition(500, 90);
	scene.Add(std::move(LivesDisplay2));

	// Points Display 1 - observes Player1ScoreChanged events
	auto PointsDisplay1 = std::make_unique<dae::GameObject>("PointsDisplay1");
	PointsDisplay1->AddComponent<dae::TextRenderComponent>("Player1 Score: 0", whiteCol, smallFont);
	PointsDisplay1->AddComponent<dae::PointsDisplayComponent>("Player1");
	PointsDisplay1->SetPosition(10, 110);
	scene.Add(std::move(PointsDisplay1));

	// Points Display 2 - observes Player2ScoreChanged events
	auto PointsDisplay2 = std::make_unique<dae::GameObject>("PointsDisplay2");
	PointsDisplay2->AddComponent<dae::TextRenderComponent>("Player2 Score: 0", whiteCol, smallFont);
	PointsDisplay2->AddComponent<dae::PointsDisplayComponent>("Player2");
	PointsDisplay2->SetPosition(500, 110);
	scene.Add(std::move(PointsDisplay2));

	auto InstructionsText = std::make_unique<dae::GameObject>("InstructionsText");
	InstructionsText->AddComponent<dae::TextRenderComponent>("Player 1: WASD | Die: K | Score: P", whiteCol, smallFont);
	InstructionsText->SetPosition(10, 50);
	scene.Add(std::move(InstructionsText));

	auto InstructionsText2 = std::make_unique<dae::GameObject>("InstructionsText2");
	InstructionsText2->AddComponent<dae::TextRenderComponent>("Player 2: DPad | Die: X | Score: A", whiteCol, smallFont);
	InstructionsText2->SetPosition(10, 70);
	scene.Add(std::move(InstructionsText2));*/
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);

    return 0;
}

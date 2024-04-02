# include <Siv3D.hpp> // Siv3D v0.6.14
# include "SpriteStudioForSiv3D/SpriteStudioForSiv3D.hpp"

namespace
{
	using AnimationPackArray = Array<SpriteStudio::AnimationBuildInfo::PackInfo>;

	// U"TestData/Ringo_Sample/Ringo/Ringo.sspj"
	static const AnimationPackArray RingoAnimationPacks
	{
		{ U"Ringo",
			{
				U"Setup", U"attack1", U"attack2", U"attack3",
				U"dance", U"dash", U"dead", U"joy",
				U"jump", U"jump_in", U"jump_out", U"jump_wait",
				U"kaiten", U"piyo", U"run", U"sitdown", U"sitdown_wait",
				U"slipdown", U"slipdown_wait", U"stand_up",
				U"wait1", U"wait2", U"wait3", U"wakeup", U"walk",
			}
		},
		{ U"Ringo_tyu", { U"Setup", U"anime_1" } },
	};
}

void Main()
{
	// ログの設定
	{
		using namespace SpriteStudio::DebugLog;
		using LogType = SpriteStudio::DebugLog::LogType;
		// 冗長なログも出す場合はコメントアウト。
		//SetExportLog(LogType::Verbose, LogEnable::Yes);
	}

	// プロジェクトデータの作成
	SpriteStudio::Project ssProject{ U"TestData/Ringo_Sample/Ringo/Ringo.sspj" };
	if (not(ssProject))
	{
		return;
	}
	
	// アニメーション生成情報
	SpriteStudio::AnimationBuildInfo buildInfo
	{
		.pProject = &ssProject,
		.packInfomations = AllAttributeV7AnimationPacks
	};
	// アニメーション再生
	SpriteStudio::AnimationPlayer ssPlayer{ buildInfo, SpriteStudio::StartPlaying::No };
	if (not(ssPlayer))
	{
		return;
	}

	// アニメーション一覧選択用
	ListBoxState animationListBoxState;
	for (const auto& pack : buildInfo.packInfomations)
	{
		for (const auto& anim : pack.animationNames)
		{
			animationListBoxState.items << U"{}_{}"_fmt(pack.animationPackName, anim);
		}
	}
	int32 selectIndex = ssPlayer.getCurrentAnimationIndex();
	if (0 <= selectIndex)
	{
		animationListBoxState.selectedItemIndex = selectIndex;
	}

	Camera2D debugCamera{ Vec2::Zero(), 0.5 };

	while (System::Update())
	{
		ClearPrint();
		Print << U"packName  : " << ssPlayer.getCurrentAnimationPackName();
		Print << U"animName  : " << ssPlayer.getCurrentAnimationName();
		Print << U"canvasSize: " << ssPlayer.getCanvasSize();
		Print << U"FPS       : " << ssPlayer.getCurrentAnimationFPS();
		Print << U"time      : " << ssPlayer.getTime();
		Print << U"frame     : " << ssPlayer.getFrame();
		Print << U"isStopped : " << ssPlayer.isStopped();

		// UPDATE
		{
			debugCamera.update();
			ssPlayer.update(Scene::DeltaTime());
		}

		// DRAW
		{
			const Transformer2D cameraTrans{ debugCamera.getMat3x2() };
			Vec2 drawPos{ 0, 0 };
			ssPlayer.drawAt(drawPos);

			RectF rect{ Point::Zero(), ssPlayer.getCanvasSize() };
			rect.setCenter(drawPos);
			rect.drawFrame();
		}

		// GUI

		if (SimpleGUI::Button(U"play", Vec2{ 10, Scene::Height() - 100 }, 100.0, ssPlayer.isStopped()))
		{
			ssPlayer.play();
		}
		if (SimpleGUI::Button(U"stop", Vec2{ 120, Scene::Height() - 100 }, 100.0, ssPlayer.isPlaying()))
		{
			ssPlayer.stop();
		}
		if (SimpleGUI::Button(U"reset", Vec2{ 230, Scene::Height() - 100 }, 100.0, ssPlayer))
		{
			ssPlayer.resetFrame();
		}
		bool isLoop = ssPlayer.isLoop();
		if (SimpleGUI::CheckBox(isLoop, U"loop", Vec2{ 340, Scene::Height() - 100 }))
		{
			ssPlayer.setLoop(SpriteStudio::LoopEnable(isLoop));
		}
		double frame = static_cast<double>(ssPlayer.getFrame());
		double startFrame = static_cast<double>(ssPlayer.getCurrentAnimationStartFrame());
		double endFrame = static_cast<double>(ssPlayer.getCurrentAnimationEndFrame());
		if (SimpleGUI::Slider(U"frame", frame, startFrame, endFrame, Vec2{0, Scene::Height() - 50}, 80.0, Scene::Width() - 80.0))
		{
			ssPlayer.play();
			ssPlayer.setFrame(static_cast<int32>(frame));
			ssPlayer.update(0.0);
			ssPlayer.stop();
		}

		if (SimpleGUI::ListBox(animationListBoxState, Vec2{ Scene::Width() - 200, 10 }, 190.0, 500.0, ssPlayer))
		{
			if (auto index = animationListBoxState.selectedItemIndex)
			{
				if (ssPlayer.changeAnimation(static_cast<int32>(index.value())))
				{

				}
			}
		}

		debugCamera.draw(Palette::Orangered);
	}
}

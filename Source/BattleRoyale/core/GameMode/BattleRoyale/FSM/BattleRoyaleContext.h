#pragma once

//TODO decidir bien todos los namespaces y ver si se pueden compartir con el otro gamemode.
namespace BRModeFSM
{
	class BattleRoyaleContext
	{
	public:
		BattleRoyaleContext(UWorld* world);

		~BattleRoyaleContext() = default;

	private:
		UWorld* mWorld;
	};
};

#ifndef GFX_GAMEBEHAVIOUR_HPP
#define GFX_GAMEBEHAVIOUR_HPP

#include "Component.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace GFX
{
    class GameBehaviour : public Component
    {
	friend class Application;
	friend class Graphics;
	private:
		static std::vector<GameBehaviour*> behaviours;
		static void NewFrame();
		static void EndFrame();
        static void OnBehaviourApplicationQuit();
        static void OnBehaviourUpdate();
        static void OnBehaviourLateUpdate();
        static void OnBehaviourFixedUpdate();
        static void OnBehaviourGUI();
        static void OnBehaviourEndFrame();
    protected:
        virtual void OnApplicationQuit();
        virtual void OnUpdate();
        virtual void OnLateUpdate();
        virtual void OnFixedUpdate();
        virtual void OnGUI();
        virtual void OnEndFrame();
        static void AddBehaviour(GameBehaviour *behaviour);
        static void RemoveBehaviour(GameBehaviour *behaviour);
    public:
        GameBehaviour();
        virtual ~GameBehaviour();
    };
}

#endif
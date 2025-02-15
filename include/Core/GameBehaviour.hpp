#ifndef GFX_GAMEBEHAVIOUR_HPP
#define GFX_GAMEBEHAVIOUR_HPP

#include "Component.hpp"
#include "Resource.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace GFX
{
    class GameBehaviour : public Component
    {
	friend class Application;
	friend class Graphics;
    friend class Resources;
    friend class PostProcessingRenderer;
	private:
		static std::vector<GameBehaviour*> behaviours;
		static void NewFrame();
		static void EndFrame();
        static void OnBehaviourApplicationQuit();
        static void OnBehaviourUpdate();
        static void OnBehaviourLateUpdate();
        static void OnBehaviourFixedUpdate();
        static void OnBehaviourGUI();
        static void OnBehaviourPostProcess(uint32_t shaderId);
        static void OnBehaviourEndFrame();
        static void OnBehaviourResourceLoadedAsync(const Resource &resource);
        static void OnBehaviourResourceBatchLoadedAsync(const ResourceBatch &resourceBatch);
    protected:
        virtual void OnApplicationQuit();
        virtual void OnUpdate();
        virtual void OnLateUpdate();
        virtual void OnFixedUpdate();
        virtual void OnGUI();
        virtual void OnPostProcess(uint32_t shaderId);
        virtual void OnEndFrame();
        virtual void OnResourceLoadedAsync(const Resource &resource);
        virtual void OnResourceBatchLoadedAsync(const ResourceBatch &resourceBatch);
        static void AddBehaviour(GameBehaviour *behaviour);
        static void RemoveBehaviour(GameBehaviour *behaviour);
    public:
        GameBehaviour();
        virtual ~GameBehaviour();
    };
}

#endif
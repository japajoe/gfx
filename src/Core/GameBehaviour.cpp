#include "GameBehaviour.hpp"
#include "GameObject.hpp"
#include "Application.hpp"
#include "Time.hpp"

namespace GFX
{
    GameBehaviour::GameBehaviour() : Component()
    {
        GameBehaviour::AddBehaviour(this);
    }

    GameBehaviour::~GameBehaviour()
    {
        GameBehaviour::RemoveBehaviour(this);
    }

    void GameBehaviour::OnApplicationQuit()
    {

    }

    void GameBehaviour::OnUpdate()
    {

    }

    void GameBehaviour::OnLateUpdate()
    {

    }

    void GameBehaviour::OnFixedUpdate()
    {

    }

    void GameBehaviour::OnGUI()
    {

    }

    void GameBehaviour::OnEndFrame()
    {

    }

    void GameBehaviour::OnResourceLoadedAsync(const Resource &resource)
    {

    }

    void GameBehaviour::OnResourceBatchLoadedAsync(const ResourceBatch &resourceBatch)
    {

    }

    std::vector<GameBehaviour*> GameBehaviour::behaviours;

    void GameBehaviour::AddBehaviour(GameBehaviour *behaviour)
    {
        for(size_t i = 0; i < behaviours.size(); i++)
        {
            if(behaviours[i] == behaviour)
                return;
        }

        behaviours.push_back(behaviour);
    }

    void GameBehaviour::RemoveBehaviour(GameBehaviour *behaviour)
    {
        bool found = false;
        size_t index = 0;

        for(size_t i = 0; i < behaviours.size(); i++)
        {
            if(behaviours[i] == behaviour)
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
        {
            behaviours.erase(behaviours.begin() + index);
        }
    }

	void GameBehaviour::NewFrame()
	{
		OnBehaviourUpdate();
        OnBehaviourFixedUpdate();
		OnBehaviourLateUpdate();
	}

	void GameBehaviour::EndFrame()
	{
		OnBehaviourEndFrame();
	}

    void GameBehaviour::OnBehaviourResourceLoadedAsync(const Resource &info)
    {
        for(size_t i = 0; i < behaviours.size(); i++)
        {
            behaviours[i]->OnResourceLoadedAsync(info);
        }
    }

    void GameBehaviour::OnBehaviourResourceBatchLoadedAsync(const ResourceBatch &resourceBatch)
    {
        for(size_t i = 0; i < behaviours.size(); i++)
        {
            behaviours[i]->OnResourceBatchLoadedAsync(resourceBatch);
        }
    }

    void GameBehaviour::OnBehaviourApplicationQuit()
    {
        for(auto behaviour : behaviours)
        {
            if(behaviour->GetGameObject()->GetIsActive())
                behaviour->OnApplicationQuit();
        }

        behaviours.clear();

        GameObject::DestroyAll();
    }

    void GameBehaviour::OnBehaviourUpdate()
    {
        for(auto behaviour : behaviours)
        {
            if(behaviour->GetGameObject()->GetIsActive())
                behaviour->OnUpdate();
        }
    }

    void GameBehaviour::OnBehaviourLateUpdate()
    {
        for(auto behaviour : behaviours)
        {
            if(behaviour->GetGameObject()->GetIsActive())
                behaviour->OnLateUpdate();
        }
    }

    static float accumulator = 0.0f;
    static constexpr float fixedTimeStep = 1.0f / 60;

    void GameBehaviour::OnBehaviourFixedUpdate()
    {
        accumulator += Time::GetDeltaTime();

        while(accumulator >= fixedTimeStep)
        {
            for(auto behaviour : behaviours)
            {
                if(behaviour->GetGameObject()->GetIsActive())
                    behaviour->OnFixedUpdate();
            }
            accumulator -= fixedTimeStep;
        }
    }

    void GameBehaviour::OnBehaviourGUI()
    {
        for(auto behaviour : behaviours)
        {
            if(behaviour->GetGameObject()->GetIsActive())
                behaviour->OnGUI();
        }
    }

    void GameBehaviour::OnBehaviourEndFrame()
    {
        for(auto behaviour : behaviours)
        {
            if(behaviour->GetGameObject()->GetIsActive())
                behaviour->OnEndFrame();
        }

        GameObject::OnEndFrame();
    }
}
#pragma once

#include "../graphics/main/GraphicContext.h"
#include "../graphics/components/ui/layout/BasicUI.h"
#include "../graphics/components/ui/layout/BehaviorUI.h"

#include "EventHandler.h"

enum SceneId
{
    Noscene,
    Scene_Main,
    Scene_Behavior,
    Scene_Text,
    Scene_Testing
};


class Scene;

//! \brief Scene
//! For now scene are just a collection of objects.
// It defines also the event handler.
class Scene
{
    public:
        Scene(GraphicContext* graphicContext, LayoutType layout = LayoutType::BASIC);
        virtual ~Scene();

        //! \brief Initialize the scene.
        //! Load objects into the graphic context.
        virtual void Init();

        //! \brief Update the scene.
        //! Won't check if the scene is initialized.
        virtual void Update();

        //! \brief Return the scene identifier defined in enum SceneId
        SceneId GetSceneId() { return m_Id;}
        
        //! \brief Connect a handler to the scene.
        //! Notice: Destroy the previous Handler
        void ConnectHandler(EventHandlerType handlerType);

        static Scene* CreateScene_1(GraphicContext* graphicContext);
        static Scene* CreateScene_3(GraphicContext* graphicContext);

        static Scene* CreateScene_Main(GraphicContext* graphicContext); // first scene the users sees.
        static Scene* CreateScene_Behavior(GraphicContext* graphicContext); // scene used for behavior introduction
        static Scene* CreateScene_Text(GraphicContext* graphicContext); // scene used for Text testing.
        static Scene* CreateScene_Testing(GraphicContext* graphicContext); // scene used for testing.
        static Scene* CreateScene_Wasm(GraphicContext* graphicContext); // scene used for wasm

    private:

        GraphicContext* m_GraphicContext;
        SceneId m_Id;
        LayoutUI* m_UI;

};
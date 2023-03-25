#pragma once

#include "../graphics/main/GraphicContext.h"
#include "../graphics/util/BasicUI.h"

#include "EventHandler.h"

class Scene;

//! \brief Scene
//! A scene contains scenario and act as an abstraction layer for later use.
//! It's main purpose is to isolate the graphic context handling and functionning from the application.
//! (i'm thinking about time events, or pause.. it should have it's own EventHandler.)
class Scene
{
    public:
        Scene(GraphicContext* graphicContext, bool withUI = true);
        virtual ~Scene();

        //! \brief Initialize the scene.
        //! Load objects into the graphic context.
        virtual void Init();

        //! \brief Update the scene.
        //! Won't check if the scene is initialized.
        virtual void Update();
        
        //! \brief Connect a handler to the scene.
        //! Notice: Destroy the previous Handler
        void ConnectHandler(EventHandlerType handlerType);

        static Scene* CreateScene_1(GraphicContext* graphicContext);
        static Scene* CreateScene_2(GraphicContext* graphicContext);
        static Scene* CreateScene_3(GraphicContext* graphicContext);
        static Scene* CreateScene_Text(GraphicContext* graphicContext); // scene used for Text testing.

    private:

        // Do not destroy. It's owned by the application.
        GraphicContext* m_GraphicContext;

        // BasicUI : Allow to display the FPS and the number of particles.
        BasicUI* m_BasicUI;
};
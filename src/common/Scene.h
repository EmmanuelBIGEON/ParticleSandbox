#pragma once

#include "../graphics/main/GraphicContext.h"
#include "../graphics/util/BasicUI.h"

#include "EventHandler.h"

class Scene;

//! \brief Scene
//! For now scene are just a collection of objects.
// It defines also the event handler.
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
        static Scene* CreateScene_3(GraphicContext* graphicContext);

        static Scene* CreateScene_Main(GraphicContext* graphicContext); // first scene the users sees.
        static Scene* CreateScene_Behavior(GraphicContext* graphicContext); // scene used for behavior introduction
        static Scene* CreateScene_Text(GraphicContext* graphicContext); // scene used for Text testing.
        static Scene* CreateScene_Testing(GraphicContext* graphicContext); // scene used for testing.

    private:

        GraphicContext* m_GraphicContext;
        BasicUI* m_BasicUI;
};
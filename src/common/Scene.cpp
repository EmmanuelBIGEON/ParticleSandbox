#include "Scene.h"

#include "../graphics/components/adapters/TriangleAdapter.h"
#include "../graphics/components/adapters/RectangleAdapter.h"
#include "../graphics/components/adapters/AdvancedRectangle.h"
#include "../graphics/components/adapters/CircleAdapter.h"
#include "../graphics/components/common/GraphicText.h"
#include "../graphics/components/common/GraphicImage.h"

#include "EventHandler.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Scene::Scene(GraphicContext* graphicContext) : m_GraphicContext(graphicContext)
{
}

Scene::~Scene()
{
}

void Scene::Init()
{
    // Do nothing for now.
    // Placeholders for later.
}

void Scene::Update()
{
    m_GraphicContext->Render();
}

Scene* Scene::CreateScene_1(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_TEST2);
    TriangleAdapter* adapter = new TriangleAdapter(graphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(graphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    return scene;
}

Scene* Scene::CreateScene_2(GraphicContext* graphicContext)
{
    // ---------- OBJECTS ----------
    TriangleAdapter* adapter = new TriangleAdapter(graphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(graphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    // GraphicText* text = new GraphicText(graphicContext, "Hello World", font_main, 400.0f, 300.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // CircleAdapter* circle = new CircleAdapter(graphicContext, Circle(Point(400.0f, 300.0f), 200.0f));
    // circle->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
    // AdvancedRectangle* rect = new AdvancedRectangle(graphicContext, Rectangle(Point(200.0f, 200.0f), Point(1400.0f, 1000.0f)), glm::vec3(0.5f, 0.5f, 0.5f));
    // CircleAdapter* circle2 = new CircleAdapter(graphicContext, Circle(Point(800.0f, 500.0f), 400.0f));
    // CircleAdapter* circle3 = new CircleAdapter(graphicContext, Circle(Point(500.0f, 400.0f), 80.0f));
    // circle3->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));


    // GraphicImage* image = new GraphicImage(graphicContext, "data/img/merry_christmas_a_bit_late.png", 400.0f, 300.0f, 400.0f, 400.0f);
    return nullptr;
}


void Scene::ConnectHandler(EventHandlerType handlerType)
{
    if(global_EventHandler != nullptr)
    {
        EventHandler* tmp = global_EventHandler;
        global_EventHandler = nullptr;
        delete tmp;
    }

    switch(handlerType)
    {
        case EVENT_HANDLER_TEST2:
        {
            EventHandler_Test2* handler = new EventHandler_Test2(m_GraphicContext);
            global_EventHandler = handler;
            break;
        }
        default:
            break;
    }
}
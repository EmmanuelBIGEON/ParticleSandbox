#include "Scene.h"

#include "../graphics/components/adapters/TriangleAdapter.h"
#include "../graphics/components/adapters/RectangleAdapter.h"
#include "../graphics/components/adapters/AdvancedRectangle.h"
#include "../graphics/components/adapters/CircleAdapter.h"
#include "../graphics/components/adapters/PanickedCircle.h"
#include "../graphics/components/adapters/WigglingCircle.h"
#include "../graphics/components/adapters/ParticleAdapter.h"
#include "../graphics/components/common/GraphicText.h"
#include "../graphics/components/common/GraphicImage.h"
#include "../graphics/components/common/GraphicLine.h"
#include "../graphics/components/common/BezierCurve.h"
#include "../graphics/components/ui/Slider.h"

#include "../graphics/components/ui/Button.h"

#include "../graphics/util/generators/ParticleGenerator.h"

#include "EventHandler.h"


#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <glm/ext.hpp>

// use lowp vector2
typedef glm::tvec2<GLbyte, glm::lowp> lowp_vec2;

Scene::Scene(GraphicContext* graphicContext, bool withUI) : m_GraphicContext(graphicContext), m_BasicUI(nullptr)
{
    if (withUI)
    {
        m_BasicUI = new BasicUI(0.0f, GraphicContext::worldHeight - 300.0f, 170.0f, 300.0f);
        m_BasicUI->Init(graphicContext);
    }
}

Scene::~Scene()
{
    // Delete all the objects.
    m_GraphicContext->Clear(true);
}

void Scene::Init()
{
    // Do nothing for now.
    // Placeholders for later.
}

void Scene::Update()
{
    if(m_BasicUI)
        m_BasicUI->Update();
        
    m_GraphicContext->Render();
}

Scene* Scene::CreateScene_1(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_TEST);
    TriangleAdapter* adapter = new TriangleAdapter(graphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(graphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    return scene;
}

Scene* Scene::CreateScene_2(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_UI);

    // generate particles
    ParticleGenerator* generator = new ParticleGenerator(graphicContext, glm::vec2(50.0f, 50.0f), glm::vec2(1500.0f, 1100.0f));
    graphicContext->AddParticles(800);
    return scene;
}



Scene* Scene::CreateScene_3(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, false);
    scene->ConnectHandler(EVENT_HANDLER_UI);
    return scene;
}

Scene* Scene::CreateScene_Text(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, false);
    scene->ConnectHandler(EVENT_HANDLER_UI);

    Rectangle rect = Rectangle(Point(220.0f, 250.0f), Point(400.0f, 200.0f));
    RectangleAdapter* recta = new RectangleAdapter(graphicContext,rect); 
    recta->SetColor(glm::vec3(0.2f, 0.2f, 0.2f));
    GraphicText* testtest = new GraphicText(graphicContext, "Un test a la con vraiment pas cool", glm::vec2(220.0f, 250.0f), glm::vec2(400.0f, 200.0f));

    // test2
    Rectangle rect2 = Rectangle(Point(420.0f, 250.0f), Point(600.0f, 200.0f));
    RectangleAdapter* recta2 = new RectangleAdapter(graphicContext,rect2);
    recta2->SetColor(glm::vec3(0.2f, 0.2f, 0.2f));
    GraphicText* testtest2 = new GraphicText(graphicContext, "Un test a la con vraim", glm::vec2(420.0f, 250.0f), glm::vec2(600.0f, 200.0f));

    // test3
    Rectangle rect3 = Rectangle(Point(620.0f, 250.0f), Point(800.0f, 200.0f));
    RectangleAdapter* recta3 = new RectangleAdapter(graphicContext,rect3);
    recta3->SetColor(glm::vec3(0.2f, 0.2f, 0.2f));
    GraphicText* testtest3 = new GraphicText(graphicContext, "My kind of text ! Much wow, this text describes the entire universe in one little sentence, that's impressive ! ", glm::vec2(620.0f, 250.0f), glm::vec2(800.0f, 200.0f));

    // test4
    Rectangle rect4 = Rectangle(Point(820.0f, 250.0f), Point(1000.0f, 200.0f));
    RectangleAdapter* recta4 = new RectangleAdapter(graphicContext,rect4);
    recta4->SetColor(glm::vec3(0.2f, 0.2f, 0.2f));
    GraphicText* testtest4 = new GraphicText(graphicContext, "My kind of text ! Much worse in one little sentence, impressive ! ", glm::vec2(820.0f, 250.0f), glm::vec2(1000.0f, 200.0f));
    return scene;
}


void Scene::ConnectHandler(EventHandlerType handlerType)
{
    if(global_EventHandler != nullptr)
    {
        delete global_EventHandler;
        global_EventHandler = nullptr;
    }

    switch(handlerType)
    {
        case EVENT_HANDLER_TEST:
        {
            EventHandler_Test* handler = new EventHandler_Test();
            global_EventHandler = handler;
            break;
        }
        case EVENT_HANDLER_PARTICLE_CREATOR:
        {
            EventHandler_ParticleCreator* handler = new EventHandler_ParticleCreator(m_GraphicContext);
            global_EventHandler = handler;
            break;
        }
        case EVENT_HANDLER_UI:
        {
            EventHandler_UI* handler = new EventHandler_UI(m_GraphicContext);
            global_EventHandler = handler;
            break;
        }
        default:
            break;
    }
}
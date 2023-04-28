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
#include "../graphics/components/ui/Input.h"
#include "../graphics/components/ui/Checkbox.h"

#include "../graphics/main/ParticleImpl.h"

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
        m_BasicUI = new BasicUI(0.0f, GraphicContext::worldHeight - 600.0f, 170.0f, 600.0f);
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

Scene* Scene::CreateScene_Main(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);

    float xmin = GraphicContext::worldWidth / 2.0f - 300.0f;
    float xmax = GraphicContext::worldWidth / 2.0f + 300.0f;
    float ymin = GraphicContext::worldHeight / 2.0f - 300.0f;
    float ymax = GraphicContext::worldHeight / 2.0f + 300.0f;

    graphicContext->AddParticles(500, PART_CLASS_1, xmin, xmax, ymin, ymax);

    return scene;
}

Scene* Scene::CreateScene_Behavior(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);

    float xmin = GraphicContext::worldWidth / 2.0f - 300.0f;
    float xmax = GraphicContext::worldWidth / 2.0f + 300.0f;
    float ymin = GraphicContext::worldHeight / 2.0f - 300.0f;
    float ymax = GraphicContext::worldHeight / 2.0f + 300.0f;

    graphicContext->AddParticles(300, PART_CLASS_1, xmin, xmax, ymin, ymax);
    // graphicContext->AddParticles(300, PART_CLASS_2, xmin, xmax, ymin, ymax);
    graphicContext->AddParticles(300, PART_CLASS_3, xmin, xmax, ymin, ymax);

    GraphicContext::behaviorDriven = true;

    return scene;
}

Scene* Scene::CreateScene_Wasm(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext,false);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);

    float xmin = GraphicContext::worldWidth / 2.0f - 300.0f;
    float xmax = GraphicContext::worldWidth / 2.0f + 300.0f;
    float ymin = GraphicContext::worldHeight / 2.0f - 300.0f;
    float ymax = GraphicContext::worldHeight / 2.0f + 300.0f;

    graphicContext->AddParticles(280, PART_CLASS_1, xmin, xmax, ymin, ymax);
    // graphicContext->AddParticles(300, PART_CLASS_2, xmin, xmax, ymin, ymax);
    graphicContext->AddParticles(280, PART_CLASS_3, xmin, xmax, ymin, ymax);

    GraphicContext::behaviorDriven = true;

    return scene;
}

Scene* Scene::CreateScene_3(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, false);
    scene->ConnectHandler(EVENT_HANDLER_UI);
    Button* button3 = new Button(graphicContext, glm::vec2(400.0f, 300.0f), glm::vec2(200.0f, 50.0f), glm::vec3(0.2f, 0.2f, 0.2f), "");
    button3->SetPathIcon("data/img/play.png");
    button3->SetColor(glm::vec3(0.2f, 0.2f, 0.4f));

    return scene;
}


Scene* Scene::CreateScene_Testing(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, true);
    scene->ConnectHandler(EVENT_HANDLER_UI);

    // Input* input = new Input(graphicContext, "100", 600.0f,700.0f);
    // input->SetNumberOnly(true);
    // input->SetMaxSize(5);

    Checkbox* checkbox = new Checkbox(graphicContext, &GraphicContext::useVelocity,glm::vec2(700.0f, 700.0f));

    ParticleAdapter* adapter = new ParticleAdapter(graphicContext, glm::vec2(400.0f, 400.0f), glm::vec3(0.2f, 0.8f, 0.2f));
    

    return scene;
}

Scene* Scene::CreateScene_Text(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, false);
    scene->ConnectHandler(EVENT_HANDLER_UI);

    glm::vec3 color = glm::vec3(0.2f, 0.2f, 0.2f);

    RectangleAdapter* rect_testtest = new RectangleAdapter(graphicContext, {220.0f, 250.0f}, {400.0f, 200.0f}, color);
    GraphicText* testtest = new GraphicText(graphicContext, "Un test a la con vraiment pas cool", glm::vec2(220.0f, 250.0f), glm::vec2(400.0f, 200.0f));

    RectangleAdapter* rect_testtest2 = new RectangleAdapter(graphicContext, {420.0f, 250.0f}, {600.0f, 200.0f}, color);
    GraphicText* testtest2 = new GraphicText(graphicContext, "Un test a la con vraim", glm::vec2(420.0f, 250.0f), glm::vec2(600.0f, 200.0f));

    RectangleAdapter* rect_testtest3 = new RectangleAdapter(graphicContext, {620.0f, 250.0f}, {800.0f, 200.0f}, color);
    GraphicText* testtest3 = new GraphicText(graphicContext, "My kind of text ! Much wow, this text describes the entire universe in one little sentence, that's impressive ! ", glm::vec2(620.0f, 250.0f), glm::vec2(800.0f, 200.0f));

    RectangleAdapter* rect_testtest4 = new RectangleAdapter(graphicContext, {820.0f, 250.0f}, {1000.0f, 200.0f}, color);
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
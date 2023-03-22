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
        m_BasicUI = new BasicUI(0.0f, GraphicContext::worldHeight - 50.0f, GraphicContext::worldWidth, 50.0f);
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
    scene->ConnectHandler(EVENT_HANDLER_TEST2);
    TriangleAdapter* adapter = new TriangleAdapter(graphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(graphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    // WigglingCircle* circle = new WigglingCircle(graphicContext, Circle(Point(800.0f, 600.0f), 200.0f));

    // GraphicText* text = new GraphicText(graphicContext, "Click anywhere on the screen !", 
    // graphicContext->font_main, 500.0f, 500.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));


    return scene;
}

Scene* Scene::CreateScene_2(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);

    // generate particles
    ParticleGenerator* generator = new ParticleGenerator(graphicContext, glm::vec2(50.0f, 50.0f), glm::vec2(1500.0f, 1100.0f));
    // generator->Generate(620, ParticleClass::PARTICLE_CLASS_A);
    // generator->Generate(000, ParticleClass::PARTICLE_CLASS_B);

    graphicContext->AddParticles(2500);
    return scene;
}



Scene* Scene::CreateScene_3(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_UI);

    Button* button = new Button(graphicContext, glm::vec2(100.0f, 100.0f), glm::vec2(200.0f, 200.0f), glm::vec3(1.0f, 0.0f, 0.0f), "Click me !");
    button->OnClick.Connect([button]()
    {
        std::cout << "Button clicked !" << std::endl;
    });

    return scene;
}


void Scene::ConnectHandler(EventHandlerType handlerType)
{
    // It is stupid.. TODO remove.
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
        case EVENT_HANDLER_PARTICLE_CREATOR:
        {
            EventHandler_ParticleCreator* handler = new EventHandler_ParticleCreator(m_GraphicContext);
            global_EventHandler = handler;
            break;
        }
        case EVENT_HANDLER_PARTICLE_HIGHLIGHTER:
        {
            EventHandler_ParticleHighlighter* handler = new EventHandler_ParticleHighlighter(m_GraphicContext);
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
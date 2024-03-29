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
#include "../graphics/components/ui/Statebox.h"
#include "../graphics/components/ui/MatrixStatebox.h"
#include "../graphics/components/ui/ParticleMatrix.h"

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

Scene::Scene(GraphicContext* graphicContext, LayoutType layout) : m_GraphicContext(graphicContext), m_UI(nullptr)
{
    switch(layout)
    {
        case LayoutType::BASIC:
        {
            m_UI = new BasicUI(0.0f, GraphicContext::worldHeight - 600.0f, 170.0f, 600.0f);
            m_UI->Init(graphicContext);
            break;
        }
        case LayoutType::BEHAVIOR:
        {
            m_UI = new BehaviorUI(200.0f, 600.0f);
            m_UI->Init(graphicContext);
            break;
        }
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
    if(m_UI)
        m_UI->Update();
        
    m_GraphicContext->Render();
}

Scene* Scene::CreateScene_1(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_TEST);
    scene->m_Id = Noscene;

    TriangleAdapter* adapter = new TriangleAdapter(graphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(graphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    return scene;
}

Scene* Scene::CreateScene_Main(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);
    scene->m_Id = Scene_Main;

    float xmin = GraphicContext::worldWidth / 2.0f - 300.0f;
    float xmax = GraphicContext::worldWidth / 2.0f + 300.0f;
    float ymin = GraphicContext::worldHeight / 2.0f - 300.0f;
    float ymax = GraphicContext::worldHeight / 2.0f + 300.0f;

    graphicContext->AddParticles(500, PART_CLASS_1, xmin, xmax, ymin, ymax);

    return scene;
}

Scene* Scene::CreateScene_Behavior(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, LayoutType::BEHAVIOR);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);
    scene->m_Id = Scene_Behavior;

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
    Scene* scene = new Scene(graphicContext,LayoutType::NONE);
    scene->ConnectHandler(EVENT_HANDLER_PARTICLE_CREATOR);
    scene->m_Id = Noscene;

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
    Scene* scene = new Scene(graphicContext, LayoutType::NONE);
    scene->ConnectHandler(EVENT_HANDLER_UI);
    scene->m_Id = Noscene;

    Button* button3 = new Button(graphicContext, glm::vec2(400.0f, 300.0f), glm::vec2(200.0f, 50.0f), glm::vec3(0.2f, 0.2f, 0.2f), "");
    button3->SetPathIcon("data/img/play.png");
    button3->SetColor(glm::vec3(0.2f, 0.2f, 0.4f));

    return scene;
}

Scene* Scene::CreateScene_Testing(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, LayoutType::BEHAVIOR);
    scene->ConnectHandler(EVENT_HANDLER_UI);
    scene->m_Id = Scene_Testing;

    // Input* input = new Input(graphicContext, "100", 600.0f,700.0f);
    // input->SetNumberOnly(true);
    // input->SetMaxSize(5);

    // Checkbox* checkbox = new Checkbox(graphicContext, &GraphicContext::useVelocity,glm::vec2(700.0f, 700.0f));

    // ParticleAdapter* adapter = new ParticleAdapter(graphicContext, glm::vec2(400.0f, 400.0f), glm::vec3(0.2f, 0.8f, 0.2f));
    // Statebox* statebox = new Statebox(graphicContext, glm::vec2(400.0f, 400.0f));
    // int id_state = statebox->AddState(glm::vec3(0.2f, 0.8f, 0.2f));
    // statebox->AddStateAction(id_state, [](){std::cout << "State 1" << std::endl;});
    // id_state = statebox->AddState(glm::vec3(0.8f, 0.2f, 0.2f));
    // statebox->AddStateAction(id_state, [](){std::cout << "State 2" << std::endl;});
    // id_state = statebox->AddState(glm::vec3(0.2f, 0.2f, 0.8f));
    // statebox->AddStateAction(id_state, [](){std::cout << "State 3" << std::endl;});


    // MatrixStatebox* matrixStatebox = new MatrixStatebox(3, glm::vec2(0.0f, 0.0f));
    // matrixStatebox->Init(graphicContext);

    // ParticleBehavior* behavior = graphicContext->GetParticleBehavior(ParticleClass::PART_CLASS_1, ParticleClass::PART_CLASS_3);

    // matrixStatebox->GetStatebox(0,0)->AddState(glm::vec3(1.0f, 0.0f, 0.0f));
    // matrixStatebox->GetStatebox(0,0)->AddStateAction(0, [](){std::cout << "State 1" << std::endl;});
    // matrixStatebox->GetStatebox(1,1)->AddState(glm::vec3(0.0f, 1.0f, 0.0f));
    // matrixStatebox->GetStatebox(1,1)->AddStateAction(0, [](){std::cout << "State 2" << std::endl;});
    // matrixStatebox->GetStatebox(2,2)->AddState(glm::vec3(0.0f, 0.0f, 1.0f));
    // matrixStatebox->GetStatebox(2,2)->AddStateAction(0, [behavior](){
    //     behavior->attraction = 0.0f;
    //     behavior->repulsion = 0.0f;
    //     behavior->attraction_distance = 0.0f;
    //     behavior->repulsion_distance = 0.0f;
    // });
    // matrixStatebox->GetStatebox(2,2)->AddState(glm::vec3(0.0f, 1.0f, 0.0f));
    // matrixStatebox->GetStatebox(2,2)->AddStateAction(1, [behavior](){
    //     behavior->attraction = 5.381f;
    //     behavior->repulsion = 100.21f;
    //     behavior->repulsion_distance = 10.23f;
    //     behavior->attraction_distance = 200.0f;
    // });
    // matrixStatebox->GetStatebox(2,2)->AddState(glm::vec3(0.0f, 1.0f, 0.0f));
    // matrixStatebox->GetStatebox(2,2)->AddStateAction(2, [behavior](){behavior->Repulsion();});

    // matrixStatebox->GetStatebox(2,2)->AddState(glm::vec3(1.0f, 0.0f, 0.0f));
    // matrixStatebox->GetStatebox(2,2)->AddStateAction(2, [](){std::cout << "State 3" << std::endl;});

    float xmin = GraphicContext::worldWidth / 2.0f - 300.0f;
    float xmax = GraphicContext::worldWidth / 2.0f + 300.0f;
    float ymin = GraphicContext::worldHeight / 2.0f - 300.0f;
    float ymax = GraphicContext::worldHeight / 2.0f + 300.0f;
    graphicContext->AddParticles(700, PART_CLASS_1, xmin, xmax, ymin, ymax);
    graphicContext->AddParticles(700, PART_CLASS_2, xmin, xmax, ymin, ymax);
    graphicContext->AddParticles(700, PART_CLASS_3, xmin, xmax, ymin, ymax);
    GraphicContext::behaviorDriven = true;

    return scene;
}

Scene* Scene::CreateScene_Text(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext, LayoutType::NONE);
    scene->ConnectHandler(EVENT_HANDLER_UI);
    scene->m_Id = Scene_Text;

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
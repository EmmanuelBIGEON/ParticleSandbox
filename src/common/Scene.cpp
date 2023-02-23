#include "Scene.h"

#include "../graphics/components/adapters/TriangleAdapter.h"
#include "../graphics/components/adapters/RectangleAdapter.h"
#include "../graphics/components/adapters/AdvancedRectangle.h"
#include "../graphics/components/adapters/CircleAdapter.h"
#include "../graphics/components/common/GraphicText.h"
#include "../graphics/components/common/GraphicImage.h"

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
    // ---------- OBJECTS ----------
    TriangleAdapter* adapter = new TriangleAdapter(m_GraphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(m_GraphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    // GraphicText* text = new GraphicText(m_GraphicContext, "Hello World", font_main, 400.0f, 300.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // CircleAdapter* circle = new CircleAdapter(m_GraphicContext, Circle(Point(400.0f, 300.0f), 200.0f));
    // circle->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
    // AdvancedRectangle* rect = new AdvancedRectangle(m_GraphicContext, Rectangle(Point(200.0f, 200.0f), Point(1400.0f, 1000.0f)), glm::vec3(0.5f, 0.5f, 0.5f));
    // CircleAdapter* circle2 = new CircleAdapter(m_GraphicContext, Circle(Point(800.0f, 500.0f), 400.0f));
    // CircleAdapter* circle3 = new CircleAdapter(m_GraphicContext, Circle(Point(500.0f, 400.0f), 80.0f));
    // circle3->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));


    // GraphicImage* image = new GraphicImage(m_GraphicContext, "data/img/merry_christmas_a_bit_late.png", 400.0f, 300.0f, 400.0f, 400.0f);

}

void Scene::Update()
{
    m_GraphicContext->Render();
}

Scene* Scene::CreateScene_1(GraphicContext* graphicContext)
{
    Scene* scene = new Scene(graphicContext);
    TriangleAdapter* adapter = new TriangleAdapter(graphicContext, Triangle(Point(0.0f, 0.0f), Point(0.0f, 800.f), Point(400.0f, 200.0f)));
    RectangleAdapter* adapter2 = new RectangleAdapter(graphicContext, Rectangle(Point(0.0f, 0.0f), Point(400.0f, 200.0f)));
    return scene;
}

Scene* Scene::CreateScene_2(GraphicContext* graphicContext)
{
    return nullptr;
}
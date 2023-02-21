#pragma once

//! \class GraphicContext
class GraphicContext
{
    public: 
        GraphicContext();
        virtual ~GraphicContext();

        virtual void Init();
        virtual void Render();
        virtual void Update(); // placeholder function for now, might be used later

    protected:
        bool okRendering;
};
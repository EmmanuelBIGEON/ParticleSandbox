#pragma once

//! \class GraphicsContext
class GraphicsContext
{
    public: 
        GraphicsContext();
        virtual ~GraphicsContext();

        virtual void Init();
        virtual void Render();
        virtual void Update(); // placeholder function for now, might be used later

    protected:
        bool okRendering;
};
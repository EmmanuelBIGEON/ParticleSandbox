#pragma once

class GraphicContext;
//! \class LayoutUI
//! Abstract class for layout.

enum LayoutType : unsigned int
{
    NONE,
    BASIC,
    BEHAVIOR
};

class LayoutUI
{
    public:
        LayoutUI() {}
        virtual ~LayoutUI() {}

        virtual void Init(GraphicContext* context) = 0;
        virtual void Update() = 0;
};
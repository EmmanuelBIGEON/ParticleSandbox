#pragma once

#include <iostream>

enum EventType
{
    EVENT_NONE,
    EVENT_MOUSE,
    EVENT_KEY,
    EVENT_WHEEL
};

enum EventMouseType
{
    EVENT_MOUSE_NONE,
    EVENT_MOUSE_MOVED,
    EVENT_MOUSE_PRESSED,
    EVENT_MOUSE_RELEASED
};

enum EventKeyType
{
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED
};

enum MouseButton
{
    MOUSE_BUTTON_NONE,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE
};

class Event
{
    public:
        Event() { m_EventType = EVENT_NONE; }
        virtual ~Event() {}

        virtual EventType GetEventType() const { return EVENT_NONE;}

        EventType m_EventType;

};

class MouseEvent : public Event
{
    public:
        MouseEvent(double x, double y) : posX(x), posY(y) { m_MouseType = EVENT_MOUSE_NONE; m_EventType = EVENT_MOUSE; }
        virtual ~MouseEvent() {}

        virtual EventType GetEventType() const { return EVENT_MOUSE; }
        virtual EventMouseType GetMouseEventType() const { return m_MouseType; }
        
        double posX;
        double posY;
    
    protected:
        EventMouseType m_MouseType;

};

class WheelEvent : public Event
{
    public:
        WheelEvent(double offsetX, double offsetY) : m_OffsetX(offsetX), m_OffsetY(offsetY) { m_EventType = EVENT_WHEEL; }
        virtual ~WheelEvent() {}

        virtual EventType GetEventType() const { return EVENT_WHEEL; }

        double m_OffsetX;
        double m_OffsetY;

};

class KeyEvent : public Event
{
    public:
        KeyEvent(int input_key) : key(input_key) 
        { 
            m_KeyType = EVENT_KEY_PRESSED; 
            m_EventType = EVENT_KEY; 

            // GLFW doesn't detect keyboard configuration, this is problematic for AZERTY and QWERTY. 
            // However in this application there's no need to go further than implementing key numbers.
            // Convert numpad keys to numbers.
            // (I work with an AZERTY, QWWERTY can directly the numbers above the letters)
            if(key == 320) key = 48;
            else if(key == 321) key = 49;
            else if(key == 322) key = 50;
            else if(key == 323) key = 51;
            else if(key == 324) key = 52;
            else if(key == 325) key = 53;
            else if(key == 326) key = 54;
            else if(key == 327) key = 55;
            else if(key == 328) key = 56;
            else if(key == 329) key = 57;
        }
        virtual ~KeyEvent() {}

        virtual EventType GetEventType() const { return EVENT_KEY; }
        virtual EventKeyType GetKeyType() const { return m_KeyType; }

        int key;

    protected:
        EventKeyType m_KeyType;
};

class MouseMovedEvent : public MouseEvent
{
    public:
        MouseMovedEvent(double x, double y) : MouseEvent(x, y) { m_MouseType = EVENT_MOUSE_MOVED; }
        virtual ~MouseMovedEvent() {}
};

class MousePressedEvent : public MouseEvent
{
    public:
        MousePressedEvent(double x, double y, MouseButton button) : MouseEvent(x, y), m_Button(button) { m_MouseType = EVENT_MOUSE_PRESSED; }
        virtual ~MousePressedEvent() {}

        MouseButton m_Button;
};

class MouseReleasedEvent : public MouseEvent
{
    public:
        MouseReleasedEvent(double x, double y, MouseButton button) : MouseEvent(x, y), m_Button(button) { m_MouseType = EVENT_MOUSE_RELEASED; }
        virtual ~MouseReleasedEvent() {}

        MouseButton m_Button;
};

class KeyPressedEvent : public KeyEvent
{
    public:
        KeyPressedEvent(int input_key) : KeyEvent(input_key) { m_KeyType = EVENT_KEY_PRESSED; }
        virtual ~KeyPressedEvent() {}
};

class KeyReleasedEvent : public KeyEvent
{
    public:
        KeyReleasedEvent(int input_key) : KeyEvent(input_key) { m_KeyType = EVENT_KEY_RELEASED; }
        virtual ~KeyReleasedEvent() {}
};


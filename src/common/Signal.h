#pragma once

#include <vector>
#include <functional>

template <typename... Args>
class Signal;

//! \class Slot
//! \brief Function container
template <typename... Args>
class Slot
{
    public:

        Slot(Signal<Args...>* signal, std::function<void(Args...)> function);
        virtual ~Slot();

        void SetActive(bool active) { m_active = active;}
        bool isActive() const { return m_active;}

        void Call(Args... args);

    protected: 
        //! Signal that the slot is connected to
        Signal<Args...>* m_Signal;

        // function pointer tempalted
        std::function<void(Args...)> m_Function;

        bool m_active;
};

//! \class Signal
//! \brief Base class for signals
template <typename... Args>
class Signal
{
    public:
        Signal();
        virtual ~Signal();

        //! \brief Emits the signal
        void Emit(Args... args);

        //! \brief Connects a slot to the signal
        //! Use in constructor of slot
        Slot<Args...>* Connect(std::function<void(Args...)> function);

        //! \brief Disconnects a slot from the signal
        //! Use in destructor of slot
        virtual void Disconnect(Slot<Args...>* slot);

        //! \brief Deactivate a slot temporarily
        //! It should work if you only use the SetActive method on the slot you connected.
        //! This is merely another way to achieve the same result.
        void Deactivate(Slot<Args...>* slot);

        //! \brief Clears all connections to the signal
        virtual void ClearConnections();

        //! \brief Prevent signal from calling other connected slots when called during Emit.
        //! Be wary of the order of the slots in the vector to prevent unwanted behavior.
        void PreventDefault();

        
    private:
        std::vector<Slot<Args...>*> m_Slots;
        bool m_preventDefault;
};


template <typename... Args>
Slot<Args...>::Slot(Signal<Args...>* signal, std::function<void(Args...)> function) : m_Signal(signal), m_Function(function), m_active(true)
{
}

template <typename... Args>
Slot<Args...>::~Slot()
{
    if(m_Signal != nullptr)
    {
        m_Signal->Disconnect(this);
    }
}

template <typename... Args>
void Slot<Args...>::Call(Args... args)
{
    if(m_Function != nullptr)
    {
        m_Function(std::forward<Args>(args)...);
    }
}

template <typename... Args>
Signal<Args...>::Signal() : m_preventDefault(false), m_Slots()
{
}

template <typename... Args>
Signal<Args...>::~Signal()
{
}

template <typename... Args>
void Signal<Args...>::Emit(Args... args)
{
    for(Slot<Args...>* slot : m_Slots)
    {
        if(!slot->isActive()) continue;

        slot->Call(std::forward<Args>(args)...);

        // Stop the execution of the signal if PreventDefault().
        if(m_preventDefault)
        {
            m_preventDefault = false;
            break;
        }
    }
}

template <typename... Args>
Slot<Args...>* Signal<Args...>::Connect(std::function<void(Args...)> function)
{
    Slot<Args...>* slot = new Slot<Args...>(this, function);
    m_Slots.push_back(slot);
    return slot;
}

template <typename... Args>
void Signal<Args...>::Disconnect(Slot<Args...>* slot)
{
    for(int i = 0; i < m_Slots.size(); i++)
    {
        if(m_Slots[i] == slot)
        {
            m_Slots.erase(m_Slots.begin() + i);
            break;
        }
    }
}

template <typename... Args>
void Signal<Args...>::Deactivate(Slot<Args...>* slot)
{
    slot->SetActive(false);
}

template <typename... Args>
void Signal<Args...>::ClearConnections()
{
    m_Slots.clear();
}

template <typename... Args>
void Signal<Args...>::PreventDefault()
{
    m_preventDefault = true;
}






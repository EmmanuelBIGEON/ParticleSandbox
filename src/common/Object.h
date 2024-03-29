#pragma once

// https://github.com/crashoz/uuid_v4
#include <uuid_v4.h>

#include <map>

//! \class Object
//! \brief Base class for all objects.
//! It provides an unique ID for each object and a way to store them together.
class Object
{
    public:
        Object() { 
            UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
            UUIDv4::UUID uuid = uuidGenerator.getUUID(); 
            m_ID = uuid;
            m_Objects[m_ID] = this; 
        }
        
        virtual ~Object() {
            m_Objects.erase(m_ID);
        }

        //! \brief Get the unique ID of the object.
        //! \return The unique ID of the object.
        const UUIDv4::UUID& GetID() const { return m_ID; }

        //! \brief Get the object with the given ID.
        static Object* GetObject(const UUIDv4::UUID& id) { return m_Objects[id]; }

    protected:
        UUIDv4::UUID m_ID; //! The unique ID of the object. 

        static std::map<UUIDv4::UUID, Object*> m_Objects; //! The list of all objects.
};
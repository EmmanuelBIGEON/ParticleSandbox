#pragma once

// https://github.com/crashoz/uuid_v4
#include <uuid_v4.h>

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
        }
        virtual ~Object() {}

        //! \brief Get the unique ID of the object.
        //! \return The unique ID of the object.
        const UUIDv4::UUID& GetID() const { return m_ID; }


    private:
        UUIDv4::UUID m_ID; //!< The unique ID of the object. 
};
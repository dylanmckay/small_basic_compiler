#pragma once

namespace epsilon
{
    class IReferenceCounted
    {

    public:

        inline IReferenceCounted() :  m_references(1) { }

        virtual inline ~IReferenceCounted() { }

        inline void grab() { m_references++; }

        inline void drop()
        {
            m_references--;

            if(m_references == 0)
                delete this;
        }

    private:

        unsigned int m_references;
    };
}

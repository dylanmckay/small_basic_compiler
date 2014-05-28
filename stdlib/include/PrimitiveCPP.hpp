#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <vector>


template<typename F, typename S>
class Pair
{

public:

    Pair(F& first, S& second) : First(first), Second(second)
    {

    }

    F First;
    S Second;


};


class String
{

public:

    String()
    {
        m_buffer = (char*) malloc(1);

        m_buffer[0] = '\0';

        m_length = 0;
    }

    String(const String& s)
    {
        this->m_buffer = (char*) malloc(s.m_length+1);

        memcpy(this->m_buffer, s.m_buffer, s.m_length+1);

        this->m_length = s.m_length;
    }

    String(const char* s)
    {
        unsigned int len = strlen(s);

        m_buffer = (char*)malloc(len+1);

        memcpy(m_buffer, s, len+1);

        m_length = len;
    }

    String& operator=(const String& s)
    {
        this->resize(s.m_length);

        memcpy(this->m_buffer, s.m_buffer, s.m_length+1);

        return *this;
    }

    ~String()
    {
        free(m_buffer);
    }

    void append(const String& s)
    {

        unsigned int origLen = m_length;

        resize(this->m_length + s.m_length);

        memcpy(&this->m_buffer[origLen], s.m_buffer, s.m_length+1);
    }

    String operator+(const String& s) const
    {
        String result = *this;

        result.append(s);

        return result;
    }

    bool operator==(const String& s) const
    {
        if(this->m_length != s.m_length)
            return false;

        for(unsigned int i=0; i<m_length; i++)
        {
            if(tolower(this->m_buffer[i]) != tolower(s.m_buffer[i]))
                return false;
        }

        return true;
    }

    bool operator!=(const String& s) const
    {
        return !this->operator==(s);
    }

    char operator[](unsigned int index) const { return m_buffer[index]; }

    inline unsigned int length() const { return m_length; }

    inline const char* data() const { return m_buffer; }

private:

    char* m_buffer;

    unsigned int m_length;

    void resize(unsigned int length)
    {
        m_buffer = (char*) realloc(m_buffer, length+1);

        m_length = length;
    }
};


class PrimitiveCPP
{

public:

    PrimitiveCPP() : m_value("")
    {
        m_lastElem = 0;
    }

    PrimitiveCPP(const PrimitiveCPP& p) : m_value(p.m_value)
    {
        m_lastElem = 0;
    }

    PrimitiveCPP(const String& val) : m_value(val)
    {
        m_lastElem = 0;
    }

    PrimitiveCPP(const char* str) : m_value(str)
    {
        m_lastElem = 0;
    }

    PrimitiveCPP(double val)
    {
        char buf[256];

        sprintf(buf, "%g", val);

        m_value = buf;

        m_lastElem = 0;
    }

    PrimitiveCPP& operator=(const PrimitiveCPP& p)
    {
        this->m_value = p.m_value;

        return *this;
    }

    PrimitiveCPP getElement(const PrimitiveCPP& key)
    {

        for(unsigned int i=0; i<m_lastElem; i++)
        {
            if(m_elements[i].First.m_value == key.m_value)
                return m_elements[i].Second;
        }

        return PrimitiveCPP(); // It doesn't exist, return an empty value.
    }

    void setElement(const PrimitiveCPP& key, const PrimitiveCPP& val)
    {
        for(unsigned int i=0; i<m_lastElem; i++)
        {
            // It exists already, overwrite it.
            if(m_elements[i].First.m_value == key.m_value)
            {
                m_elements[i].Second = val;
            }
        }

        PrimitiveCPP keyCopy = key;
        PrimitiveCPP valCopy = val;

        m_elements[m_lastElem++] = Pair<PrimitiveCPP,PrimitiveCPP>(key, val);

        //It does not exist, add it.
        //m_elements.push_back(Pair<PrimitiveCPP,PrimitiveCPP>(keyCopy,valCopy));
    }

    const String& getValue() const
    {
        return m_value;
    }

    bool isNumeric() const
    {
        const char* str = m_value.data();
        char* end;

        /* double val = */ strtod(str, &end);

        if(((const char*)end) == str)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    PrimitiveCPP operator==(const PrimitiveCPP& p) const
    {
        return PrimitiveCPP(this->m_value == p.m_value);
    }

    PrimitiveCPP operator!=(const PrimitiveCPP& p) const
    {
        return PrimitiveCPP(this->m_value != p.m_value);
    }

    PrimitiveCPP operator+(const PrimitiveCPP& p) const
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1+d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP(this->getValue() + p.getValue());
        }
    }

    PrimitiveCPP operator-(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1-d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator*(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1*d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator/(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1/d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator<(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1<d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator>(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1>d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator<=(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1<=d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator>=(const PrimitiveCPP& p)
    {
        // Check if both values are numeric.
        if(this->isNumeric() && p.isNumeric())
        {
            // Convert primitives to numbers.
            double d1 = this->toDouble();
            double d2 = p.toDouble();

            // Add them.
            return PrimitiveCPP(d1>=d2);
        }
        else // We are appending strings.
        {
            return PrimitiveCPP();
        }
    }

    PrimitiveCPP operator-() const
    {
        if(this->isNumeric())
        {
            double d = toDouble();

            d = fabs(d)*-1; // Make it negative.

            return PrimitiveCPP(d);
        }
        else
        {
            return PrimitiveCPP();
        }

    }

    PrimitiveCPP operator!() const
    {
        return PrimitiveCPP(!(this->toBoolean()));
    }


    const char* toString() const
    {
        return m_value.data();
    }

    /*!
     * Converts the primitive to a double.
     */
    double toDouble() const
    {
        const char* str = m_value.data();
        char* end;

        double val =  strtod(str, &end);

        // Check if the double conversion failed.
        if(((const char*)end) == str)
        {
            return 0;
        }
        else // The primitive is a valid double.
        {
            return val;
        }
    }

    bool toBoolean() const
    {
        // Check if the primitive is numeric.
        if(this->isNumeric())
        {
            double val = toDouble();

            // If it equals 0, false, otherwise true.
            return val == 0 ? false : true;
        }
        else // The primitive is textual.
        {
            return (this->m_value == "False") ? false : true;
        }
    }

private:

    String m_value;


    unsigned int m_lastElem;

    Pair<PrimitiveCPP,PrimitiveCPP> m_elements[256];
};



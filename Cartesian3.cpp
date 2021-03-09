///////////////////////////////////////////////////
//
//  Hamish Carr
//  January, 2018
//
//  ------------------------
//  Cartesian3.h
//  ------------------------
//  
//  A minimal class for a point in Cartesian space
//  
///////////////////////////////////////////////////

#include "Cartesian3.h"
#include "math.h"
#include <iomanip>

// constructors
Cartesian3::Cartesian3() 
    : x(0.0), y(0.0), z(0.0) 
    {}

Cartesian3::Cartesian3(float X, float Y, float Z)
    : x(X), y(Y), z(Z) 
    {}

Cartesian3::Cartesian3(const Cartesian3 &other)
    : x(other.x), y(other.y), z(other.z) 
    {}
    
// equality operator
bool Cartesian3::operator ==(const Cartesian3 &other) const
    { // Cartesian3::operator ==()
    return ((x == other.x) && (y == other.y) && (z == other.z));
    } // Cartesian3::operator ==()

// addition operator
Cartesian3 Cartesian3::operator +(const Cartesian3 &other) const
    { // Cartesian3::operator +()
    Cartesian3 returnVal(x + other.x, y + other.y, z + other.z);
    return returnVal;
    } // Cartesian3::operator +()

// subtraction operator
Cartesian3 Cartesian3::operator -(const Cartesian3 &other) const
    { // Cartesian3::operator -()
    Cartesian3 returnVal(x - other.x, y - other.y, z - other.z);
    return returnVal;
    } // Cartesian3::operator -()

// multiplication operator
Cartesian3 Cartesian3::operator *(float factor) const
    { // Cartesian3::operator *()
    Cartesian3 returnVal(x * factor, y * factor, z * factor);
    return returnVal;
    } // Cartesian3::operator *()

// division operator
Cartesian3 Cartesian3::operator /(float factor) const
    { // Cartesian3::operator /()
    Cartesian3 returnVal(x / factor, y / factor, z / factor);
    return returnVal;
    } // Cartesian3::operator /()


// unary minus operator flips inverses direction
Cartesian3 Cartesian3::operator-() const {
    Cartesian3 returnVal(-x, -y, -z);
    return returnVal;
}

// dot product routine
float Cartesian3::dot(const Cartesian3 &other) const
    { // Cartesian3::dot()
    float returnVal = x * other.x + y * other.y + z * other.z;
    return returnVal;
    } // Cartesian3::dot()

// cross product routine
Cartesian3 Cartesian3::cross(const Cartesian3 &other) const
    { // Cartesian3::cross()
    Cartesian3 returnVal(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    return returnVal;
    } // Cartesian3::cross()

// routine to find the length
float Cartesian3::length() const
    { // Cartesian3::length()
    return sqrt(x*x + y*y + z*z);   
    } // Cartesian3::length()

// normalisation routine
Cartesian3 Cartesian3::unit() const
    { // Cartesian3::unit()
    float length = sqrt(x*x+y*y+z*z);
    Cartesian3 returnVal(x/length, y/length, z/length);
    return returnVal;
    } // Cartesian3::unit()

// operator that allows us to use array indexing instead of variable names
float &Cartesian3::operator [] (const int index)
    { // operator []
    // use default to catch out of range indices
    // we could throw an exception, but will just return the 0th element instead
    switch (index)
        { // switch on index
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        // actually the error case
        default:
            return x;       
        } // switch on index
    } // operator []

// operator that allows us to use array indexing instead of variable names
const float &Cartesian3::operator [] (const int index) const
    { // operator []
    // use default to catch out of range indices
    // we could throw an exception, but will just return the 0th element instead
    switch (index)
        { // switch on index
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        // actually the error case
        default:
            return x;       
        } // switch on index
    } // operator []

// multiplication operator
Cartesian3 operator *(float factor, const Cartesian3 &right)
    { // operator *
    // scalar multiplication is commutative, so flip & return
    return right * factor;
    } // operator *

// stream input
std::istream & operator >> (std::istream &inStream, Cartesian3 &value)
    { // stream output
    inStream >> value.x >> value.y >> value.z;
    return inStream;
    } // stream output
        
// stream output
std::ostream & operator << (std::ostream &outStream, const Cartesian3 &value)
    { // stream output
    outStream << std::setprecision(4) << value.x << " " << std::setprecision(4) << value.y << " " << std::setprecision(4) << value.z;
    return outStream;
    } // stream output
        

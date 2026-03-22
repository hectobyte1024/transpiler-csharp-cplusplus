#ifndef GENERATED_H
#define GENERATED_H

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

// Interface: IShape
class IShape
{
    public:
        virtual double GetArea() = 0;
public:
    virtual ~IShape() = default;
};


#endif // GENERATED_H

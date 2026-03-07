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
        virtual double GetPerimeter() = 0;
public:
    virtual ~IShape() = default;
};

class Circle : public IShape
{
    private:
        double radius;

public:
    void SetRadius(double r);
    double GetArea();
    double GetPerimeter();
};


#endif // GENERATED_H

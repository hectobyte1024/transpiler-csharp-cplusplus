#ifndef GENERATED_H
#define GENERATED_H

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

class Animal
{
    private:
        std::string name;

public:
    void SetName(std::string n);
    virtual void MakeSound();
};

class Dog : public Animal
{
public:
    void MakeSound() override;
};


#endif // GENERATED_H

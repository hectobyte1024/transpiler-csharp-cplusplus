#ifndef GENERATED_H
#define GENERATED_H

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

template<typename T>
class Box
{
    private:
        T value;

public:
    T GetValue();
    void SetValue(T val);
};


#endif // GENERATED_H

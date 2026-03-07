// C# generics example
class Box<T>
{
    private T value;
    
    public T GetValue()
    {
        return value;
    }
    
    public void SetValue(T val)
    {
        value = val;
    }
}

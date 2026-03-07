// C# inheritance example
class Animal
{
    protected string name;
    
    public void SetName(string n)
    {
        name = n;
    }
    
    public virtual void MakeSound()
    {
        Console.WriteLine("Some sound");
    }
}

class Dog : Animal
{
    public override void MakeSound()
    {
        Console.WriteLine("Woof!");
    }
}

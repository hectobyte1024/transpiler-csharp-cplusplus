// C# interface example
interface IShape
{
    double GetArea();
    double GetPerimeter();
}

public class Circle : IShape
{
    private double radius;
    
    public void SetRadius(double r)
    {
        radius = r;
    }
    
    public double GetArea()
    {
        return 3.14159 * radius * radius;
    }
    
    public double GetPerimeter()
    {
        return 2.0 * 3.14159 * radius;
    }
}

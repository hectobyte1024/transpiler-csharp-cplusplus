public class Circle
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
}

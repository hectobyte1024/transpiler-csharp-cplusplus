// C# exception handling example
class Calculator
{
    public static void Initialize()
    {
        Console.WriteLine("Calculator ready");
    }
    
    public static int Divide(int a, int b)
    {
        try
        {
            if (b == 0)
            {
                throw new Exception("Division by zero");
            }
            return a / b;
        }
        catch (Exception ex)
        {
            Console.WriteLine("Error occurred");
            return 0;
        }
        finally
        {
            Console.WriteLine("Cleanup");
        }
    }
}

using System;

namespace MathOperations
{
    class Calculator
    {
        public static int Add(int a, int b)
        {
            return a + b;
        }
        
        public static int Multiply(int a, int b)
        {
            return a * b;
        }
        
        public static void Main()
        {
            int x = 10;
            int y = 20;
            int sum = Add(x, y);
            int product = Multiply(x, y);
            
            Console.WriteLine(sum);
            Console.WriteLine(product);
        }
    }
}

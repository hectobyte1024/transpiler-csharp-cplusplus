using System;

namespace ControlFlow
{
    class Program
    {
        public static void Main()
        {
            int n = 10;
            int sum = 0;
            
            for (int i = 1; i <= n; i++)
            {
                sum = sum + i;
            }
            
            if (sum > 50)
            {
                Console.WriteLine(sum);
            }
            else
            {
                Console.WriteLine(0);
            }
        }
    }
}

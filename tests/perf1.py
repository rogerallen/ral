def Fibonacci(n): 
    if n<0: 
        print("Incorrect input") 
    # First Fibonacci number is 0 
    elif n==1: 
        return 0
    # Second Fibonacci number is 1 
    elif n==2: 
        return 1
    else: 
        return Fibonacci(n-1)+Fibonacci(n-2)

print("fib 1 =",Fibonacci(1))
print("fib 10 =",Fibonacci(10))
print("fib 20 =",Fibonacci(20))
print("fib 25 =",Fibonacci(25))

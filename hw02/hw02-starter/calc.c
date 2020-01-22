#include <stdio.h>
#include <stdlib.h>

long compute(long x, long y, char op); 

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("Usage:\n %s N op N\n", argv[0]);
        return 0;
    }
    
    char op = argv[2][0]; // *argv[2];
    printf("Operator number: %ld\n", op);
    // argv[2] might be a string, or a pointer to a string
    // if it is a pointer, idk what to do tbh
    
    if (!((op == '+') || (op == '-') || (op == '/') || (op == '*'))) {
        printf("Usage:\n %s N op N, where op must be one of: +, -, *, /\n", argv[0]);
        return 0;
    }
    
    long x = atol(argv[1]);
    long y = atol(argv[3]);
    
    long ans = compute(x, y, op);

    printf("%ld %c %ld = %ld\n", x, op, y, ans);
    return 0;
    
}

long compute(long x, long y, char op) {
    
    long ans;
    
    if (op == '+') {
	ans = x + y;
    }
    
    if (op == '-') {
        ans = x - y;
    }
    
    if (op == '*') {
        ans = x * y;
    }
    
    if (op == '/') {
        ans = x / y;
    }
    
    return ans;
           
}

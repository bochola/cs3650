#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("Usage:\n %s N op N\n", argv[0]);
        return 0;
    }
    
    string op = argv[2];
    // argv[2] might be a string, or a pointer to a string
    // if it is a pointer, idk what to do tbh
    if ((op !=) ||(op !=) ||(op !=) ||(op !=)) {
        printf("Usage:\n %s N op N, where op must be one of: +, -, *, /\n", argv[0]);
        return 0;
    }

    long x = atol(argv[1]);
    long y = atol(argv[3]);
    
    int ans = compute(x, y, op);

    printf("%ld %ld %ld = %ld\n", x, op, y, ans);
    return 0;
}

int compute(int x, int y, string op) {
    
    long ans;

    switch(op) {
        
        case op == "+":
            ans = x + y;
            break;
        case op == "-":
            ans = x - y;
            break;
        case op == "/":
            ans = x / y;
            break;
        case op == "*":
            ans = x * y;
            break;
    }

    return ans;           
}

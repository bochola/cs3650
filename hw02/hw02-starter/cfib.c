int cfib(int x) {
    
    int ans;
    
    if (x > 1) {
        
        ans = cfib(x-1) + cfib(x-2);
    }
    else if (x == 1) {
        ans = x;
    }
    else {
        ans = 0;
    }

    return ans;
}

.global main

.data
bad_input: .string "bad input"
ld_string: .string "%ld"


.text
main:
    push %r12         // %r12 is nn
    push %r13         // %r13 is xs
    enter $0, $0
    
    // lea %r12(%rsp), %rdi  the comiler told me to junk this line?
    jmp read_int
    
    // long nn;
    // read_int(&nn);
    
    // How do I get the function to come back to here after going to read_int??
    // Considering that read_int is supposed to be called an infinite number of times
    // we cant hardcode it to jump back to a theoretical "main_part2" function
    
    // long* xs = malloc(nn * sizeof(long));
    
    //for (long ii = 0; ii < nn; ++ii) {
    //    read_int(&(xs[ii]));
    //}
    
    //quicksort(xs, 0, nn);
    
    //printf("%ld\n", nn);
    //for (long ii = 0; ii < nn; ++ii) {
    //    printf("%ld ", xs[ii]);
    //}
    //printf("\n");
    
    //free(xs);
    //return 0;
    
    
    
    leave
    ret
 

swap:
    //swap(long* xs, long ii, long jj)
    //{
    //    if (ii != jj) {
    //        long tmp = xs[ii];
    //        xs[ii] = xs[jj];
    //        xs[jj] = tmp;
    //}



partition:
    //partition(long* xs, long lo, long hi)
    //{
    //    long pivot = xs[hi - 1];

    //    long ii = lo;
    //    long jj = lo;
    //    for (; jj < (hi - 1); ++jj) {
    //        if (xs[jj] < pivot) {
    //            swap(xs, ii, jj);
    //            ++ii;
    //        }
    //    }
    //    swap(xs, ii, hi - 1);
    //    return ii;
    //}




quicksort:
    //quicksort(long* xs, long lo, long hi)
    //{
    //  if (hi - lo < 1) {
    //      return;
    //}

    //    long pp = partition(xs, lo, hi);
    //    quicksort(xs, lo, pp);
    //    quicksort(xs, pp + 1, hi);
    //}


read_int:
    // ptr is %r8
    mov %rdi, %r8
    
    lea ld_string(%rsp), %rdi    // Loads the address of ld_string from the stack pointer into rdi
    //lea %r8(%rsp), %rsi compiler told me to junk this line?
    call scanf  
// ??????? When calling scanf, what are the output registers? How to copy the != 1 from the c code?
// Is the %rax the true or false result? How do I access the 
    cmp $1, %rax                 // Compare 1 with the result of scanf
    jne invalid_input            // If the result is not equal, go to invalid_input
    
    // the last lines  should place the input number into %rax
    // how do I send it back to where it was called from?
    
invalid_input:
    //  puts("bad input");
    //  exit(1);
    mov $bad_input, %rdi
    call puts
    call abort


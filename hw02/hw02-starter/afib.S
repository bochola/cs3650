//afib.S
.global main

.data

.text
main:
    push %rbx         // Pushes off the base pointer location

    enter $0, $0      // Essentially just brackets

    mov %rdi, %rdx    // Moves the provided number(x)  to rdx

    cmp $1, %rdx      // Compares x to 1 and flags the result
    je one            // If x = 1, jump to ONE
    jg fib            // If x > 1, jump to FIB

    cmp $0, %rdx
    je zero           // If x = 0, jump to ZERO    

fib:
    push %rax         // Saves the value of the previous loop    
    mov %rdx, %r12    // Copy x to r12

    mov %rdx, %r12    // Copy x to r12

    dec %r12          // Subtract 1 from x
    mov %r12, %rdi    // Moves x-1 into rdi for next afib call
    call main
    mov %rax, %r13    // Saves the result of fib(x-1)

    dec %r12          // Subtract 1 from x (effectively x-2 now)
    mov %r12, %rdi    // Moves x-2 into rdi for next afib call
    call main
    mov %rax, %r14    // Saves the result of fib(x-2)

    add %r13, %r14    // fib(x-1) + fib(x-2) = r13
    mov %r13, %rax
    j done

one:
    mov $1, %rax
    j done

zero:
    mov $0, %rax
    j done

done:
    leave
    pull %rbx

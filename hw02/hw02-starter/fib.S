//fib.S
.global main

.data
    Usage1: .string "Usage: ./afib  N, where N >= 0\n"
    Output: .string "fib(%ld) = %ld\n"

.text
main:
    push %r12          // r12 is argv
    push %r13          // r13 is x, the provided number
    enter $0, $0       // Bracket things
    
    mov %rsi, %r12     // populates r12 with argv
    
    //if (argc != 2){printf(Usage msg)}
    // rdi is argc
    cmp $2, %rdi       // Compares argc to 2
    jne invalid_arg
    
    //int value = atol(argv[1]);
    mov 8(%r12), %rdi  // moves the pointer to r12 into rdi for atol
    call atol
    mov %rax, %r13     // puts the value of x into r13
    
    
    //if (value < 0) {printf(Usage msg)}
    cmp $0, %r13
    jle invalid_arg
    
    mov %r13, %rdi     // Moves x into rdi for the fib sequencing
    jmp fib
      

fib:

    mov %rdi, %r14     // Copy the provided number into r14
    
    cmp $1, %r14       // Compares x to 1 and flags the result
    je one             // If x = 1, jump to ONE
    jg fib_loop        // If x > 1, jump to FIB

    cmp $0, %r14
    je zero            // If x = 0, jump to ZERO    

fib_loop:

    dec %r14           // Subtract 1 from x
    mov %r14, %rdi     // Moves x-1 into rdi for next afib call
    call fib
    mov %rax, %r15     // Saves the result of fib(x-1)
    
    dec %r14           // Subtract 1 from x (effectively x - 2 now)
    mov %r14 %rdi      // Moves x-2 into rdi for next afib call
    call fib
    mov %rax, %r14     // Saves the result of fib(x-2)

    add %r14, %r15     // fib(x-1) + fib(x-2) = r9
    mov %r14, %rax
    jmp done

one:
    mov $1, %rax       // Place the result into rax 
    jmp done

zero:
    mov $0, %rax       // Place the result in rax
    jmp done

invalid_arg:
    
    mov Usage1, %rdi
    //mov $0, %al
    call printf
    
    leave
    pop %rbx


done:
    
    
    mov Output, %rdi
    mov %r12, %rsi
    mov %rax, %rdx
    mov $0, %al
    call printf
    
    leave
    pop %rbx



.global main
.data
bad_input: .string "bad input"

.text
main:
    enter $0, $0
    
    // long nn;
    // read_int(&nn);
    
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
    //read_int(long* ptr)
    //{
    //    if (scanf("%ld", ptr) != 1) {
    //        puts("bad input");
    //        exit(1);
    //    }
    //}

    
    mov $bad_input, %rdi
    call puts




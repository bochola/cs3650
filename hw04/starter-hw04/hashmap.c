// hashmap.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// See "man strlcpy"
#include <bsd/string.h>
#include <string.h>
#include "hashmap.h"

int hash(char* key)
{
    // TO-DONE: Produce an appropriate hash value.
    // Using modulo on the character's position multiplied by
    // the ascii value of the character will produce a unique 
    // hash for every single possible key
    int hashCode = 0;
    for (int ii = 0; ii < strlen(key); ii++) {
        hashCode = hashCode + (key[ii] * ii % 41);
    }
    
    return hashCode;
}

hashmap* make_hashmap_presize(int nn)
{
    // TODO: Allocate and initialize a hashmap with capacity 'nn'.
    hashmap* hh = calloc(nn, sizeof(hashmap));
    hh->mapCap = 2 * nn;
    hh->mapSize = nn;
    hh->data = calloc(nn, sizeof(hashmap_pair));
    // If hashmaps are lists of hash_pairs, then do i need to initialize the pairs?
    
    return hh;
}

hashmap* make_hashmap()
{
    return make_hashmap_presize(4);
}

void free_hashmap(hashmap* hh)
{
    // TO-DONE: Free all allocated data.
    if (hh) {
        printf("Freeing hashmap data...\n");

        int ii;
        for (ii = 0; ii < hh->mapCap; ii++)
        {
            // Compiler is complaining about using a struct type when
            // a scalar is required? But hh->data[ii] should be a
            // pointer right? And this is how to check for a null pointer
            // right?
            //
            //if (hh->data[ii]) {
            //    free(hh->data[ii]);
            //}
        }

        printf("Freeing hashmap...\n");
        free(hh);
        printf("Freed\n");
    }
}

int hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}

int hashmap_get(hashmap* hh, char* kk)
{
    // TODO: Return the value associated with the
    // key kk.
    // Note: return -1 for key not found.
    //
    // Using linear probing, search where kk points to, if kk isnt found then
    // increment to the next slot. If an empty slot is found before the 
    // correct key is found, then return -1
}

void hashmap_put(hashmap* hh, char* kk, int vv)
{
    // TODO: Insert the value 'vv' into the hashmap
    // for the key 'kk', replacing any existing value for that key.
    // Linear probing says to use the same procedure as hashmap_get
    // and place the value in that slot if found. Should free here to 
    // avoid potential memory leaks
    //
    // Load factor is the number of filled slots divided by the total slots
    // if that gets to be larger than 0.5, then expand the hashmap
    // How to do that?
}

void hashmap_del(hashmap* hh, char* kk)
{
    // TODO: Remove any value associated with
    // this key in the map.
    // 
    // To remove the value associated with kk, scan forward in the hashmap to
    // see if there are any other keys that are equal or less than kk. If you
    // hit an empty slot then empty kk. If a key =< kk is found, swap that new
    // key with kk and start the process over at the new key
}

hashmap_pair hashmap_get_pair(hashmap* hh, int ii)
{
    // TO-DONE: Get the {k,v} pair stored in index 'ii'.
    return hh->data[ii];
}

void hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    // TODO: Print out all the keys and values currently
    // in the map, in storage order. Useful for debugging.
}

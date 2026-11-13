/*

A simple demonstration of a generic modulo hash function used in 
a cuckoo hash algorithm.

Creates an array of size num_keys , populates the array with sequential session IDs, 
uses hash_insert to populate hash table , hash_insert uses mod_hash to 
define where the key value goes in the table. 

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

unsigned int PRIME = 88883;
unsigned int SECOND_PRIME = 67000009;
int NUM_COLLISIONS = 0;

// int MAX_EVICTION_CHAIN = 0;

int NUM_SUCC_DELETIONS = 0;


uint64_t generate_session_id(void){

   // create 16 bit building blocks
   // necessary since rand() produces up to 31 bits (Google AI)
   uint64_t p1 = (uint64_t)(rand() & 0xFFFF);
   uint64_t p2 = (uint64_t)(rand() & 0xFFFF);
   uint64_t p3 = (uint64_t)(rand() & 0xFFFF);
   uint64_t p4 = (uint64_t)(rand() & 0xFFFF);

   // stitch the 16-bit chunks into one 64 bit session ID
   // p1 is the most significant block and p4 the least.
   uint64_t id = (p1 << 48) | (p2 << 32) | (p3 << 16) | p4; 

   // no more negative integers (uint64_t) so -1 will wrap around
   // to 0xFFFFFFFFFFFFFFFF. Since -1 is now a valid value, we tell 
   // generator to forbid 0, reserving 0 as the marker for an empty slot. 
   if (id == 0) id = 1;

   return id;
}

unsigned int mod_hash(uint64_t key, int table_size){
   uint64_t u_table_size = (uint64_t)table_size;
   
   uint64_t index = (key * PRIME) % u_table_size;

   return (unsigned int)index;   
}

void hash_insert(uint64_t* hash_table, int table_size, uint64_t key){
   unsigned int index = mod_hash(key, table_size);

   // if slot is occupied
   if (hash_table[index] != 0){
      printf("Collision! Evicting tenant %" PRIu64 " for %" PRIu64 " at index %d\n", 
              hash_table[index], key, index);
      hash_insert(hash_table, table_size, key * SECOND_PRIME);
      NUM_COLLISIONS += 1;
   } else {
      printf("Empty slot at %d. Inserting %" PRIu64 "\n", index, key);
   }

   // Actual storage executed here
   hash_table[index] = key;

}

void hash_delete(uint64_t* hash_table, int table_size, uint64_t key){
   unsigned int index = mod_hash(key, table_size);

   if (hash_table[index] == key){
      hash_table[index] = 0;
      printf("Deleted key %" PRIu64 " from index %d\n", key, index);
      NUM_SUCC_DELETIONS += 1;
   } else if (hash_table[index] == 0){
      printf("Delete failed: index %d is empty\n", index);
   } else {
      printf("Delete failed: a different key was found in index %d\n", index);
   }

}

int main(int argc, char* argv[]){
   if (argc <= 3){
      printf("Error: Illegal arguments\n");
      printf("Usage: mod-hash <table_size>, <num_keys>, <num_deletes>\n"
            "Optional: <prime>, <second_prime>\n");
      return 1; 
   } else if (argc > 4) {
      PRIME = (unsigned int)atoi(argv[4]);
      SECOND_PRIME = (unsigned int)atoi(argv[5]);
   }

   int table_size = atoi(argv[1]);
   int num_keys = atoi(argv[2]);
   int num_deletes = atoi(argv[3]);

 
   if (table_size <= 0 || num_keys <= 0 || num_deletes < 0){
      printf("Error: Both table size and number of keys must be greater than" 
              "zero and number of deletes must not be negative\n");
      return 1;
   }
   
   if (num_deletes > num_keys){
      printf("Delete fewer keys than num_keys");
      num_deletes = num_keys;
   }

   // Use time to create a "random" number generator
   srand((unsigned int)time(NULL));

   // allocate memory for key array
   uint64_t* keys = malloc(num_keys * sizeof(uint64_t));
   if (keys == NULL){
      printf("Memory allocation error while creating key array\n");
      return 1;
   }
   printf("Allocated %d spaces for keys in key array\n", num_keys);

   for (int i = 0; i < num_keys; i++){
      printf("Generating key %d for input array\n", i);
      keys[i] = generate_session_id();
   } 

   // Allocate/initialize empty (-1) hash table
   uint64_t* hash_table = malloc(table_size * sizeof(uint64_t));
   if (hash_table == NULL){
      printf("Memory allocation error while creating hash table\n");
      return 1;
   }
   printf("Allocated %d spaces for hash table\n", table_size);
   
   for (int i = 0; i < table_size; i++){
      hash_table[i] = 0; 
   }

   printf("Initialized hash table\n\n");

   // inserting random numbers to hash table using hash_insert
   for (int i = 0; i < num_keys; i++){
      printf("Attempting to insert %" PRIu64 " into hash table\n", keys[i]);
      hash_insert(hash_table, table_size, keys[i]);
   }
   printf("Inserted %d keys into hash table\n", num_keys);

   // delete
   if (num_deletes > 0){
      for (int i = 0; i < num_deletes; i++) {
         int random_index = rand() % num_keys;
         uint64_t target_key = keys[random_index];

         printf("Attempting to delete key %" PRIu64 ":\n", target_key);
         hash_delete(hash_table, table_size, target_key);
      }
   }
   

   // print final form of hash table
   for (int i = 0; i < table_size; i++){
      if (hash_table[i] != -1){
         printf("Slot %d : %" PRIu64 "\n", i, hash_table[i]);
      } else {
         printf("Slot %d: empty\n", i);
      }
   }

   printf("Number of collisions: %d\n", NUM_COLLISIONS);
   printf("Number of successful deletions: %d\n", NUM_SUCC_DELETIONS);

   free(keys);

   free(hash_table);
   return 0;

}

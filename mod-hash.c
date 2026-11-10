/*

A simple demonstration of a generic modulo hash function


*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int PRIME;
int NUM_COLLISIONS = 0;

int mod_hash(int key, int table_size){
   if (key < 0) key = -key;
   return (key * PRIME) % table_size;
}

void hash_insert(int* hash_table, int table_size, int key){
   int index = mod_hash(key, table_size);

   // if slot is occupied
   if (hash_table[index] != -1){
      printf("Collision! Evicting tenant %d for %d at index %d\n", 
              hash_table[index], key, index);
      NUM_COLLISIONS += 1;
   } else {
      printf("Empty slot at %d. Inserting %d\n", index, key);
   }

   // Actual storage executed here
   hash_table[index] = key;

}

void hash_delete(int* hash_table, int table_size, int key){
   int index = mod_hash(key, table_size);

   if (hash_table[index] == key){
      hash_table[index] = -1;
      printf("Deleted key %d from index %d\n", key, index);
   } else if (hash_table[index] == -1){
      printf("Delete failed: index %d is empty\n", index);
   } else {
      printf("Delete failed: a different key was found in index %d\n", index);
   }

}

int main(int argc, char* argv[]){
   if (argc != 5){
      printf("Error: Illegal arguments\n");
      printf("Usage: mod-hash <table_size>, <num_keys>, <num_deletes>, <prime>\n");
      return 1; 
   }

   int table_size = atoi(argv[1]);
   int num_keys = atoi(argv[2]);
   int num_deletes = atoi(argv[3]);
   PRIME = atoi(argv[4]);
 
   if (table_size <= 0 || num_keys <= 0 || num_deletes < 0){
      printf("Error: Both table size and number of keys must be greater than" 
              "zero and number of deletes must not be negative\n");
      return 1;
   }
   
   if (num_deletes > num_keys){
      printf("Delete fewer keys than num_keys");
      num_deletes = num_keys;
   }

   printf("Deleting %d keys", num_deletes);

   // Use time to create a "random" number generator
   srand((unsigned int)time(NULL));

   // allocate memory for key array
   int* keys = malloc(num_keys * sizeof(int));
   if (keys == NULL){
      printf("Memory allocation error while creating key array\n");
      return 1;
   }

   // Allocate/initialize empty (-1) hash table
   int* hash_table = malloc(table_size * sizeof(int));
   if (hash_table == NULL){
      printf("Memory allocation error while creating hash table\n");
      return 1;
   }
   
   for (int i = 0; i < table_size; i++){
      hash_table[i] = -1;
   }

   printf("Running with table size: %d and %d keys\n\n", table_size, num_keys);

   // inserting random numbers to hash table using hash_insert
   for (int i = 0; i < num_keys; i++){
      keys[i] = rand();
      hash_insert(hash_table, table_size, keys[i]);
   }

   // delete
   if (num_deletes > 0){
      for (int i = 0; i < num_deletes; i++) {
         int random_index = rand() % num_keys;
         int target_key = keys[random_index];

         printf("Attempting to delete key %d:\n", target_key);
         hash_delete(hash_table, table_size, target_key);
      }
   }

   // print final form of hash table
   for (int i = 0; i < table_size; i++){
      if (hash_table[i] != -1){
         printf("Slot %d: %d\n", i, hash_table[i]);
      } else {
         printf("Slot %d: empty\n", i);
      }
   }

   printf("Number of collisions: %d\n", NUM_COLLISIONS);

   free(keys);

   free(hash_table);
   return 0;

}

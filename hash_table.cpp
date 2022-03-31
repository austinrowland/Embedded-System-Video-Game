/*
 Student Name: Austin Rowland
 Date: Nov 05, 2021

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
  // Allocate memory for the new HashTableEntry struct on heap
  HashTableEntry * newHashTableEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
  
  if (!newHashTableEntry) {
    return NULL;  // returns NULL if there is no space on the heap
  }

  newHashTableEntry->key = key;  //  initialize the struct member key using key passed in
  newHashTableEntry->value = value;  //  initialize the struct member value using value passed in
  newHashTableEntry->next = NULL;  //  initialize the struct member next to NULL
  return newHashTableEntry;  // return the struct that was just created
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
  unsigned int bucketNum = hashTable->hash(key);  // calculate the key's bucketNum by taking the hash(key)
  HashTableEntry * curr = hashTable->buckets[bucketNum];  // get the head entry

  // while curr is not NULL, keep looping.
  while (curr != NULL) {
    if (curr->key == key) {
      return curr;  // return the HashTableEntry if the key is found
    }
    curr = curr->next;  // otherwise move to next node
  }
  return NULL;  // if not found, return NULL
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
  HashTableEntry ** bucketArr = hashTable->buckets;  // creates a pointer to the head of buckets
  int length = hashTable->num_buckets;  // calculates number of buckets
  for (unsigned int i = 0; i < length; i++) {  // loop to clear each index in buckets
    if (bucketArr[i] == NULL) {
      continue;  // if buckets[i] == NULL, continue. Note: NULL does not need to be freed.
    }
    if (bucketArr[i] != NULL) {  // enter loop if buckets[i] != NULL
      HashTableEntry * curr = bucketArr[i];  // sets a curr pointer to move through Linked List at buckets[i]
      HashTableEntry * nextEntry = curr->next;  // sets a pointer to the next hashTableEntry
      while (nextEntry != NULL) {  // while nextEntry not at end of Linked List
        free(curr->value);  // free the pointer at curr->value (must be done bc pointer in memory)
        free(curr);  // free the curr hashTableEntry altogether in memory
        curr = nextEntry;  // move the curr pointer to the nextEntry
        nextEntry = nextEntry->next;  // move our nextEntry to the nextEntry->next pointer
      }
      free(curr->value);  // free the pointer at curr->value (must be done bc pointer in memory)
      free(curr);  // frees the last remaining curr element once loop condition is false
    }
  }
  free(hashTable->buckets);  // frees the pointer to buckets from memory
  free(hashTable);  // frees all space taken up by hashTable
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
  HashTableEntry * itemPointer = findItem(hashTable, key);  // retrieves value from findItem to see if item was found
  // Case 1: Key is found in Linked List
  if (itemPointer != NULL) {  // if key is found in Linked List
    void* oldValue = itemPointer->value;  // sets the old Value into a varaible
    itemPointer->value = value;  // sets the found item's->next to the new value
    return oldValue;  // return oldValue
  }
  // Case 2: Key is not found in Linked List
  unsigned int bucketNum = hashTable->hash(key);  // performs a hashFunction on the key
  HashTableEntry * newNode = createHashTableEntry(key, value);  // creates a newNode using createHashTableEntry
  if (!newNode) {
    return NULL;  // returns NULL if there is no space on the heap and new entry was not created
  }
  newNode->next = hashTable->buckets[bucketNum];  // sets the newNode->next to the head of the linked list at buckets[bucketNum]
  hashTable->buckets[bucketNum] = newNode;  // sets the head of buckets[bucketNum] LinkedList to the newNode
  return NULL;  // return NULL
}

void* getItem(HashTable* hashTable, unsigned int key) {
  HashTableEntry * itemPointer = findItem(hashTable, key);  // performs a hashFunction on the key
  if (itemPointer != NULL) {  // if findItem returns the item in the linked list
    return itemPointer->value;  // return the foundItem's value
  }
  return NULL;  // returns NULL if findItem returns NULL 
}

void* removeItem(HashTable* hashTable, unsigned int key) {
  unsigned int bucketNum = hashTable->hash(key);  // calculate the key's bucketNum by taking the hash(key)
  HashTableEntry * curr = hashTable->buckets[bucketNum];  // get the head entry
  HashTableEntry * trailingPointer;  // declare the pointer to the previous node
  // Case 1: Linked List Empty: Head of Linked List is NULL
  if (curr == NULL) {
    return NULL;  // if curr == NULL, returns NULL and nothing was removed
  }
  // Case 2: Removal from Head of Linked List
  if (curr->key == key) {
    void* oldValue = curr->value;  // if key at head of linked list, set oldValue to curr->value
    hashTable->buckets[bucketNum] = curr->next;  // set the head of this bucket to curr->next
    free(curr);  // free the hashTableEntry curr
    return oldValue;  // return the oldValue
  }
  // Case 3/4: Removal from Middle/End of Linked List
  trailingPointer = curr;  // sets the trailing node of the current node (1 behind it)
  curr = curr->next;  // moves curr to curr->next
  while (curr != NULL) {  // loop continues while curr != NULL
    if (curr->key == key) {  // if the keys are equal, enter code below
      void* oldValue = curr->value;  // sets oldValue to curr->value
      trailingPointer->next = curr->next;  // sets the trailing pointer's next to curr->next (skips over curr in linked list)
      free(curr);  // frees space on heap used by curr
      return oldValue;  // returns the oldValue
    }
    trailingPointer = curr;  // sets trailing pointer to curr (move 1 forward)
    curr = curr->next;  // sets curr to curr->next (move 1 forward)
  }
  // Case 5: Key not found in Linked List
  return NULL;  // returns NULL if key is never found during the search
}

void deleteItem(HashTable* hashTable, unsigned int key) {
  free(removeItem(hashTable, key));  // performs all of the code in removeItem, while also freeing the value pointer returned.
}
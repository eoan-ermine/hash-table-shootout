#include <inttypes.h>
#include <string>
#include <map>

typedef std::map<int64_t, int64_t> hash_t;
typedef std::map<std::string, int64_t> str_hash_t;

#define SETUP hash_t hash; str_hash_t str_hash;

#define RESERVE_INT(size) ;
#define RESERVE_STR(size) ;
#define LOAD_FACTOR(map) 1

#define INSERT_RANGE_INTO_HASH(it1, it2) hash.insert(it1, it2)
#define INSERT_RANGE_INTO_STR_HASH(it1, it2) str_hash.insert(it1, it2)
#define INSERT_INT_INTO_HASH(key, value) hash.insert(hash_t::value_type(key, value))
#define DELETE_INT_FROM_HASH(key) hash.erase(key)
#define FIND_INT_EXISTING_FROM_HASH(key) if(hash.find(key) == hash.end()) { printf("error"); exit(1); }
#define FIND_INT_MISSING_FROM_HASH(key) if(hash.find(key) != hash.end()) { printf("error"); exit(2); }
#define FIND_INT_EXISTING_FROM_HASH_COUNT(key, count) \
    if(hash.find(key) != hash.end()) { count++; }
#define CHECK_INT_ITERATOR_VALUE(iterator, value) if(iterator->second != value) { printf("error"); exit(3); }

#define INSERT_STR_INTO_HASH(key, value) str_hash.insert(str_hash_t::value_type(key, value))
#define DELETE_STR_FROM_HASH(key) str_hash.erase(key);
#define FIND_STR_EXISTING_FROM_HASH(key) if(str_hash.find(key) == str_hash.end()) { printf("error"); exit(4); }
#define FIND_STR_MISSING_FROM_HASH(key) if(str_hash.find(key) != str_hash.end()) { printf("error"); exit(5); }
#define FIND_STR_EXISTING_FROM_HASH_COUNT(key, count) \
    if(str_hash.find(key) != str_hash.end()) { count++; }

#include "template.c"

#ifndef _LOCAL_DEF_H_
#define _LOCAL_DEF_H_
/*
* return code defination.
*/
#define RET_OK     0
#define RET_ERROR -1
#define RET_BUSY   2

#define MAX_SIZE_1K    1024
#define MAX_SIZE_2K    2048
#define MAX_SIZE_4K    4096
#define MAX_SIZE_64K   65546
#define MAX_SIZE_1M    1048576

#define VOID_CHECK(T) {if (nullptr == T) {return;}}
#define INT_CHECK(T) {if (nullptr == T) {return RET_ERROR;}}
#define PTR_CHECK(T) {if (nullptr == T) {return nullptr;}}
#define BOOL_CHECK(T) {if (nullptr == T) {return false;}}

#endif // _LOCAL_DEF_H_

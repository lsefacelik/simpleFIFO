/**
 ********************************************************************************
 * @file    fifo.h
 * @author  sefa.celik
 * @date
 * @brief
 ********************************************************************************
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********** Includes ***********************************************************/
#include<stdint.h>
#include<stddef.h>
#include<stdio.h>

/*********** Exported constants *************************************************/
#define FIFO_LEN 12000
#define FIFO_DATA_TYPE uint8_t

// Comment out to disable locking
#define LOCK_MECHANISM_ENABLED_DEFINED
#ifdef LOCK_MECHANISM_ENABLED_DEFINED
#define LOCK_MECHANISM_ENABLED 1
#else
#define LOCK_MECHANISM_ENABLED 0
#endif

#define PRINT_ENABLED
#ifdef PRINT_ENABLED
#define printd printf
#else
#define printd ((void *) 0)
#endif

/*********** Exported macros ****************************************************/
#if LOCK_MECHANISM_ENABLED
#define FIFO_LOCK(HANDLE) do{						       	    \
								if((HANDLE)->lock)				\
									return FIFO_LOCKED;			\
								else							\
									(HANDLE)->lock = 1;			\
							}while(0)

#define FIFO_UNLOCK(HANDLE) (HANDLE)->lock = 0;

#define IS_FIFO_LOCKED(HANDLE) do{				                \
								if((HANDLE)->lock)				\
									return;						\
								}while(0)
#else
#define FIFO_LOCK(HANDLE)		 ((void)0)
#define FIFO_UNLOCK(HANDLE) 	 ((void)0)
#define IS_FIFO_LOCKED(HANDLE)   ((void)0)
#endif

/*********** Exported types *****************************************************/
typedef enum{
	FIFO_ERROR = 0,
	FIFO_EMPTY,
	FIFO_FULL,
	FIFO_WRITE_SUCCESSFUL,
	FIFO_READ_SUCCESSFUL,
	FIFO_NOT_ENOUGH_SPACE,
	FIFO_NOT_ENOUGH_DATA,
#if LOCK_MECHANISM_ENABLED
	FIFO_LOCKED,
#endif
}fifo_ret_t;

typedef FIFO_DATA_TYPE fifo_data_t;

typedef struct{
	size_t head;
	size_t tail;
	size_t len;
	fifo_data_t buffer[FIFO_LEN];
	uint8_t full;
#if LOCK_MECHANISM_ENABLED
	uint8_t lock;
#endif
}fifo_t;

/*********** Exported variables *************************************************/

/*********** Exported functions *************************************************/
void fifo_init(fifo_t *);
fifo_ret_t fifo_read_single(fifo_t *, fifo_data_t *);
fifo_ret_t fifo_write_single(fifo_t *, fifo_data_t);
void fifo_print_buffer(fifo_t *);
size_t fifo_get_written_length(fifo_t *);
fifo_ret_t fifo_read(fifo_t *, fifo_data_t *, size_t);
fifo_ret_t fifo_write(fifo_t *, fifo_data_t *, size_t);

#ifdef __cplusplus
}
#endif

#endif

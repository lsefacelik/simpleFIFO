/**
 ********************************************************************************
 * @file    fifo.c
 * @author  sefa.celik
 * @date
 * @brief
 ********************************************************************************
 */

/*********** Includes ***********************************************************/
#include "fifo.h"
#include <string.h>

/*********** Private defines ****************************************************/

/*********** Private macros *****************************************************/

/*********** Private types ******************************************************/

/*********** Private variables **************************************************/

/*********** Exported variables *************************************************/

/*********** Imported variables *************************************************/

/*********** Private function prototypes ****************************************/

/*********** Private functions **************************************************/

/*********** Exported functions *************************************************/

/**
 *
 * @param fifo
 */
void fifo_init(fifo_t * fifo){
	if((fifo == NULL))
		return;

	fifo->len = FIFO_LEN;
	memset(fifo->buffer, 0, fifo->len * sizeof(fifo_data_t));
	fifo->head = 0;
	fifo->tail = 0;
	fifo->full = 0;
#if LOCK_MECHANISM_ENABLED
	fifo->lock = 0;
#endif
}

/**
 *
 * @param fifo
 * @param ret
 * @return
 */
fifo_ret_t fifo_read_single(fifo_t * fifo, fifo_data_t * ret){

	if((fifo == NULL) || (ret == NULL))
		return FIFO_ERROR;

	if((fifo->head == fifo->tail) && !(fifo->full)){
		return FIFO_EMPTY;
	}

	FIFO_LOCK(fifo);
	*ret = fifo->buffer[fifo->tail];
	fifo->buffer[fifo->tail] = 0;
	fifo->tail = (fifo->tail + 1) % fifo->len;
	fifo->full = 0;
	FIFO_UNLOCK(fifo);
	return FIFO_READ_SUCCESSFUL;
}

/**
 *
 * @param fifo
 * @param data
 * @return
 */
fifo_ret_t fifo_write_single(fifo_t * fifo, fifo_data_t data){

	if((fifo == NULL))
		return FIFO_ERROR;

	if(fifo->full){
		return FIFO_FULL;
	}

	FIFO_LOCK(fifo);
	fifo->buffer[fifo->head] = data;
	fifo->head = (fifo->head + 1) % fifo->len;

	if(fifo->head == fifo->tail)
		fifo->full = 1;

	FIFO_UNLOCK(fifo);
	return FIFO_WRITE_SUCCESSFUL;
}

/**
 *
 * @param fifo
 */
void fifo_print_buffer(fifo_t * fifo){

	if((fifo == NULL))
		return;

	IS_FIFO_LOCKED(fifo);
	printd("head index: %lu, tail index: %lu \n", fifo->head, fifo->tail);
	if(!fifo->full){
		for(size_t i = fifo->tail; i < fifo->head; (void) ((i++) % fifo->len)){
			printd("%d ", fifo->buffer[i]);
		}
	}
	else{
		for(size_t i = fifo->tail; i < fifo->tail + fifo->len; i++ ){
			printd("%d ", fifo->buffer[i]);
		}
	}
	printd("\n");
}

/**
 *
 * @param fifo
 * @return
 */
size_t fifo_get_written_length(fifo_t * fifo){

	if((fifo == NULL))
		return FIFO_ERROR;

	if(fifo->full)
		return fifo->len;
	else{
		if(fifo->head >= fifo->tail)
			return (fifo->head - fifo->tail);
		else
			return (fifo->len + fifo->head - fifo->tail);
	}
}

/**
 *
 * @param fifo
 * @param p_out
 * @param len
 * @return
 */
fifo_ret_t fifo_read(fifo_t * fifo, fifo_data_t * p_out, size_t len){


	if((fifo == NULL) || (p_out == NULL) || (len == 0) || (&p_out[len - 1] == NULL))
		return FIFO_ERROR;

	if(len > fifo_get_written_length(fifo))
		return FIFO_NOT_ENOUGH_DATA;

	FIFO_LOCK(fifo);

	if( fifo->head > fifo->tail){
		memcpy(p_out, &fifo->buffer[fifo->tail], len * sizeof(fifo_data_t));
		fifo->tail += len;
	}
	else{
		if((len + fifo->tail) > fifo->len){
			memcpy(p_out, &fifo->buffer[fifo->tail], (fifo->len - fifo->tail) * sizeof(fifo_data_t));
			memcpy(&p_out[fifo->len - fifo->tail], &fifo->buffer[0], (len - (fifo->len - fifo->tail)) * sizeof(fifo_data_t));
			fifo->tail = (len - (fifo->len - fifo->tail)) % fifo->len;
		}
		else{
			memcpy(p_out, &fifo->buffer[fifo->tail], (len) * sizeof(fifo_data_t));
			fifo->tail = (len + fifo->tail) % fifo->len;
		}
	}

	fifo->full = 0;
	FIFO_UNLOCK(fifo);
	return FIFO_READ_SUCCESSFUL;
}

/**
 * 
 * @param fifo
 * @param p_in
 * @param len
 * @return
 */
fifo_ret_t fifo_write(fifo_t * fifo, fifo_data_t * p_in, size_t len){

	if((fifo == NULL) || (p_in == NULL) || (len == 0) || (&p_in[len - 1] == NULL))
		return FIFO_ERROR;

	size_t available_len = fifo->len - fifo_get_written_length(fifo);
	if(len > available_len)
		return FIFO_NOT_ENOUGH_SPACE;

	FIFO_LOCK(fifo);

	if(len + fifo->head > fifo->len){
		memcpy(&fifo->buffer[fifo->head], p_in, (fifo->len - fifo->head) * sizeof(fifo_data_t));
		memcpy(fifo->buffer, &p_in[fifo->len - fifo->head], (len - (fifo->len - fifo->head)) * sizeof(fifo_data_t));
		fifo->head = len - (fifo->len - fifo->head);
	}
	else{
		memcpy(&fifo->buffer[fifo->head], p_in, len * sizeof(fifo_data_t));
		fifo->head = (fifo->head + len)%fifo->len;
	}


	if(fifo->head == fifo->tail)
		fifo->full = 1;

	FIFO_UNLOCK(fifo);
	return FIFO_WRITE_SUCCESSFUL;
}

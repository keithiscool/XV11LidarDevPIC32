#include "queue.h"
//"_" in front of a function designates that it is used within the struct definition for each DMA struct


struct _dmaSettings {
    unsigned char *dma_array;
    volatile unsigned int *dmacon;
    unsigned int con_busy_mask;
    unsigned int con_en_mask;
    volatile unsigned int *dmasize;
    volatile unsigned int *dmaecon;
    unsigned int econ_force_mask;
};


// function runs to initiate the queue system
// the function is handed all the tools it needs to work the dma for sending
void _queue_begin(struct dmaSettings *_settings, int module_number) {
    // move settings to internal variables
    DMA_Buffer_One.dma_array = _settings->dma_array;
    DMA_Buffer_One.dmacon = _settings->dmacon;
    DMA_Buffer_One.con_busy_mask = _settings->con_busy_mask;
    DMA_Buffer_One.con_en_mask = _settings->con_en_mask;
    DMA_Buffer_One.dmasize = _settings->dmasize;
    DMA_Buffer_One.dmaecon = _settings->dmaecon;
    DMA_Buffer_One.econ_force_mask = _settings->econ_force_mask;
    DMA_Buffer_One.module_ID = module_number;

    _queue_init(); //flush the queue buffer
}


// set everything to zeros
void _queue_init(void) {
    DMA_Buffer_One.send_queue.head = 0;
    DMA_Buffer_One.send_queue.tail = 0;
    DMA_Buffer_One.send_queue.count = 0;
    int i;
    for (i = 0; i < TOP_LEVEL_QUEUE_DEPTH; i++) {
        DMA_Buffer_One.send_queue.queue[i].count = 0;
        DMA_Buffer_One.send_queue.queue[i].head = 0;
        DMA_Buffer_One.send_queue.queue[i].tail = 0;
        memset(DMA_Buffer_One.send_queue.queue[i].buf, 0, SECOND_LEVEL_QUEUE_DEPTH);
    }
}


//void _queue_put(unsigned char *what, unsigned char how_many, unsigned char where, unsigned char from_where) {
void _queue_put(unsigned char *what, unsigned char how_many) {

    if (DMA_Buffer_One.send_queue.count < TOP_LEVEL_QUEUE_DEPTH) {
//        _queue_data_put(0x06);
//        _queue_data_put(0x85);
//        _queue_data_put(where);
//        _queue_data_put(from_where);
        _queue_data_put(how_many);
        int i;

        for (i = 0; i < (how_many + 1); i++) {
            _queue_data_put(what[i]);
//            _queue_data_put(what);
        }
        DMA_Buffer_One.send_queue.head = modulo_inc(DMA_Buffer_One.send_queue.head, TOP_LEVEL_QUEUE_DEPTH);
        DMA_Buffer_One.send_queue.count++;
    } else {
        _queue_data_put(how_many);
        int i;

        for (i = 0; i <= how_many; i++) {
            _queue_data_put(what[i]);
//            _queue_data_put(what);
        }
        DMA_Buffer_One.send_queue.head = modulo_inc(DMA_Buffer_One.send_queue.head, TOP_LEVEL_QUEUE_DEPTH);
        DMA_Buffer_One.send_queue.tail = modulo_inc(DMA_Buffer_One.send_queue.tail, TOP_LEVEL_QUEUE_DEPTH);
        DMA_Buffer_One.queueFullErrorCounter++; // because the queue is full to use this functions
    }
}


void _queue_data_put(unsigned char data) {
    if (DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].count < SECOND_LEVEL_QUEUE_DEPTH) {
        DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].buf[DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].head] = data;
        DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].head = modulo_inc(DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].head, SECOND_LEVEL_QUEUE_DEPTH);
        DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].count++;
    } else {
        DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].buf[DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].head] = data;
        DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].head = modulo_inc(DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].head, SECOND_LEVEL_QUEUE_DEPTH);
        DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].tail = modulo_inc(DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.head].tail, SECOND_LEVEL_QUEUE_DEPTH);
    }
}


unsigned char _queue_data_get(void) {
    unsigned char dat;
    if (DMA_Buffer_One.send_queue.count > 0) {
        if (DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].count > 0) {
            dat = DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].buf[DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].tail];
            DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].tail--;
            DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].count--;
        } else {
            dat = 0;
        }
    } else {
        dat = 0;
    }
    return dat;
}


unsigned int _modulo_inc(const unsigned int value, const unsigned int modulus) {
    unsigned int my_value = value + 1;
    if (my_value >= modulus) {
        my_value = 0;
    }
    return (my_value);
}


bool _queue_send(void) {
    if (DMA_Buffer_One.send_queue.count > 0) { // if send queue is empty no need to send
        if ((*DMA_Buffer_One.dmacon & DMA_Buffer_One.con_busy_mask) == 0) { // if dma is busy then no need to send
            // if all the checks pass populate the dma buffer, set the size, enable and force a send
            int g;
            int h = DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].count;

            for (g = 0; g < h; g++) {
                DMA_Buffer_One.dma_array[g] = DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].buf[g];
            }
                *DMA_Buffer_One.dmasize = h;
                *DMA_Buffer_One.dmacon |= DMA_Buffer_One.con_en_mask;
                *DMA_Buffer_One.dmaecon |= DMA_Buffer_One.econ_force_mask;
                // flush send queue
                DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].count = 0;
                DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].head = 0;
                DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].tail = 0;
                memset(DMA_Buffer_One.send_queue.queue[DMA_Buffer_One.send_queue.tail].buf, 0, SECOND_LEVEL_QUEUE_DEPTH);
                //tidy up tail and counter
                DMA_Buffer_One.send_queue.count--;
                DMA_Buffer_One.send_queue.tail = modulo_inc(DMA_Buffer_One.send_queue.tail, TOP_LEVEL_QUEUE_DEPTH);
                return true;

        }else {
            return false;
        }
    }else {
        return false;
    }
}


// returns the queue full error
unsigned int _fullError(void) {
    return DMA_Buffer_One.queueFullErrorCounter;
}
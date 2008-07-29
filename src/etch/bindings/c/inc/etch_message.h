/* 
 * etch_message.h 
 */

#ifndef ETCHMESSAGE_H
#define ETCHMESSAGE_H

#include "etch_structval.h"
#include "etch_valuefact.h"


/* 
 * etch_message
 */
struct etch_message
{
    unsigned short obj_type; 
    unsigned short class_id; 
    etch_structvalue* sval;
    etch_valuefact*   vfactory;
     
};

typedef struct etch_message etch_message;


/* 
 * constructors 
 */
etch_message* new_etch_message();
ETCH_MESSAGE* new_boxed_message();


#endif /* #ifndef ETCHMESSAGE_H*/ 
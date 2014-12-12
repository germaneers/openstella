
/*
 * configuration file for openstella: This file holds the default values.
 *
 * This file is included _after_ (your local copy of) openstellaconfig.h
 * It holds default values for all options that aren't overridden in openstellaconfig.h
 */


#ifndef OPENSTELLADEFAULTS_H_
#define OPENSTELLADEFAULTS_H_


#ifndef OPENSTELLA_UART_QUEUESIZE
#define OPENSTELLA_UART_QUEUESIZE 64
#endif

#ifndef OPENSTELLA_CANCONTROLLER_QUEUESIZE
#define OPENSTELLA_CANCONTROLLER_QUEUESIZE 32
#endif

#ifndef CANCONTROLLER_STACK_SIZE
#define CANCONTROLLER_STACK_SIZE 160
#endif

#endif //OPENSTELLADEFAULTS_H_



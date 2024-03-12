/**
 *
 */
#ifndef __KVMPROCESS_H
#define __KVMPROCESS_H


#include "kvmprocess_t.h"

void kvmProcess_init(KvmProcess *process);

sint_t krluserspace_accessfailed(addr_t fairvadrs);

void kvmProcess_test_main();

void kvm_init();


#endif //__KVMPROCESS_H
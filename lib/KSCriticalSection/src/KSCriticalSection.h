#ifndef _KSCRITICALSECTION_H
#define _KSCRITICALSECTION_H

#include <Arduino.h>


// definition of bits for KSCriticalSection
#define KSCRITICALSECTION_FREE (1UL << 0UL)   // zero shift for bit0



class KSCriticalSection {
    public:
        KSCriticalSection();
        ~KSCriticalSection();
        
        void EnterCriticalSection();
        void LeaveCriticalSection();

    private:
        EventGroupHandle_t _hEventGroup = NULL;
};





#endif  // #define _KSCRITICALSECTION_H
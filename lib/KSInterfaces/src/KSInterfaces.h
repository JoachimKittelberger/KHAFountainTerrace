#ifndef _KSINTERFACES_H
#define _KSINTERFACES_H



//#warning IKSxxx Interfaces were former KSIxxx Interfaces. Please change Interface names in your projects.


class IKSBatteryPower {
    public:
        virtual float getVoltage() = 0;
        virtual float getBatteryPercentage() = 0;
};


class IKSTemperature {
    public:
        virtual float getTemperature() = 0;
};


class IKSHumidity {
    public:
        virtual float getHumidity() = 0;
};






// Test Verwendung von abstrakte Basis-Klassen
/*
class BaseA {
  public:
//    virtual void DoSomethingA() = 0;      // für interface als abstrakte Base class notwendig
    virtual void DoSomethingA() {};
    virtual void DoSomethingA1() {};
    virtual ~BaseA() {};   // damit destruktor sauber aufgerufen wird
};

class BaseB {
  public:
//    virtual void DoSomethingB() = 0;      // für interface als abstrakte Base class notwendig
    virtual void DoSomethingB() {};
    virtual ~BaseB() {};   // damit destruktor sauber aufgerufen wird
};

//class Component : public virtual BaseA, public virtual BaseB {    // nur notwendig, wenn gleich Basisklassen und damit mehrdeutigleit der Funktionen.
  // Dann wird nur eine Funktion übernommen, die im abgeleiteter Klasse überschrieben werden kann oder wenn schon implementiert, nicht muss.
class Component : public BaseA, public BaseB {
  public:
    //virtual void DoSomethingA() {};
    //virtual void DoSomethingB() {};
    
    virtual ~Component() {};    // damit destruktor sauber aufgerufen wird
};


void setup() {
  // put your setup code here, to run once:
  Component componente;
  componente.DoSomethingA();
  componente.DoSomethingB();

  BaseA* pBaseA = &componente;
  pBaseA->DoSomethingA();
  pBaseA->DoSomethingA1();

  BaseB* pBaseB = &componente;
  pBaseB->DoSomethingB();

  Component* pComponent = &componente;
  pComponent->DoSomethingA();
  pComponent->DoSomethingB();
  pComponent->DoSomethingA1();

}

void loop() {
  // put your main code here, to run repeatedly:

}
*/



#endif          // #define _KSINTERFACES_H

#pragma once

#include <ESPTelnet.h>



/* override ESPTelnet and add support for printf-Funcitonality */

class KSESPTelnet : public ESPTelnet {

  public:
    KSESPTelnet() : ESPTelnet() {}


    size_t write(const uint8_t *buf, size_t size) {
      size_t ret = 0;
      ret = client.write(buf, size);
      return ret;
    }
    void flush() {
      client.flush();
    }

};

  // << operator
  //template<class T> inline KSESPTelnet &operator <<(KSESPTelnet &obj, T arg) { obj.print(arg); return obj; } 


#pragma once

#include <OnRequest.h>
#include <JsonServer.h>

class DummyBS : public OnRequest
{
  public:
    DummyBS(const std::string& x);
    virtual ~DummyBS();

  private:
    ptree myData;
    void getHumidity();
    void clearData();
    void sendUpdates();
};

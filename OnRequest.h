#include "ValueUpdater.h"

class OnRequest : public ValueUpdater
{
  public:
    OnRequest(const std::string& x);
    virtual ~OnRequest();
  
  private:
    OnRequest();

    const JsonServer::ptree updateMe(const JsonServer::ptree& x);
};
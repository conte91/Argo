#include "ValueUpdater.h"

class OnRequest : public ValueUpdater
{
  public:
    OnRequest(const std::string& x);
  
  private:
    OnRequest();

    const JsonServer::ptree updateMe(const JsonServer::ptree& x);

};

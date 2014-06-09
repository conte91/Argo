#include "OnRequest.h"

OnRequest::OnRequest(const std::string& x) : ValueUpdater(x)
{
  _jServer.handle("Update", boost::bind(&OnRequest::updateMe, this, _1));
}

const JsonServer::ptree OnRequest::updateMe(const JsonServer::ptree& x){
  std::cout << "Updating on request" << std::endl;
  _tUpdater.manualNotify();
  JsonServer::ptree res;
  res.put("Update", "Done");
  return res;
}

OnRequest::~OnRequest(){
}

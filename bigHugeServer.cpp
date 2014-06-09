#include "JsonServer.h"
#include "JsonManager.h"
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

JsonServer::ptree logSunlight(const JsonServer::ptree& x){
  std::cout << "Hey i'm recording a sunlight of " << x.get<double>("Value") << " recorded at time " << x.get<std::string>("Time") << std::endl;
  JsonServer::ptree t;
  t.put("ACK", "ACK");
  return t;
}

JsonServer::ptree logHumidity(const JsonServer::ptree& x){
  std::cout << "Hey i'm recording a humidity of " << x.get<double>("Data") << " recorded at time " << x.get<std::string>("Time") << std::endl;
  JsonServer::ptree t;
  t.put("ACK", "ACK");
  return t;
}

int main(){
  using namespace boost::asio::ip;

  JsonServer s("bigServer", 4243);
  s.handle("Humidity", &logHumidity);
  s.handle("Sunlight", &logSunlight);
  std::string str;

  JsonServer::ptree updateRequest;
  updateRequest.put("id", "test");
  updateRequest.put("Data.Update", "Now");

  while(1){
    std::cin >> str;
    if(str=="Update"){
      boost::asio::io_service io_service;
      tcp::socket socket(io_service);
      socket.connect(tcp::endpoint(address::from_string("127.0.0.2"), 4242));
      JsonManager::writeJson(socket, updateRequest);
    }
  }
  return 0;
}

/*
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

/**
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "JsonServer.h"
#include "JsonManager.h"

#include <string>

#include <utility>
#include <vector>

#include <boost/asio.hpp> 
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <iostream> 
#include <sstream>

JsonServer::JsonServer(const std::string& value, int port)
  :
    /** Note: from C++ standard, class members are initialized in the order in which they are declared *into* the class declaration
    */
    _networkThread(boost::bind(&JsonServer::networkServer, this, port))
{
  myID=value;
}

void JsonServer::spin() {
  _networkThread.join();
}


void JsonServer::networkServer(int port) {

  using boost::asio::ip::tcp;
  using boost::property_tree::json_parser::write_json;


  boost::asio::io_service io_service;

  /** TCP listener on port 4242 */
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
  std::cout << "Listening on port "<< port << ".." << std::endl;

  while(1){
    /** This buffer is cleared each iteration by RAII */

    tcp::socket socket(io_service);
    acceptor.accept(socket);

    try{
      JsonManager::readJson(socket, requests);

      /** Elaborates an answer */
      manageJSONRequest(socket);

      /** Does something if specified */
      if(postHook){
        postHook(answers);
      }
      JsonManager::writeJson(socket, answers);

    }
    catch(boost::system::system_error e){
      /**Nevermind*/
      std::cerr << "Eccezione: " << e.what() << " - " << e.code() << std::endl;
    }
  }

}

void JsonServer::manageJSONRequest(boost::asio::ip::tcp::socket& socket){

  using boost::property_tree::write_json;

  answers.erase("");

  if(!requests.get_optional<std::string>("id")){
    /**Sends an error */
    answers.put("BadRequest.Reason", "MissingID");
    write_json(std::cout, answers);
    return;
  }
  if(!requests.get_child_optional("Data")){
    /**Sends an error */
    answers.put("BadRequest.Reason", "MissingDATA");
    write_json(std::cout, answers);
    return;
  }

  if(requests.get<std::string>("id")==myID){
    std::cout << "I'm there" << std::endl;

    BOOST_FOREACH(ptree::value_type& v, requests.get_child("Data")) {
      std::cout << "Managing: " << v.first << std::endl;
      /** Manages the request */
      if(!(handlers.count(v.first))){
        /** There is no function associated to this request */
        if(defaultHandler){
          answers.put_child(std::string("Data.")+v.first, defaultHandler(v.second));
        }
      }
      else{
        /** This handlers are soo LISP*/
        const DataHandler& h=handlers[v.first];
        answers.put_child(std::string("Data.")+v.first, h(v.second));
      }
    }
  }
  return;
}

void JsonServer::handle(const std::string& node, const JsonServer::DataHandler& handler){
  handlers[node]=handler;
}

void JsonServer::setDefaultHandler(const JsonServer::DataHandler& handler){
  defaultHandler=handler;
}

void JsonServer::setPostHook(const JsonServer::Hook& handler){
  postHook=handler;
}


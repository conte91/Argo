
#include "JsonServer.h"

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

JsonServer::JsonServer(const std::string& value)
  :
    /** Note: from C++ standard, class members are initialized in the order in which they are declared *into* the class declaration
    */
    _networkThread(boost::bind(&JsonServer::networkServer, this))
{
  myID=value;
}

void JsonServer::spin(){
  _networkThread.join();
}


void JsonServer::networkServer(){

  using boost::asio::ip::tcp;
  using boost::property_tree::json_parser::write_json;

  std::cout << "Network thread detached" << std::endl;

  boost::asio::io_service io_service;

  /** TCP listener on port 4242 */
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 4242));

  while(1){
    /** This buffer is cleared each iteration by RAII */
    boost::asio::streambuf message;

    tcp::socket socket(io_service);
    acceptor.accept(socket);

    try{
      currentSocket=&socket;
      boost::asio::read_until(socket, message, boost::bind(&JsonServer::jsonReceived, this, _1, _2));

      /** Elaborates an answer */
      manageJSONRequest(socket);

      /** Does something if specified */
      if(postHook){
        postHook(answers);
      }

    }
    catch(boost::system::system_error e){
      /**Nevermind*/
      std::cerr << "Eccezione:" << e.code() << std::endl;
    }
  }

}

/** Used to check for a complete JSON message */
std::pair<JsonServer::BufIterator, bool> JsonServer::jsonReceived(JsonServer::BufIterator begin, JsonServer::BufIterator end){

  using boost::asio::buffers_iterator;
  using boost::asio::streambuf;
  using boost::asio::basic_streambuf;
  using boost::property_tree::json_parser::read_json;
  using boost::property_tree::json_parser_error;

  /** Initialize our JSON-buffer :
   * Copy buffer to a string to be parsed 
   **/
  std::string readBufStr="";
  for(BufIterator i=begin; i!=end; ++i){
    readBufStr.push_back(*i);
  }

  /** Try to parse the received buffer as a JSON object */
  try{
    std::istringstream stream(readBufStr);
    read_json(stream, this->requests);
    return std::make_pair(end, true);
  }
  catch(json_parser_error){
    /** Go on reading if message is not complete */
    /*std::cout << "Stamazza: \"" << readBufStr << "\"" << std::endl;*/
  }

  return std::make_pair(begin, false);

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
      std::cout << "Type: " << v.first << " value: " << v.second.data() << std::endl;
      /** Manages the request */
      if(!(handlers.count(v.second.data()))){
        /** There is no function associated to this request */
        if(defaultHandler){
          answers.put_child(std::string("Data.")+v.second.data(), defaultHandler(v.second));
        }
      }
      else{
        /** This handlers are soo LISP*/
        const DataHandler& h=handlers[v.second.data()];
        answers.put_child(std::string("Data.")+v.second.data(), h(v.second));
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

void JsonServer::sendTree(const JsonServer::ptree& x){
      std::ostringstream ansStream;
      /** 3rd parameter is for compact output */
      write_json(ansStream, answers, false);
      /*Debug:
       * write_json(std::cout, answers);*/
      ansStream.flush();
      boost::asio::write(*currentSocket, boost::asio::buffer(ansStream.str()));
}

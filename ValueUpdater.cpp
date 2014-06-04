#include "ValueUpdater.h"

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

ValueUpdater::ValueUpdater(const std::string& value)
  :
    /** Note: from C++ standard, class members are initialized in the order in which they are declared *into* the class declaration
    */
    _updateMutex(),
    _updateLock(_updateMutex),
    _updateCond(),

    _timeFrameThread(boost::bind(&ValueUpdater::timeFrame, this)),
    _networkThread(boost::bind(&ValueUpdater::networkServer, this)),
    _updateThread(boost::bind(&ValueUpdater::updateFunc, this))
{
  myID=value;

}

void ValueUpdater::updateFunc(){

  std::cout << "Detached new updater thread" << std::endl;

  while(1)
  {
    /** Waits for an event */
    _updateCond.wait(_updateLock);
    do_something();
  }

}

void ValueUpdater::do_something(){
  std::cout << "Hey i'm doing something" << std::endl;
}

void ValueUpdater::timeFrame(){
  boost::asio::io_service ioservice;
  boost::asio::deadline_timer t(ioservice, boost::posix_time::seconds(3));

  while(1)
  {
    t.wait();
    t.expires_at(t.expires_at()+boost::posix_time::seconds(3));
    std::cout << "Signaling time frame" << std::endl;
    _updateCond.notify_one();
  }
}


void ValueUpdater::spin(){
  _timeFrameThread.join();
  _networkThread.join();
  _updateThread.join();
}


void ValueUpdater::networkServer(){

  using boost::asio::ip::tcp;
  using boost::property_tree::json_parser::write_json;

  std::cout << "Network thread detached" << std::endl;

  boost::asio::io_service io_service;

  /** TCP listener on port 4242 */
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 4242));

  while(1){
    /** This buffer is cleared each iteration by RAII */
    boost::asio::streambuf message;
    std::ostringstream ansStream;

    tcp::socket socket(io_service);
    acceptor.accept(socket);

    try{
      boost::asio::read_until(socket, message, boost::bind(&ValueUpdater::jsonReceived, this, _1, _2));

      /** Sends answer to the server */
      manageJSONRequest(socket);
      /** 3rd parameter for compact output */
      write_json(ansStream, answers, false);
      /*Debug:
       * write_json(std::cout, answers);*/
      ansStream.flush();
      boost::asio::write(socket, boost::asio::buffer(ansStream.str()));
    }
    catch(boost::system::system_error e){
      /**Nevermind*/
      std::cerr << "Eccezione:" << e.code() << std::endl;
    }
  }

}

/** Used to check for a complete JSON message */
std::pair<ValueUpdater::BufIterator, bool> ValueUpdater::jsonReceived(ValueUpdater::BufIterator begin, ValueUpdater::BufIterator end){

  using boost::property_tree::ptree;
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

void ValueUpdater::manageJSONRequest(boost::asio::ip::tcp::socket& socket){

  using boost::property_tree::ptree;
  using boost::property_tree::write_json;

  answers.erase("");

  if(!requests.get_optional<std::string>("id")){
    /**Sends an error */
    answers.put("BadRequest.Reason", "MissingID");
    write_json(std::cout, answers);
    return;
  }
  if(!requests.get_child_optional("req")){
    /**Sends an error */
    answers.put("BadRequest.Reason", "MissingREQ");
    write_json(std::cout, answers);
    return;
  }

  if(requests.get<std::string>("id")==myID){
    std::cout << "I'm there" << std::endl;

    BOOST_FOREACH(ptree::value_type& v, requests.get_child("req")) {
      std::cout << "Type: " << v.first << " value: " << v.second.data() << std::endl;
      /** Manages the request */
      if(!(handlers.count(v.second.data()))){
        /** There is no function associated to this request */
        const boost::function<ptree (std::string)>& h=boost::bind(&ValueUpdater::defaultHandler, this, _1);
        answers.put_child(std::string("Data.")+v.second.data(), h(v.second.data()));
      }
      else{
        /** This handlers are soo LISP*/
        const boost::function<ptree (std::string)>& h=handlers.find(v.second.data())->second;
        answers.put_child(std::string("Data.")+v.second.data(), h(v.second.data()));
      }
    }
  }
  return;
}

const boost::property_tree::ptree ValueUpdater::defaultHandler(std::string x){

  using boost::property_tree::ptree;

  ptree t;
  t.put("Success", "False");
  t.put("Error", std::string("There is no ")+x+std::string(" on this system"));

  return t;

}

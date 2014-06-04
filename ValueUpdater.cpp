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
    _updateThread(boost::bind(&ValueUpdater::updateFunc, this)),
    jServer(value)
{
  myID=value;
  jServer.setDefaultHandler(boost::bind(&ValueUpdater::errorNotFound, this, _1));
  jServer.setPostHook(boost::bind(&ValueUpdater::sendBackData, this, _1));
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
  jServer.spin();
  _updateThread.join();
}

void ValueUpdater::sendBackData(const ValueUpdater::ptree& x){
  jServer.sendTree(x);
}

const ValueUpdater::ptree ValueUpdater::errorNotFound(const ValueUpdater::ptree& x){
  ptree t;
  t.put("Success", "False");
  t.put("Error", std::string("There is no ")+x.data()+std::string(" on this system"));

  return t;
}

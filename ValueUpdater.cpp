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
    _tUpdater(boost::posix_time::seconds(3)),
    _jServer(value)
{
  myID=value;
  _jServer.setDefaultHandler(boost::bind(&ValueUpdater::errorNotFound, this, _1));

  _jServer.setPostHook(boost::bind(&ValueUpdater::sendBackData, this, _1));

  /** List of things to be done */
  _tUpdater.toDo().push(boost::bind(&ValueUpdater::do_something, this));

}

void ValueUpdater::do_something(){
  std::cout << "Hey i'm doing something" << std::endl;
}


void ValueUpdater::spin(){
  _jServer.spin();
  _tUpdater.spin();
}

void ValueUpdater::sendBackData(const ValueUpdater::ptree& x){
  _jServer.sendTree(x);
}

const ValueUpdater::ptree ValueUpdater::errorNotFound(const ValueUpdater::ptree& x){
  ptree t;
  t.put("Success", "False");
  t.put("Error", std::string("There is no ")+x.data()+std::string(" on this system"));

  return t;
}


ValueUpdater::~ValueUpdater(){}

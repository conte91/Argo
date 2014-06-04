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

#include "Timer.h"

Timer::Timer(const Timer::Time& t)
  :
    /** Note: from C++ standard, class members are initialized in the order in which they are declared *into* the class declaration
    */
    _updateMutex(),
    _updateLock(_updateMutex),
    _updateCond(),

    _timeFrameThread(boost::bind(&Timer::timeFrame, this)),
    _updateThread(boost::bind(&Timer::updateFunc, this)),
    _updateTime(t)
{
}

void Timer::updateFunc(){

  std::cout << "Detached new updater thread" << std::endl;

  while(1)
  {
    /** Waits for an event */
    _updateCond.wait(_updateLock);
    funcs.execute();
  }

}

Timer::Handlers& Timer::toDo(){
  return funcs;
}

void Timer::timeFrame(){
  boost::asio::io_service ioservice;
  boost::asio::deadline_timer t(ioservice, _updateTime);

  while(1)
  {
    t.wait();
    t.expires_at(t.expires_at()+_updateTime);
    std::cout << "Signaling time frame" << std::endl;
    _updateCond.notify_one();
  }
}

void Timer::manualNotify(){
  _updateCond.notify_one();
}

void Timer::spin(){
  _timeFrameThread.join();
  _updateThread.join();
}

void Timer::Handlers::push(boost::function<void (void)> x){
  funcs.push_back(x);
}

void Timer::Handlers::execute(){
  for(std::vector<boost::function<void (void)> >::iterator i=funcs.begin(); i!=funcs.end(); ++i){
    (*i)();
  }
}


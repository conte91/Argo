#pragma once

#include <string>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/property_tree/ptree.hpp>
#include "JsonServer.h"

class ValueUpdater{
  public:
    ValueUpdater(const std::string& id);

    void spin();
    void startUpdate();
    void stopUpdate();

  private:

    typedef boost::property_tree::ptree ptree;

    ValueUpdater();

    std::string myID;

    JsonServer jServer;

    /** Cycles, signaling condition every time frame */
    void timeFrame();

    /** Listens to condition variable */
    void updateFunc();
    void do_something();

    void sendBackData(const ptree& x);
    const ptree errorNotFound(const ptree& x);

    /** These fields are necessary for the update condition to be created */
    boost::mutex _updateMutex;
    boost::unique_lock<boost::mutex> _updateLock;
    boost::condition_variable _updateCond;
    
    boost::thread _timeFrameThread,  _updateThread;

};


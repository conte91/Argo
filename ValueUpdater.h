#pragma once

#include <string>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/property_tree/ptree.hpp>
#include "JsonServer.h"
#include "Timer.h"

class ValueUpdater{
  public:
    ValueUpdater(const std::string& id);
    virtual ~ValueUpdater();

    void spin();
    void startUpdate();
    void stopUpdate();

  protected:

    typedef boost::property_tree::ptree ptree;

    ValueUpdater();

    std::string myID;

    JsonServer _jServer;
    Timer _tUpdater;

    void do_something();

    const ptree errorNotFound(const ptree& x);
};


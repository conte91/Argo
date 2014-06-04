#pragma once

#include <string>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/property_tree/ptree.hpp>

class Timer{
  public:
    typedef boost::posix_time::time_duration Time;

    Timer(const Time& t);

    class Handlers{
      public:
        void push(boost::function<void (void)>);
        void execute();
      private:
        std::vector<boost::function<void (void)> > funcs;
    };

    void spin();
    Handlers& toDo();
    void startUpdate();
    void stopUpdate();

    /** Manually triggers an execution of the timer */
    void manualNotify();

  private:

    Timer();

    /** Cycles, signaling condition every time frame */
    void timeFrame();

    Handlers funcs;

    /** Listens to condition variable */
    void updateFunc();

    /** These fields are necessary for the update condition to be created */
    boost::mutex _updateMutex;
    boost::unique_lock<boost::mutex> _updateLock;
    boost::condition_variable _updateCond;
    
    boost::thread _timeFrameThread,  _updateThread;

    Time _updateTime;

};


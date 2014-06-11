/**
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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


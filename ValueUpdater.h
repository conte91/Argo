#pragma once

#include <string>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/property_tree/ptree.hpp>

class ValueUpdater{
  public:
    ValueUpdater(const std::string& id);

    void spin();
    void startUpdate();
    void stopUpdate();

  private:

    ValueUpdater();

    std::string myID;

    boost::property_tree::ptree requests, answers;

    /** This will store association between requests and actions */
    std::map<std::string, boost::function<const boost::property_tree::ptree (std::string)> > handlers;

    typedef boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> BufIterator;
    /** Used to check for a complete JSON message */
    std::pair<BufIterator, bool> jsonReceived(BufIterator begin, BufIterator end);

    /** Cycles, signaling condition every time frame */
    void timeFrame();

    /** Simple network server */
    void networkServer();

    /** Listens to condition variable */
    void updateFunc();
    void do_something();

    void manageJSONRequest(boost::asio::ip::tcp::socket& socket);

    const boost::property_tree::ptree defaultHandler(std::string x);
    /*boost::asio::io_service _ioService;*/
    /*boost::asio::deadline_timer _timer;*/

    /** These fields are necessary for the update condition to be created */
    boost::mutex _updateMutex;
    boost::unique_lock<boost::mutex> _updateLock;
    boost::condition_variable _updateCond;
    
    boost::thread _timeFrameThread, _networkThread, _updateThread;

};


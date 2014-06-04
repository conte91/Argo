#pragma once

#include <string>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/property_tree/ptree.hpp>

class JsonServer{
  private:
    typedef boost::property_tree::ptree ptree;
    typedef boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> BufIterator;
    typedef boost::function<const ptree (const ptree&)> DataHandler;
    typedef boost::function<void (const ptree&)> Hook;

  public:

    JsonServer(const std::string& id);

    void spin();

    /** Assigns an handler to a JSON request */
    void handle(const std::string&, const DataHandler&);

    /** Functions that will be run when nothing is available or when the conversion is complete */
    void setDefaultHandler(const DataHandler& handler);

    void setPostHook(const Hook& handler);

    void sendTree(const JsonServer::ptree& x);

  private:

    JsonServer();

    std::string myID;

    ptree requests, answers;

    /** This will store association between requests and actions */
    std::map<std::string, DataHandler > handlers;

    /** The default data handler for this thing */
    DataHandler defaultHandler;

    /** Handler that will receive the complete answer and use it as it wants */
    Hook postHook;

    /** Used to check for a complete JSON message */
    std::pair<BufIterator, bool> jsonReceived(BufIterator begin, BufIterator end);

    /** Simple network server */
    void networkServer();

    void manageJSONRequest(boost::asio::ip::tcp::socket& socket);

    boost::thread _networkThread;

    boost::asio::ip::tcp::socket* currentSocket;

};

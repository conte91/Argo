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

class JsonServer{

  public:
    typedef boost::property_tree::ptree ptree;

  private:
    typedef boost::function<const ptree (const ptree&)> DataHandler;
    typedef boost::function<void (const ptree&)> Hook;

  public:

    JsonServer(const std::string& id, int port=4242);

    void spin();

    /** Assigns an handler to a JSON request */
    void handle(const std::string&, const DataHandler&);

    /** Functions that will be run when nothing is available or when the conversion is complete */
    void setDefaultHandler(const DataHandler& handler);

    void setPostHook(const Hook& handler);

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

    /** Simple network server */
    void networkServer(int port);

    void manageJSONRequest(boost::asio::ip::tcp::socket& socket);

    boost::thread _networkThread;

};

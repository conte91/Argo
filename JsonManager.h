/**
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include <boost/ref.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/property_tree/json_parser.hpp>

class JsonManager{

  public:
    typedef boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> BufIterator;
    typedef boost::property_tree::ptree ptree;
    virtual ~JsonManager()=0;

    template<class Reader>
      static void readJson(Reader& socket, JsonManager::ptree& result);

    template<class Writer>
      static void writeJson(Writer& socket, const JsonManager::ptree& result);

  private:
    /** Used to check for a complete JSON message */
    JsonManager();
    static
      std::pair<BufIterator, bool> jsonReceived(ptree& requests, BufIterator begin, BufIterator end);


};

template<class Reader>
void JsonManager::readJson(Reader& socket, JsonManager::ptree& result){
  boost::asio::streambuf message;
  boost::asio::read_until(socket, message, boost::bind(&JsonManager::jsonReceived, boost::ref(result), _1, _2));
}

template<class Writer>
void JsonManager::writeJson(Writer& socket, const JsonManager::ptree& result){
  std::ostringstream ansStream;
  /** 3rd parameter is for compact output */
  write_json(ansStream, result, false);
  /*Debug:
   * write_json(std::cout, results);*/
  ansStream.flush();
  boost::asio::write(socket, boost::asio::buffer(ansStream.str()));
}

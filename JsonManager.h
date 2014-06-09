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

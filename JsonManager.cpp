#include <boost/asio.hpp>
#include <algorithm>

#include "JsonManager.h"

/** Used to check for a complete JSON message */
std::pair<JsonManager::BufIterator, bool> JsonManager::jsonReceived(JsonManager::ptree& requests, JsonManager::BufIterator begin, JsonManager::BufIterator end){

  using boost::asio::buffers_iterator;
  using boost::asio::streambuf;
  using boost::asio::basic_streambuf;
  using boost::property_tree::json_parser::read_json;
  using boost::property_tree::json_parser_error;

  /** Initialize our JSON-buffer :
   * Copy buffer to a string to be parsed 
   **/
  std::string readBufStr="";
  for(BufIterator i=begin; i!=end; ++i){
    readBufStr.push_back(*i);
  }

  /** Try to parse the received buffer as a JSON object */
  try{
    std::istringstream stream(readBufStr);
    read_json(stream, requests);
    return std::make_pair(end, true);
  }
  catch(json_parser_error){
    /** Go on reading if message is not complete */
  }

  return std::make_pair(begin, false);

}


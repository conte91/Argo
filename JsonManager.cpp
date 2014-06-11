/**
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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


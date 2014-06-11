/**
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "ValueUpdater.h"

#include <string>

#include <utility>
#include <vector>

#include <boost/asio.hpp> 
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <iostream> 
#include <sstream>

ValueUpdater::ValueUpdater(const std::string& value)
  :
    /** Note: from C++ standard, class members are initialized in the order in which they are declared *into* the class declaration
    */
    _tUpdater(boost::posix_time::seconds(3)),
    _jServer(value)
{
  myID=value;
  _jServer.setDefaultHandler(boost::bind(&ValueUpdater::errorNotFound, this, _1));

  /** List of things to be done */
  _tUpdater.toDo().push(boost::bind(&ValueUpdater::do_something, this));

}

void ValueUpdater::do_something(){
  std::cout << "Hey i'm doing something" << std::endl;
}


void ValueUpdater::spin(){
  _jServer.spin();
  _tUpdater.spin();
}

const ValueUpdater::ptree ValueUpdater::errorNotFound(const ValueUpdater::ptree& x){
  ptree t;
  t.put("Success", "False");
  t.put("Error", std::string("There is no ")+x.data()+std::string(" on this system"));

  return t;
}


ValueUpdater::~ValueUpdater(){}

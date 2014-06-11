/**
 *
Copyright (C) 2014 Simone Baratta - Conte91 <at> gmail <dot> com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "DummyBS.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <JsonManager.h>

DummyBS::DummyBS(const std::string& x) : OnRequest(x) {
  _tUpdater.toDo().push(boost::bind(&DummyBS::clearData, this));
  _tUpdater.toDo().push(boost::bind(&DummyBS::getHumidity, this));
  _tUpdater.toDo().push(boost::bind(&DummyBS::sendUpdates, this));
}

DummyBS::~DummyBS(){}

void DummyBS::clearData(){
  myData.clear();
  myData.put("id", "bigServer");
}

void DummyBS::getHumidity(){
  using boost::gregorian::date;
  using boost::gregorian::day_clock;
  myData.put("Data.Humidity.Data", "5");
  myData.put("Data.Humidity.Time", boost::gregorian::to_iso_string(date(day_clock::universal_day())));
}

void DummyBS::sendUpdates(){
  using namespace boost::asio::ip;
  JsonManager::ptree t;
  boost::asio::io_service io_service;
  tcp::socket socket(io_service);
  socket.connect(tcp::endpoint(address::from_string("127.0.0.1"), 4243));
  std::cout << "Sending " ;
  boost::property_tree::json_parser::write_json(std::cout, myData);
  std::cout << std::endl;
  JsonManager::writeJson(socket, myData);
  JsonManager::readJson(socket, t);
  std::cout << "Received ";
  boost::property_tree::json_parser::write_json(std::cout, t);
  std::cout << std::endl;
}

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

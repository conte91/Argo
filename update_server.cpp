#include <boost/asio.hpp> 
#include <boost/asio/signal_set.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>
#include <iostream> 

/** Scheme of this server:
 *
 *            _Time frame elapsed
 *           v
 * updateFunc 
 *           ^_User event received<----Network request received
 *
 */
boost::asio::deadline_timer* t;
boost::asio::io_service* io_service; 

boost::mutex _updateMutex;
boost::unique_lock<boost::mutex> _updateLock;
boost::condition 

boost::function0<void> updateFunc;
// Construct a signal set registered for process termination.
boost::asio::signal_set* signals;

void periodicUpdate(const boost::system::error_code &ec, int signal_number) 
{ 
  std::cout << "Periodic update" << std::endl;
  t->expires_at(t->expires_at()+boost::posix_time::seconds(3));
  t->async_wait(periodicUpdate); 
  updateFunc();
} 

/*
   _communismCond.notify_one();
   _timeFrameDriverMutex(),
   _timeFrameDriverLock(_timeFrameDriverMutex),
   _timeFrameCond.wait(_timeFrameDriverLock);
   _timeFrameCond.notify_all();
   */

void signalUpdate(const boost::system::error_code &ec){
  std::cout << "Signal update" << std::endl;
  // Start an asynchronous wait for one of the signals to occur.
  signals.async_wait(signalUpdate);
  updateFunc();
}

void something(){
  std::cout<< "Ciao"<<std::endl;
}

int main() 
{ 
  updateFunc=something;
  io_service=new boost::asio::io_service();

  signals=new boost::asio::signal_set(*io_service, SIGUSR1);

  t=new boost::asio::deadline_timer((*io_service), boost::posix_time::seconds(3));

  /** This thread will set the time frames */
  boost::thread timewatchThread(
  t->async_wait(&periodicUpdate); 
  // Start an asynchronous wait for one of the signals to occur.
  signals.async_wait(signalUpdate);
  //boost::thread thread1(periodicUpdate); 
  io_service->run(); 

  delete t;
  delete signals;
  delete io_service;
} 

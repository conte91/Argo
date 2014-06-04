#include <string>
#include "ValueUpdater.h"
#include "OnRequest.h"

int main(){
  ValueUpdater* up;
  up=new OnRequest("test");
  up->spin();
  return 0;
}

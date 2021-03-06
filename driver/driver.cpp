// driver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <memory>
#include <iostream>
#include "brave_rewards_service_impl.h"
#include <curl/curl.h>

int main()
{
  //init
  curl_global_init(CURL_GLOBAL_ALL);

  try
  {
    //test 1: CreateWallet//////////////////////////////////////////////
    std::cout << std::endl << "TEST 1: init() started" << std::endl;
    Profile profile;
    std::unique_ptr<brave_rewards::BraveRewardsService> service(new brave_rewards::BraveRewardsServiceImpl(&profile));

    //allow to run no more than 1 timers total. Otherwise TestingJoinAllRunningTasks will wait forever.
    //download publishers list  and exit
    service->AllowTimersRun(1);
    service->Init();
    service->TestingJoinAllRunningTasks();
    std::cout << std::endl << "TEST 1: init() finished" << std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    std::cout << std::endl << "TEST 2: CreateWallet() started" << std::endl;
    service->CreateWallet();
    service->TestingJoinAllRunningTasks();
    std::cout << std::endl << "TEST 2: CreateWallet() finished" << std::endl;
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //resume unlimited number of timers
    service->AllowTimersRun(0);

  }
  catch (std::exception & e)
  {
    std::cout<< "Exception caught: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unexpected exception caught: " << std::endl;
  }

  //cleanup
  curl_global_cleanup();

  std::cout << std::endl << "Exiting...." << std::endl;
  return 0;
}


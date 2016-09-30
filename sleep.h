#ifndef __PHIL_SLEEP
#define __PHIL_SLEEP
//##############################################################

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
	#include <time.h>
	srand(time(NULL));
	void sleepRandMilliseconds(int min, int max){
		Sleep(rand()%(max-min) + min);
	}
#else
	#include <random>
	#include <chrono>
	void sleepRandMilliseconds(int min, int max){
		std::mt19937_64 rnd{std::random_device{}()};
		std::uniform_int_distribution<> dist{min, max};
		std::this_thread::sleep_for(std::chrono::milliseconds{dist(rnd)});
	}
#endif

//##############################################################
#endif

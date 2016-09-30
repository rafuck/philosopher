#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <thread>
#include <mutex>
#include <atomic>

#include "resource.h"
#include "strategy.h"
#include "sleep.h"

void phil(t_id id, Resource &v_left, Resource &v_right){
	int n = rand()%10;
	ISupervisor &s = Strategy::getSupervisor();

	for(int i=0; i<n; ++i){
		s.take(v_left, v_right);

		printf("P %02d start eating: %s\n", id, s.flags());
		
		sleepRandMilliseconds(300, 800);

		s.put(v_left, v_right);

		printf("P %02d stop  eating: %s\n", id, s.flags());
	}
}

int main(int argc, char *argv[]){
	printf("Hardware concurrency: %d\n", std::thread::hardware_concurrency());
	const t_id N = 20;

	SupervisorStrategy strategy = StrategyMutex;
	if (argc > 1 && 0 == strcmp(argv[1], "atomics")){
		strategy = StrategyAtomics;
	}

	if (strategy == StrategyMutex){
		printf("Strategy: mutex\n");
	}
	else{
		printf("Strategy: atomics\n");
	}
	
	std::thread worker[N];

	Strategy::setStrategy(strategy);
	Strategy::getSupervisor().setNWorkers(N);
	
	Resource r[N];
	for(t_id i=0; i<N; ++i){
		r[i].id(i);
	}

	for(t_id i=0; i<N; ++i){
		t_id ir = i;
		t_id il = (i == 0) ? N-1 : i-1;
		
		worker[i] = std::thread(phil, i, std::ref(r[il]), std::ref(r[ir]));
	}

	for(t_id i=0; i<N; ++i){
		worker[i].join();
	}

	return EXIT_SUCCESS;
}
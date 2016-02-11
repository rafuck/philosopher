#include <cstdlib>
#include <cstdio>

#include <thread>
#include <mutex>

#include <random>
#include <chrono>

class Resource{
private:
	std::mutex m;
public:
	int id;
	void take(){
		m.lock();
	}

	void put(){
		m.unlock();
	}

	bool try_take(){
		return m.try_lock();
	}
};

class Supervisor{
private:
	std::mutex m;
	char *F;
	bool try_take(Resource &v_left, Resource &v_right){
		std::lock_guard<std::mutex> lock(m);

		if (!v_right.try_take()){
			v_left.put();
			return false;
		}

		F[v_left.id]++;
		F[v_right.id]++;

		return true;
	}

	Supervisor(){
		F = NULL;
	}

	~Supervisor(){
		if (F){
			delete[] F;
		}
	}

public:
	Supervisor(Supervisor const&) = delete;
	void operator=(Supervisor const&)  = delete;

	static Supervisor& getInstance(){
		static Supervisor instance;
		return instance;
	}

	void setNWorkers(size_t N){
		if (F){
			delete[] F;
		}

		F = new char[N+1];
		for(size_t i=0; i<N; ++i){
			F[i] = '0';
		}
		F[N] = 0;
	}
	
	void take(Resource &v_left, Resource &v_right){
		v_left.take();
		while(!try_take(v_left, v_right)){
			v_left.take();
		}
	}

	void put(Resource &v_left, Resource &v_right){
		F[v_left.id]--;
		F[v_right.id]--;

		v_right.put();
		v_left.put();
	}

	const char * const flags() const{
		return F;
	}
};

void phil(int id, Resource &v_left, Resource &v_right){
	Supervisor &s = Supervisor::getInstance();

	s.take(v_left, v_right);

	printf("P %02d start eating: %s\n", id, s.flags());
	
	std::mt19937_64 rnd{std::random_device{}()};
	std::uniform_int_distribution<> dist{500, 1000};
	std::this_thread::sleep_for(std::chrono::milliseconds{dist(rnd)});

	s.put(v_left, v_right);

	printf("P %02d stop  eating: %s\n", id, s.flags());
}

int main(){
	const size_t N = 10;
	
	std::thread worker[N];
	Supervisor::getInstance().setNWorkers(N);
	
	Resource r[N];
	for(int i=0; i<N; ++i){
		r[i].id = i;
	}

	for(int i=0; i<N; ++i){
		size_t ir = i;
		size_t il = (i == 0) ? N-1 : i-1;
		
		worker[i] = std::thread(phil, i, std::ref(r[il]), std::ref(r[ir]));
	}

	for(int i=0; i<N; ++i){
		worker[i].join();
	}

	return EXIT_SUCCESS;
}
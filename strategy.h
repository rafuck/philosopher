#ifndef __PHIL_STRATEGY_H
#define __PHIL_STRATEGY_H
//##############################################################

#include "resource.h"
typedef enum{
	StrategyMutex = 0,
	StrategyAtomics
} SupervisorStrategy;

class ISupervisor
{
public:
    virtual ~ISupervisor() { }
    virtual void take(IResource &v_left, IResource &v_right) = 0;
    virtual void put(IResource &v_left, IResource &v_right) = 0;
    virtual void setNWorkers(t_id N) = 0;
    virtual const char* flags() const = 0;
};

class SupervisorAtomics:public ISupervisor{
private:
	char *F;
	std::atomic<unsigned int> f_locked;

	bool try_take(const IResource &v_left, const IResource &v_right){
		/* ??????? */
		return false;
	}

	SupervisorAtomics(){
		F = NULL;
		f_locked = 0;
	}

	~SupervisorAtomics(){
		if (F){
			delete[] F;
		}
	}

public:
	SupervisorAtomics(SupervisorAtomics const&) = delete;
	void operator=(SupervisorAtomics const&)  = delete;

	static SupervisorAtomics& getInstance(){
		static SupervisorAtomics instance;
		return instance;
	}

	void setNWorkers(t_id N){
		if (F){
			delete[] F;
		}

		F = new char[N+1];
		for(t_id i=0; i<N; ++i){
			F[i] = '0';
		}
		F[N] = 0;
	}

	void take(IResource &v_left, IResource &v_right){
		while (!try_take(v_left, v_right)){
			continue;
		}

		F[v_left.id()]++;
		F[v_right.id()]++;
	}

	void put(IResource &v_left, IResource &v_right){
		F[v_left.id()]--;
		F[v_right.id()]--;

		/* ????? */
	}

	const char* flags() const{
		return F;
	}
};

class SupervisorMutex:public ISupervisor{
private:
	std::mutex m;
	char *F;

	bool try_take(IResource &v_left, IResource &v_right){
		IResource &v1 = std::ref((v_left.id() > v_right.id()) ? v_left : v_right);
		IResource &v2 = std::ref((v_left.id() < v_right.id()) ? v_left : v_right);
		
		if (v1.try_take()){
			if (!v2.try_take()){
				v1.put();
				return false;
			}
		}

		return true;
	}

	SupervisorMutex(){
		F = NULL;
	}

	~SupervisorMutex(){
		if (F){
			delete[] F;
		}
	}

public:
	SupervisorMutex(SupervisorMutex const&) = delete;
	void operator=(SupervisorMutex const&)  = delete;

	static SupervisorMutex& getInstance(){
		static SupervisorMutex instance;
		return instance;
	}

	void setNWorkers(t_id N){
		if (F){
			delete[] F;
		}

		F = new char[N+1];
		for(t_id i=0; i<N; ++i){
			F[i] = '0';
		}
		F[N] = 0;
	}

	void take(IResource &v_left, IResource &v_right){
		while(!try_take(v_left, v_right)){
			continue;
		}

		F[v_left.id()]++;
		F[v_right.id()]++;
	}

	void put(IResource &v_left, IResource &v_right){
		F[v_left.id()]--;
		F[v_right.id()]--;

		v_right.put();
		v_left.put();
	}

	const char* flags() const{
		return F;
	}
};

namespace Strategy{
	namespace settings{
		SupervisorStrategy _strategy = StrategyMutex;
	}
	
	ISupervisor &getSupervisor(){
		if (settings::_strategy == StrategyMutex){
			return SupervisorMutex::getInstance();
		}
		else{
			return SupervisorAtomics::getInstance();
		}
	}
	
	void setStrategy(SupervisorStrategy strategy){
		settings::_strategy = strategy;
	}
};

//##############################################################
#endif

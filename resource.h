#ifndef __PHIL_RESOURCE_H
#define __PHIL_RESOURCE_H
//##############################################################

typedef unsigned char t_id;

class IResource{
public:
	virtual ~IResource() { }
	
	virtual t_id id() const = 0;
	virtual inline unsigned int flag() const = 0;
	virtual void take() = 0;
	virtual void put() = 0;
	virtual bool try_take() = 0;
};

class Resource: public IResource{
private:
	std::mutex m;
	unsigned int _flag;
	t_id _id;
public:
	t_id id() const{
		return _id;
	}

	inline unsigned int flag() const{
		return _flag;
	}

	void id(t_id pid){
		_id = pid;
		_flag = 1 << _id;
	}

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

//##############################################################
#endif

#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
public:
	static T& getInstance();
	~Singleton();

protected:
	Singleton() {};

private:
	// Protexted functions
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

	// Data
	static T* pInstance;
};

#endif
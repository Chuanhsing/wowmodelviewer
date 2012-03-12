#ifndef SINGLETON_H
#define SINGLETON_H

#include <QMutex>
#include <QAtomicPointer>

namespace WMVEngine
{

template<typename T>
class Singleton
{
private:
	static QMutex mutex;

public:
	static T* getInstance();

private:
	// Protected functions
	Singleton();
	static void DestroySingleton();
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

	// Data
	static QAtomicPointer<T>* pInstance;
};

}	// Namespace

#endif
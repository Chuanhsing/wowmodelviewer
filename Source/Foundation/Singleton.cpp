#include "Singleton.h"

using namespace WMVEngine;

template<typename T>
QAtomicPointer<Singleton<T>> Singleton<T>::pInstance = NULL;
template<typename T>
QMutex Singleton<T>::mutex;

template<typename T>
T* Singleton<T>::getInstance()
{
	if (pInstance == NULL)
	{
		mutex.lock();
		if (!pInstance == NULL)
		{
			pInstance = new &Singleton<T>;
		}
		mutex.unlock();
	}
	return pInstance;
};

template<typename T>
void Singleton<T>::DestroySingleton()
{
	mutex.lock();
	delete pInstance;
	pInstance = NULL;
	mutex.unlock();
}
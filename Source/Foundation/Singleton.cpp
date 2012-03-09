#include "Singleton.h"

template<typename T>
T& Singleton<T>::getInstance()
{
	// Currently not thread-safe!!
	if (pInstance == NULL){
		pInstance = new Singleton<T>;
	}
	return pInstance;
}
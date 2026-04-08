#pragma once

template <typename T>
class singletonBase
{
protected:
	//ΩÃ±€≈Ê ¿ŒΩ∫≈œΩ∫ º±æ
	static T* singleton;

	singletonBase(void){};
	~singletonBase(void){};

public:
	static T* getSingleton(void);
	void releaseSingleton(void);
};

//ΩÃ±€≈Ê √ ±‚»≠
template <typename T>
T* singletonBase<T>::singleton = 0;

//ΩÃ±€≈Ê ∞™ ∞°¡Æø¿±‚
template <typename T>
T* singletonBase<T>::getSingleton(void)
{
	if (!singleton) singleton = new T;
	return singleton;
}

//ΩÃ±€≈Ê ∏ﬁ∏∏Æ «ÿ¡¶~
template <typename T>
void singletonBase<T>::releaseSingleton(void)
{
	if (singleton)
	{
		delete singleton;
		singleton = 0;
	}
}

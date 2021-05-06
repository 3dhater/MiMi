#ifndef __MI_SINGLETON_H__
#define __MI_SINGLETON_H__

template<typename type>
class miSingleton
{
	miSingleton(){}
public:

	~miSingleton(){}

	static type * s_instance;
};

#endif
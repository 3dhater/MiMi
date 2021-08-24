#ifndef _MITEXTURE_H_
#define _MITEXTURE_H_

class miTexture
{
public:
	miTexture() :m_implementationPtr(0){}
	~miTexture() {}

	void* m_implementationPtr;
};

#endif
#ifndef _MIPL_STD_H_
#define _MIPL_STD_H_

class miplStd : public miPlugin
{
public:
	miplStd();
	virtual ~miplStd();

	virtual const wchar_t* GetName();
	virtual const wchar_t* GetDescription();
	virtual const wchar_t* GetAuthor();

	virtual bool IsDebug();
	virtual miplDestroyPlugin_t GetDestroyFunction();

	virtual bool Init(miSDK* sdk);

	virtual void OnPopupCommand(unsigned int);
};

#endif
#ifndef SoftwareUpdateApp__INCLUDED
#define SoftwareUpdateApp__INCLUDED

#include <wx/wx.h>

class SoftwareUpdateApp: 
	public wxApp
{
public:
	SoftwareUpdateApp();
	~SoftwareUpdateApp();

    virtual bool OnInit();
	virtual int OnRun();
private:
};

#endif // SoftwareUpdateApp__INCLUDED


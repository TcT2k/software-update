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
private:
};

#endif // SoftwareUpdateApp__INCLUDED


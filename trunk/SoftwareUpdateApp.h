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

	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);
private:
	enum LaunchMode
	{
		LaunchStandalone,
		LaunchCheck,
		LaunchUpdate
	};

	LaunchMode m_launchMode;
	wxString m_updateFileName;
};

#endif // SoftwareUpdateApp__INCLUDED


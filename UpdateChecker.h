#ifndef UpdateChecker__INCLUDED
#define UpdateChecker__INCLUDED

#include <wx/wx.h>
#include <wx/progdlg.h>

wxDECLARE_EVENT(wxEVT_UPDATE_PROGRESS, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_UPDATE_FINISHED, wxThreadEvent);

class UpdateChecker:
	public wxEvtHandler,
	protected wxThreadHelper
{
public:
	UpdateChecker();
	~UpdateChecker();

	// Checks if update interval has been reached
	bool IsCheckRequired();

	// Ask server for update
	bool CheckForUpdate();

	// Ask the user for confirmation
	bool ConfirmUpdate();

	// Download and start the update
	void ApplyUpdate();

protected:
	wxThread::ExitCode Entry();

private:
	typedef unsigned long SoftwareVersion[4];

	// Software parameters
	wxString m_softwareTitle;
	wxString m_softwareId;
	wxString m_softwareDisplayVersion;
	SoftwareVersion m_softwareVersion;
	wxString m_softwareUpdateURL;
	wxUint32 m_softwareUpdateInterval;	// Update interval in days

	// Update information
	wxString m_updateDownloadURL;
	bool m_updateDownloadCanceled;

	// UI
	wxProgressDialog* m_progDlg;

	void Init();

	// Look for local external software info
	void LoadSoftwareInfo();

	// Load software information
	void LoadSoftwareInfo(wxInputStream& istr);

	void SaveCheckDate() const;

	void OnUpdateProgress(wxThreadEvent& event);

	void OnUpdateFinished(wxThreadEvent& event);

	static void StringToVersion(const wxString& str, SoftwareVersion& version);

	static bool IsVersionNewer(const SoftwareVersion& v1, const SoftwareVersion& v2);
};

#endif // UpdateChecker__INCLUDED

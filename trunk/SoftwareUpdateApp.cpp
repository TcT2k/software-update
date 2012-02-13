#include "SoftwareUpdateApp.h"

#include <wx/cmdline.h>
#include <wx/stdpaths.h>

#include "UpdateChecker.h"

wxIMPLEMENT_APP(SoftwareUpdateApp);

SoftwareUpdateApp::SoftwareUpdateApp():
	m_launchMode(LaunchStandalone)
{
}

SoftwareUpdateApp::~SoftwareUpdateApp()
{
}

bool SoftwareUpdateApp::OnInit()
{
	SetAppDisplayName(_("Software Update"));
	SetAppName("SoftwareUpdate");
	SetVendorName("ACE");

	if (!wxApp::OnInit())
		return false;

	return true;
}

int SoftwareUpdateApp::OnRun()
{
	int result = 0;

	UpdateChecker checker;
	if (m_launchMode == LaunchUpdate)
	{
		checker.CheckForUpdate(); // workaround for wxWidgets thread issue on Win32
		checker.SetUpdateInfoFileName(m_updateFileName);
		checker.ApplyUpdate();
		result = wxApp::OnRun();
	} else {
		bool updateAvailable = false;
		if (m_launchMode == LaunchStandalone || checker.IsCheckRequired())
			updateAvailable = checker.CheckForUpdate();

		if (updateAvailable && checker.ConfirmUpdate())
		{
			// Relaunch new process in update mode
			wxExecute(wxString::Format(wxT("\"%s\" --update=\"%s\""), wxStandardPaths::Get().GetExecutablePath(), checker.GetUpdateInfoFileName()));

			// Return errorlevel 1
			result = 1;
		}

		if (m_launchMode == LaunchStandalone && !updateAvailable)
			wxMessageBox(wxString::Format(_("No update available.\n\nThe latest version of %s is installed."), checker.GetSoftwareTitle()), 
				_("Information"), wxICON_INFORMATION | wxOK);
	}

	return result;
}

void SoftwareUpdateApp::OnInitCmdLine(wxCmdLineParser &parser)
{
	wxApp::OnInitCmdLine(parser);

    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "c", "check",			_("returns errorlevel 1 if update is available")},
        { wxCMD_LINE_OPTION, "u", "update",			_("start update from info file") },

		{ wxCMD_LINE_NONE }
    };
	parser.SetDesc(cmdLineDesc);
}

bool SoftwareUpdateApp::OnCmdLineParsed(wxCmdLineParser &parser)
{
	if (parser.Found(wxT("check")))
		m_launchMode = LaunchCheck;
	else if (parser.Found(wxT("update"), &m_updateFileName))
	{
		if (!wxFileExists(m_updateFileName))
		{
			wxLogError(wxT("Update file \"%s\" not found."), m_updateFileName);
			return false;
		}
		m_launchMode = LaunchUpdate;
	}

	return wxApp::OnCmdLineParsed(parser);
}

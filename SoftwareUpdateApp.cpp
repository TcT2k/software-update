#include "SoftwareUpdateApp.h"

#include "UpdateChecker.h"

wxIMPLEMENT_APP(SoftwareUpdateApp);

SoftwareUpdateApp::SoftwareUpdateApp()
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
	if (checker.IsCheckRequired())
	{
		if (checker.CheckForUpdate())
		{
			if (checker.ConfirmUpdate())
			{
				checker.ApplyUpdate();
				result = wxApp::OnRun();
			}
		}
	}

	return result;
}

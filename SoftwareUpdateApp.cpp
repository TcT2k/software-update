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

	UpdateChecker checker;

	if (checker.IsCheckRequired())
	{
		if (checker.CheckForUpdate())
		{
			if (checker.ConfirmUpdate())
			{
				checker.ApplyUpdate();
				return true;
			}
		}
	}

	return false;
}

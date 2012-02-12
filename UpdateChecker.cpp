#include "UpdateChecker.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>
#include <wx/url.h>
#include <wx/protocol/http.h>
#include <wx/mstream.h>
#include <wx/richmsgdlg.h>

wxDEFINE_EVENT(wxEVT_UPDATE_PROGRESS, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_UPDATE_FINISHED, wxThreadEvent);

UpdateChecker::UpdateChecker()
{
	Init();
	LoadSoftwareInfo();
}

UpdateChecker::~UpdateChecker()
{
}

void UpdateChecker::Init()
{
	m_softwareUpdateInterval = 7;
	for (int i = 0; i < 4; i++)
		m_softwareVersion[i] = 0;

	// Bind events
	Bind(wxEVT_UPDATE_PROGRESS, &UpdateChecker::OnUpdateProgress, this);
	Bind(wxEVT_UPDATE_FINISHED, &UpdateChecker::OnUpdateFinished, this);
}

void UpdateChecker::LoadSoftwareInfo()
{
	wxFileName confFN(wxStandardPaths::Get().GetExecutablePath());
	confFN.SetFullName(wxT("SoftwareUpdate.conf"));
	if (confFN.FileExists())
	{
		wxFileInputStream istr(confFN.GetFullPath());
		LoadSoftwareInfo(istr);
	}
}

void UpdateChecker::LoadSoftwareInfo(wxInputStream& istr)
{
	wxFileConfig cfg(istr, wxConvUTF8);

	m_softwareTitle = cfg.Read(wxT("Software/Title"));
	m_softwareId = cfg.Read(wxT("Software/Id"));
	m_softwareDisplayVersion = cfg.Read(wxT("Software/DisplayVersion"));
	StringToVersion(cfg.Read(wxT("Software/Version")), m_softwareVersion);
	m_softwareUpdateURL = cfg.Read(wxT("Software/UpdateURL"));
	m_softwareUpdateInterval = cfg.Read(wxT("Software/UpdateInterval"), 7);
}

bool UpdateChecker::IsCheckRequired()
{
	// Always return false if configuration is incomplete
	if (m_softwareId.empty())
		return false;

	wxConfigBase* conf = wxConfigBase::Get();
	wxDateTime lastCheck(conf->ReadDouble(wxString::Format("%s/LastCheck", m_softwareId), 0.0));
	wxDateTime checkDate(wxDateTime::Now());
	checkDate.Subtract(wxDateSpan::Days(m_softwareUpdateInterval));
	return lastCheck.IsEarlierThan(checkDate);
}

bool UpdateChecker::CheckForUpdate()
{
	bool result = false;

	// Check update URL result
	wxURL url(m_softwareUpdateURL);
	wxInputStream* istr = url.GetInputStream();
	if (istr)
	{
		// Get file information to local stream
		wxFileName tempFileName;
		wxFile tempFile;
		tempFileName.AssignTempFileName("SU", &tempFile);
		wxFileOutputStream tempOutStream(tempFile);
		tempOutStream.Write(*istr);
		tempOutStream.Close();
		delete istr;

		// Read configuration
		wxFileInputStream tempInStream(tempFile);
		tempInStream.SeekI(0);
		wxFileConfig cfg(tempInStream);
		tempFile.Close();

		SoftwareVersion updateVersion; 
		StringToVersion(cfg.Read(wxT("Software/Version")), updateVersion);
		result = IsVersionNewer(m_softwareVersion, updateVersion);
		if (result)
			m_updateDownloadURL = cfg.Read(wxT("Software/DownloadURL"));
		else
			SaveCheckDate();

		wxRemoveFile(tempFileName.GetFullPath());
	}

	return result;
}

bool UpdateChecker::ConfirmUpdate()
{
	bool result = false;

	wxRichMessageDialog dlg(NULL, 
		wxString::Format(_("%s update available.\n\nDo you wish to install this update now?"), m_softwareTitle), _("Software Update"),
		wxOK| wxCANCEL | wxICON_INFORMATION);
	dlg.SetOKCancelLabels(_("Install"), _("Remind me later"));
	result = dlg.ShowModal() == wxID_OK;

	wxConfigBase* conf = wxConfigBase::Get();
	if (result)
		SaveCheckDate();

	return result;
}

void UpdateChecker::ApplyUpdate()
{
	m_updateDownloadCanceled = false;
	m_progDlg = new wxProgressDialog(_("Software Update"), _("Downloading Update..."));

	// Start download thread
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}

	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not run the worker thread!");
		return;
	}
}

void UpdateChecker::OnUpdateProgress(wxThreadEvent& event)
{
	m_updateDownloadCanceled = m_progDlg->Update(event.GetInt());
}

void UpdateChecker::OnUpdateFinished(wxThreadEvent& event)
{
	delete m_progDlg;
	m_progDlg = NULL;
}

wxThread::ExitCode UpdateChecker::Entry()
{
	wxLogDebug("Thread entry...");

	wxFileName tempFileName;

	// Download update
	wxURL url(m_updateDownloadURL);
	wxInputStream* istr = url.GetInputStream();
	if (istr)
	{
		wxFile tempFile;
		tempFileName.AssignTempFileName("SU", &tempFile);
		wxFileOutputStream tempOutStream(tempFile);

		// Copy data with progress
		int prevProgress = -1;
		static const int DEF_READ_SIZE = 64*1024;
		char* buf = new char[DEF_READ_SIZE];
		wxFileOffset downloadSize = istr->GetSize();
		wxFileOffset copiedSize = 0;
		while (copiedSize < downloadSize)
		{
			size_t readSize;
			if (downloadSize - copiedSize < DEF_READ_SIZE)
				readSize = downloadSize - copiedSize;
			else
				readSize = DEF_READ_SIZE;

			istr->Read(buf, readSize);
			tempOutStream.Write(buf, readSize);

			copiedSize += readSize;

			int newProgress = (copiedSize * 100) / downloadSize;
			if (newProgress > prevProgress)
			{
				wxThreadEvent* evt = new wxThreadEvent(wxEVT_UPDATE_PROGRESS);
				evt->SetInt(newProgress);
				wxQueueEvent(this, evt);
				prevProgress = newProgress;
			}
		}

		delete [] buf;

		tempOutStream.Close();
		delete istr;
	}

	// Verfiy update

	// Start update


	wxQueueEvent(this, new wxThreadEvent(wxEVT_UPDATE_FINISHED));
	return (wxThread::ExitCode)0; 
}


void UpdateChecker::StringToVersion(const wxString& str, SoftwareVersion& version)
{
	// Initialize array
	for (int i = 0; i < 4; i++)
		version[i] = 0;

	// Extract version components
	int componentIndex = 0;
	wxString::const_iterator componentStart = str.begin();
	for (wxString::const_iterator ch = str.begin(); ch != str.end() && componentIndex < 4; ch++)
	{
		if (*ch == '.' || ch == str.end() - 1)
		{
			size_t len = ch - componentStart;
			if (*ch != '.')
				len++;
			wxString comp = str.substr(componentStart - str.begin(), len);
			comp.ToULong(&version[componentIndex]);
			componentStart = ch;
			componentStart++;
			componentIndex++;
		}
	}
}

bool UpdateChecker::IsVersionNewer(const SoftwareVersion& v1, const SoftwareVersion& v2)
{
	for (int i = 0; i < 4; i++)
		if (v2[i] > v1[i])
			return true;

	return false;
}

void UpdateChecker::SaveCheckDate() const
{
	wxConfigBase* conf = wxConfigBase::Get();
//	conf->Write(wxString::Format("%s/LastCheck", m_softwareId), wxDateTime::Now().GetJulianDayNumber());
}

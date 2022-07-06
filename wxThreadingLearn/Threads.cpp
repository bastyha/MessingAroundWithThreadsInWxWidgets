#include "Threads.h"
wxIMPLEMENT_APP(MyApp);

MyApp::MyApp()
{
	m_shuttingDown = false;
}

static MyFrame* frame = NULL;

bool MyApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	frame = new MyFrame("wxThreads learning app");
	frame->Show();
	return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(THREAD_ABOUT, MyFrame::OnAbout)
EVT_MENU(THREAD_QUIT, MyFrame::OnQuit)

EVT_BUTTON(THREAD_DoSg, MyFrame::OnDoSgOnADifferentThread)
//EVT_BUTTON(THREAD_DATEADDING, MyFrame::OnStartDateAdding) //old datePicker
EVT_BUTTON(ON_TIME_CHANGE, MyFrame::OnAskTime)    //TimePicker
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title)
	:wxFrame(NULL, wxID_ANY, title)
{
	wxMenuBar* menuBar = new wxMenuBar;

	wxMenu* menuFile = new wxMenu;

	menuFile->Append(THREAD_ABOUT, "&About");
	menuFile->AppendSeparator();
	menuFile->Append(THREAD_QUIT, "&Exit");
	menuBar->Append(menuFile, "&File");

	SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
	CreateStatusBar(1);
#endif

	wxBoxSizer* btns = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags flags;
	flags.Border(wxALL, 10);
	flags.Align(wxALIGN_CENTER_HORIZONTAL);

	//old datePicker
	/*
	btns->Add(new wxButton(this, THREAD_DATEADDING,
		"&Add a date"), flags);
	*/

	btns->Add(new wxButton(this, ON_TIME_CHANGE,
		"&Add time"), flags);
	btns->Add(new wxButton(this, THREAD_DoSg,
		"&play music on another thread"), flags);

	btns->SetMinSize(wxSize(300, 100));
	SetSizerAndFit(btns);

	Center();
	msg.Printf("Helo there!");
	SetStatusText(msg, 0);
}

MyFrame::~MyFrame()
{
	{
		wxCriticalSectionLocker locker(wxGetApp().m_critsect);

		const wxArrayThread& threads = wxGetApp().m_threads;
		size_t count = threads.GetCount();

		if (!count)
			return;

		wxGetApp().m_shuttingDown = true;
	}

	wxGetApp().m_semAllDone.Wait();
}

//old datePicker
/*static FrameToAskForDate* asking = NULL;  

void MyFrame::OnStartDateAdding(wxCommandEvent& WXUNUSED(event))
{
	asking = new FrameToAskForDate(this,
		"Entering dates");
	frame->Show(false);
	asking->Show();
}
*/
//-------------------------------------------
//--------DialogToAskForDate-----------------//old datePicker
//-------------------------------------------

/*
wxBEGIN_EVENT_TABLE(FrameToAskForDate, wxFrame)
EVT_BUTTON(DATEASK_OK, FrameToAskForDate::OnOk)
EVT_BUTTON(DATEASK_ANOTHER, FrameToAskForDate::OnAnother)
EVT_BUTTON(DATEASK_CANCEL, FrameToAskForDate::OnCancel)

EVT_CLOSE(FrameToAskForDate::OnExit)
wxEND_EVENT_TABLE()



FrameToAskForDate::FrameToAskForDate(wxWindow* parent,
	const wxString& title)
	:wxFrame(parent, wxID_ANY, title)
{

	wxIntegerValidator<unsigned int> m_year(&nYear);
	m_year.SetMin(1);
	m_year.SetMax(2040);

	wxIntegerValidator<unsigned int> m_month(&nMonth);
	m_month.SetMin(1);
	m_month.SetMax(12);

	wxIntegerValidator<unsigned int> m_day(&nDay);
	m_day.SetMin(1);
	m_day.SetMax(31);

	txtctrl1 = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0L, m_year);
	txtctrl2 = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0L, m_month);
	txtctrl3 = new wxTextCtrl(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0L, m_day);

	wxSizerFlags flags;
	flags.Border(wxALL, 10);
	flags.Align(wxALIGN_CENTER_HORIZONTAL);

	wxFlexGridSizer* const sizerMain = new wxFlexGridSizer(3);
	sizerMain->Add(new wxStaticText(this, wxID_ANY,
		"Enter &year:"), flags);
	sizerMain->Add(new wxStaticText(this, wxID_ANY,
		"Enter &month:"), flags);
	sizerMain->Add(new wxStaticText(this, wxID_ANY,
		"Enter &day:"), flags);

	wxSizerFlags flags1;
	flags1.Border(wxALL, 10);
	

	wxSizer* btns1 = new wxBoxSizer(wxHORIZONTAL);
	btns1->Add(new wxButton(this, DATEASK_OK, "&Ok"), flags1);
	btns1->Add(new wxButton(this, DATEASK_ANOTHER, "&Another date"), flags1);
	btns1->Add(new wxButton(this, DATEASK_CANCEL, "&Cancel"), flags1);

	sizerMain->Add(txtctrl1, flags);
	sizerMain->Add(txtctrl2, flags);
	sizerMain->Add(txtctrl3, flags);
	wxBoxSizer* dateout = new wxBoxSizer(wxVERTICAL);
	dateout->Add(sizerMain, flags);
	dateout->Add(btns1, flags);

	SetSizerAndFit(dateout);

	Center();

	AnotherCalled = 0;
}

bool FrameToAskForDate::IsLeapYear(unsigned year)
{
	if ((!(year % 4) &&
		year % 100) ||
		!(year % 400))
		return true;
	else
		return false;
}

bool FrameToAskForDate::MonthTesting(unsigned year ,unsigned month, unsigned day)
{
	if (txtctrl1->IsEmpty() || txtctrl2->IsEmpty() || txtctrl3->IsEmpty())
	{
		wxMessageBox("Enter something everywhere", "Error", wxOK | wxICON_ERROR);
		return false;
	}
	else if (year < 2010)
	{
		wxMessageBox("Don't enter a year under 2010. Look into the future", "Error", wxOK | wxICON_ERROR);
		return false;
	}
	else if (IsLeapYear(year) &&
		((month == 2) &&
			(day > 29)))
	{
		wxMessageBox("February only has 29 days",
			"Error", wxOK | wxICON_ERROR);
		return false;
	}
	else if (!(IsLeapYear(year)) &&
		((month == 2) &&
			(day > 28)))
	{
		wxMessageBox("February only has 28 days",
			"Error", wxOK | wxICON_ERROR);
		return false;
	}
	else if (day > 30 && (month == 4 || month == 6 || month == 9 || month == 11))
	{
		wxMessageBox("This month doesn't have 31 days",
			"Error", wxOK | wxICON_ERROR);
		return false;
	}
	else
		return true;
}

void FrameToAskForDate::AddDateToFile(unsigned year, unsigned month, unsigned day)
{
	std::vector<std::string> StrVec;

	StrVec.push_back(std::to_string(month));
	StrVec.push_back(std::to_string(day));

	for (unsigned i = 0; i < StrVec.size(); i++)
	{
		if (!(StrVec[i].length() - 1))
			StrVec[i].insert(StrVec[i].begin(), '0');
	}

	wxString toFile;
	toFile.Printf("%u-%s-%s\n", year, StrVec[0], StrVec[1]);

	std::ofstream myfile;
	myfile.open("dates.txt", std::ios::app);
	myfile << toFile;
	myfile.close();
}

void FrameToAskForDate::OnExit(wxCloseEvent& event)
{
	asking->Show(false);
	frame->Show();
	wxString msgToAdd = "Date adding was exited by the user";
	frame->SetStatusText(msgToAdd, 0);
}

void FrameToAskForDate::OnCancel(wxCommandEvent& event)
{
	asking->Show(false);
	frame->Show();
	wxString msgToAdd = "Date adding was cancelled by the user";
	frame->SetStatusText(msgToAdd, 0);
}

void FrameToAskForDate::OnOk(wxCommandEvent& event)
{
	nYear = wxAtoi(txtctrl1->GetLineText(0));
	nMonth = wxAtoi(txtctrl2->GetLineText(0));
	nDay = wxAtoi(txtctrl3->GetLineText(0));

	if (MonthTesting(asking->nYear, asking->nMonth, asking->nDay))
	{
		AddDateToFile(asking->nYear, asking->nMonth, asking->nDay);
		if (AnotherCalled == 0)
		{
			asking->Show(false);
			frame->Show();

			wxString msgToAdd = "You added a date successfully";
			frame->SetStatusText(msgToAdd, 0);
			//I will later probably add the inputs to a file

		}
		else if (AnotherCalled != 0)
		{
			asking->Show(false);
			frame->Show();

			wxString msgToAdd = "You added dates successfully";
			frame->SetStatusText(msgToAdd, 0);
		}
	}
}

void FrameToAskForDate::OnAnother(wxCommandEvent& event)
{
	nYear = wxAtoi(txtctrl1->GetLineText(0));
	nMonth = wxAtoi(txtctrl2->GetLineText(0));
	nDay = wxAtoi(txtctrl3->GetLineText(0));

	if (MonthTesting(asking->nYear, asking->nMonth, asking->nDay))
	{
		AnotherCalled++;
		AddDateToFile(asking->nYear, asking->nMonth, asking->nDay);
		txtctrl1->Clear();
		txtctrl2->Clear();
		txtctrl3->Clear();

	}
	//same as OnOk() w/out the Show() things
	// and maybe reset Textctrl-s
}

*/

//-----------------------------------------------
//--------------------Casual---------------------
//-----------------------------------------------

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox("I'm testing threads and input (a bit)",
		"&About", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

//----------------------------------------------
//------------------Thread----------------------
//----------------------------------------------

void MyFrame::OnDoSgOnADifferentThread(wxCommandEvent& WXUNUSED(event)) //wxThread::ExitCode MyThread::Entry() <-- this runs
{
	MyThread* thread = CreateThread();

	if (thread->Run() != wxTHREAD_NO_ERROR)
		wxMessageBox("Can't start thread!", "Error", wxOK | wxICON_ERROR);

	SetStatusText("Thread started", 0);
}

MyThread::MyThread()
	:wxThread()
{
	m_count = 0;
}

MyThread::~MyThread()
{
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);

	wxArrayThread& threads = wxGetApp().m_threads;
	threads.Remove(this);

	if (threads.IsEmpty())
	{
		if (wxGetApp().m_shuttingDown)
		{
			wxGetApp().m_shuttingDown = false;

			wxGetApp().m_semAllDone.Post();
		}
	}
}

wxThread::ExitCode MyThread::Entry()
{
	soundFile = WAV_FILE;
	m_sound = NULL;

	for (m_count = 0; m_count < 3; m_count++)
	{
		{
			wxCriticalSectionLocker locker(wxGetApp().m_critsect);
			if (wxGetApp().m_shuttingDown)
				return NULL;
		}
		if (TestDestroy())
			break;

		wxThread::Sleep(1000);
	}
	PlayBell();
	wxMessageBox("Hell yes", "Decider", wxOK | wxICON_INFORMATION);

	return NULL;
}

MyThread* MyFrame::CreateThread()
{
	MyThread* thread = new MyThread;

	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		wxMessageBox("Can't create thread!", "Error", wxOK | wxICON_ERROR);
	}
	wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	wxGetApp().m_threads.Add(thread);

	return thread;
}


//----------Play sound in thread-------------

void MyThread::PlayBell()
{
	if (!m_sound)
		m_sound = CreateBellSound();

	if (m_sound)
		m_sound->Play(wxSOUND_ASYNC);
}

wxSound* MyThread::CreateBellSound()
{
	wxSound* sound = new wxSound;
	sound->Create(soundFile);
	return sound;
}

//------------------------------------------------------
//--------------------TimePicker------------------------
//------------------------------------------------------

void MyFrame::OnAskTime(wxCommandEvent& event)
{
	TimeAsker timeAskerdlg(this);
	timeAskerdlg.ShowModal();
}
wxBEGIN_EVENT_TABLE(TimeAsker, wxDialog)
EVT_TIME_CHANGED(wxID_ANY, TimeAsker::OnTimeChange)
EVT_DATE_CHANGED(wxID_ANY, TimeAsker::OnDateChange)

EVT_BUTTON(DATEASK_OK, TimeAsker::OnOk)
EVT_BUTTON(DATEASK_ANOTHER, TimeAsker::OnAnother)
EVT_BUTTON(DATEASK_CANCEL, TimeAsker::OnCancel)

EVT_CLOSE(TimeAsker::OnClose)
wxEND_EVENT_TABLE()



void TimeAsker::OnTimeChange(wxDateEvent& event)
{
	m_timeText->SetLabel(event.GetDate().FormatISOTime());
}

void TimeAsker::OnDateChange(wxDateEvent& event)
{
	m_dateText->SetLabel(event.GetDate().FormatISODate());
}


TimeAsker::TimeAsker(wxWindow* parent)
	:wxDialog(parent, wxID_ANY, wxString("Choose time"))
{
	IsItTheFirst = true;

	m_timePicker = new wxTimePickerCtrl(this, wxID_ANY);
	m_datePicker = new wxDatePickerCtrl(this, wxID_ANY);

	m_datePicker->SetRange(wxDateTime(1, wxDateTime::Jan, 1900), wxDefaultDateTime);

	m_timeText = new wxStaticText(this, wxID_ANY,
		m_timePicker->GetValue().FormatISOTime());
	m_dateText = new wxStaticText(this, wxID_ANY,
		m_datePicker->GetValue().FormatISODate());

	const wxSizerFlags flags = wxSizerFlags().Centre().Border();
	wxFlexGridSizer* const sizerMain = new wxFlexGridSizer(2);
	sizerMain->Add(new wxStaticText(this, wxID_ANY, "Enter &date:"), flags);
	sizerMain->Add(new wxStaticText(this, wxID_ANY, "Enter &time:"), flags);

	sizerMain->Add(m_timePicker, flags);
	sizerMain->Add(m_datePicker, flags);

	wxSizer* const sizerTexts = new wxBoxSizer(wxHORIZONTAL);
	sizerTexts->Add(new wxStaticText(this, wxID_ANY, "Entered date and time:"),flags);
	sizerTexts->Add(m_timeText, flags);
	sizerTexts->Add(m_dateText, flags);

	wxSizer* const sizerBTNs = new wxBoxSizer(wxHORIZONTAL);
	sizerBTNs->Add(new wxButton(this, DATEASK_OK, "&Ok"), flags);
	sizerBTNs->Add(new wxButton(this, DATEASK_ANOTHER, "&Another"), flags);
	sizerBTNs->Add(new wxButton(this, DATEASK_CANCEL, "&Cancel"), flags);


	wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
	sizerTop->Add(sizerMain, flags);
	sizerTop->Add(sizerTexts, flags);
	sizerTop->Add(sizerBTNs, flags);

	SetSizerAndFit(sizerTop);
}

void TimeAsker::OnOk(wxCommandEvent& event)
{
	std::ofstream DateTimeToFile;
	DateTimeToFile.open("date.txt", std::ios::app);
	DateTimeToFile << m_datePicker->GetValue().FormatISODate() <<
		'/' << m_timePicker->GetValue().FormatISOTime() << std::endl;
	DateTimeToFile.close();

	if (IsItTheFirst)
		frame->SetStatusText("Addded one date", 0);
	else
		frame->SetStatusText("Added multiple dates", 0);
			
	
	EndModal(0);
}

void TimeAsker::OnAnother(wxCommandEvent& event)
{
	IsItTheFirst = false;

	std::ofstream DateTimeToFile;
	DateTimeToFile.open("date.txt", std::ios::app);
	DateTimeToFile << m_datePicker->GetValue().FormatISODate() <<
		'/' << m_timePicker->GetValue().FormatISOTime() << std::endl;
	DateTimeToFile.close();

	m_timePicker->SetValue(wxDateTime::Now());
	m_datePicker->SetValue(wxDateTime::Now());
	
}

void TimeAsker::OnCancel(wxCommandEvent& event)
{
	frame->SetStatusText("Date adding was cancelled", 0);
	EndModal(1);
}

void TimeAsker::OnClose(wxCloseEvent& event)
{
	frame->SetStatusText("Date adding was closed by the user", 0);
	EndModal(2);
}
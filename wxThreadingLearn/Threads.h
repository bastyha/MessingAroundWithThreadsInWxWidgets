#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_THREADS
#error "This program requires thread support!"
#endif // !!wxUSE_THREADS

#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/numdlg.h>
#include <wx/valnum.h>
#include <wx/string.h>
#include <exception>
#include <fstream>
#include <wx/sound.h>
#define WAV_FILE "Bell.wav"
#include <string>
#include <vector>
#include <wx/timectrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>

class MyThread;
WX_DEFINE_ARRAY_PTR(wxThread*, wxArrayThread);

class MyApp : public wxApp
{
public:
	MyApp();
	virtual ~MyApp() {}

	virtual bool OnInit() wxOVERRIDE;

	//CritSect to protects threads
	wxCriticalSection m_critsect;

	//all the threads currently alive
	wxArrayThread m_threads;

	//When shutting down, used to terminate all the threads
	wxSemaphore m_semAllDone;

	//boolean indicating, that we are shutting
	// down and threads should close
	bool m_shuttingDown;
};

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title);
	virtual ~MyFrame();

	wxString msg;

private:
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	//void OnStartDateAdding(wxCommandEvent& event); //old datePicker

	//wxThread::ExitCode MyThread::Entry() <-- runs
	void OnDoSgOnADifferentThread(wxCommandEvent& event); 

	//creates new thread but doesn't run it
	MyThread* CreateThread();

	void OnAskTime(wxCommandEvent& event); //TimePicker

	wxDECLARE_EVENT_TABLE();
};

enum
{
	THREAD_QUIT = wxID_EXIT,
	THREAD_ABOUT = wxID_ABOUT,

	//THREAD_DATEADDING, //old datePicker

	THREAD_DoSg, 

	DATEASK_OK,
	DATEASK_CANCEL,
	DATEASK_ANOTHER,

	ON_TIME_CHANGE //TimePicker
};

class MyThread : public wxThread
{
public:
	MyThread();
	virtual ~MyThread();

	virtual void* Entry() wxOVERRIDE;

	unsigned m_count;

private:
	//<sounds>
	wxSound* CreateBellSound();
	void PlayBell();
	wxString soundFile;
	wxSound* m_sound;
	//</sounds>
};

//old datePicker
/*
class FrameToAskForDate : public wxFrame
{
public:
	FrameToAskForDate(wxWindow* parent, const wxString& title);

	unsigned nYear;
	unsigned nMonth;
	unsigned nDay;

	unsigned AnotherCalled;

	wxTextCtrl* txtctrl1;
	wxTextCtrl* txtctrl2;
	wxTextCtrl* txtctrl3;

	wxStaticText* m_dateText;


	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnAnother(wxCommandEvent& event);
	void OnExit(wxCloseEvent& event);

	//testing if the entered year is leapyear
	bool IsLeapYear(unsigned year); 

	//testing if a num was entered everywhere
	// and if the date exists
	bool MonthTesting(unsigned year, unsigned month, unsigned day); 

	//adding the entered dates to a file
	void AddDateToFile(unsigned year, unsigned month, unsigned day);

	wxDECLARE_EVENT_TABLE();
};
*/

class TimeAsker : public wxDialog //TimePicker
{
public:
	TimeAsker(wxWindow* parent);

	wxDateTime GetTime() const { return m_timePicker->GetValue(); }
	wxDateTime GetDate() const { return m_datePicker->GetValue(); }

	wxTimePickerCtrlBase* m_timePicker;
	wxDatePickerCtrlBase* m_datePicker;

private:
	void OnTimeChange(wxDateEvent& event);
	void OnDateChange(wxDateEvent& event);
	wxStaticText* m_timeText;
	wxStaticText* m_dateText;

	bool IsItTheFirst;

	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnAnother(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	wxDECLARE_EVENT_TABLE();
};
#ifndef BETEX_H
#define BETEX_H

#include <be/app/Application.h>
#include <be/app/Message.h>
#include <be/interface/Alert.h>
#include <be/storage/Path.h>
#include "MainWindow.h"

class BeTeX : public BApplication {
public:
	BeTeX();
	virtual ~BeTeX();

	virtual void AboutRequested();
	virtual void RefsReceived(BMessage* message);
	virtual bool QuitRequested();

private:
	MainWindow* m_mainWindow;
	BAlert* m_aboutBetex;
	BPath m_prefsPath;
};

#endif

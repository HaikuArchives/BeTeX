/**	From SimplyVorbis code
	Get the original code here: http://www.bebits.com/app/4227
	Code under BSD/MIT licence, code by Darkwyrm
 */
#include "Preferences.h"

BLocker prefsLock;
BMessage preferences;

status_t SavePreferences(const char *path)
{
	if(!path)
		return B_ERROR;
	
	prefsLock.Lock();
	
	BFile file(path,B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	
	status_t status=file.InitCheck();
	if(status!=B_OK)
	{
		prefsLock.Unlock();
		return status;
	}
	
	status=preferences.Flatten(&file);
		
	prefsLock.Unlock();
	return status;
}

status_t LoadPreferences(const char *path)
{
	if(!path)
		return B_ERROR;
	
	prefsLock.Lock();
	
	BFile file(path,B_READ_ONLY);

	BMessage msg;
	
	status_t status=file.InitCheck();
	if(status!=B_OK)
	{
		prefsLock.Unlock();
		return status;
	}
	
	status=msg.Unflatten(&file);
	if(status==B_OK)
		preferences=msg;
		
	prefsLock.Unlock();
	return status;
	
}

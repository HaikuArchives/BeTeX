/**	From SimplyVorbis code
	Get the original code here: http://www.bebits.com/app/4227
	Code under BSD/MIT licence, code by Darkwyrm
 */
#include "Preferences.h"

BLocker prefsLock;

status_t
SavePreferences(BMessage* preferences_archive, const char* path)
{
	if (!path)
		return B_ERROR;

	prefsLock.Lock();

	BFile file(path, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);

	status_t status = file.InitCheck();
	if (status != B_OK) {
		prefsLock.Unlock();
		return status;
	}

	status = preferences_archive->Flatten(&file);

	prefsLock.Unlock();
	return status;
}

status_t
LoadPreferences(const char* path, BMessage* preferences_archive)
{
	if (!path)
		return B_ERROR;

	prefsLock.Lock();

	BFile file(path, B_READ_ONLY);

	BMessage msg;

	status_t status = file.InitCheck();
	if (status != B_OK) {
		prefsLock.Unlock();
		return status;
	}

	status = preferences_archive->Unflatten(&file);

	prefsLock.Unlock();
	return status;
}

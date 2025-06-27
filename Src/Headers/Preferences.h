/**	From SimplyVorbis code
	Get the original code here: http://www.bebits.com/app/4227
	Code under BSD/MIT licence, code by Darkwyrm
 */
#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <Locker.h>
#include <be/app/Message.h>
#include <be/storage/File.h>

extern BLocker prefsLock;

status_t SavePreferences(BMessage* preferences_archive, const char* path);
status_t LoadPreferences(const char* path, BMessage* preferences_archive);

#endif

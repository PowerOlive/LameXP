///////////////////////////////////////////////////////////////////////////////
// LameXP - Audio Encoder Front-End
// Copyright (C) 2004-2012 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// http://www.gnu.org/licenses/gpl-2.0.txt
///////////////////////////////////////////////////////////////////////////////

#include "Tool_Abstract.h"

#include "Global.h"

#include <QProcess>
#include <QMutex>
#include <QMutexLocker>
#include <QLibrary>
#include <QProcessEnvironment>
#include <QDir>

/*
 * Win32 API definitions
 */
typedef HANDLE (WINAPI *CreateJobObjectFun)(__in_opt LPSECURITY_ATTRIBUTES lpJobAttributes, __in_opt LPCSTR lpName);
typedef BOOL (WINAPI *SetInformationJobObjectFun)(__in HANDLE hJob, __in JOBOBJECTINFOCLASS JobObjectInformationClass, __in_bcount(cbJobObjectInformationLength) LPVOID lpJobObjectInformation, __in DWORD cbJobObjectInformationLength);
typedef BOOL (WINAPI *AssignProcessToJobObjectFun)(__in HANDLE hJob, __in HANDLE hProcess);

/*
 * Static vars
 */
quint64 AbstractTool::m_lastLaunchTime = 0ui64;
QMutex *AbstractTool::m_mutex_startProcess = NULL;
HANDLE AbstractTool::m_handle_jobObject = NULL;
unsigned int AbstractTool::m_jobObjRefCount = 0U;

/*
 * Const
 */
static const DWORD START_DELAY = 333; //in milliseconds
static const quint64 START_DELAY_NANO = START_DELAY * 1000 * 10; //in 100-nanosecond intervals

/*
 * Constructor
 */
AbstractTool::AbstractTool(void)
{
	static CreateJobObjectFun CreateJobObjectPtr = NULL;
	static SetInformationJobObjectFun SetInformationJobObjectPtr = NULL;

	if(!m_mutex_startProcess)
	{
		m_mutex_startProcess = new QMutex();
	}

	QMutexLocker lock(m_mutex_startProcess);

	if(m_jobObjRefCount < 1U)
	{
		DWORD osVersionNo = lamexp_get_os_version();
		if(((HIWORD(osVersionNo) == 5) && (LOWORD(osVersionNo) >= 1)) || (HIWORD(osVersionNo) > 5))
		{
			if((!CreateJobObjectPtr) || (!SetInformationJobObjectPtr))
			{
				QLibrary Kernel32Lib("kernel32.dll");
				CreateJobObjectPtr = (CreateJobObjectFun) Kernel32Lib.resolve("CreateJobObjectA");
				SetInformationJobObjectPtr = (SetInformationJobObjectFun) Kernel32Lib.resolve("SetInformationJobObject");
			}
		}
		if(CreateJobObjectPtr && SetInformationJobObjectPtr)
		{
			HANDLE jobObject = CreateJobObjectPtr(NULL, NULL);
			if((jobObject != NULL) && (jobObject != INVALID_HANDLE_VALUE))
			{
				JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobExtendedLimitInfo;
				memset(&jobExtendedLimitInfo, 0, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
				memset(&jobExtendedLimitInfo.BasicLimitInformation, 0, sizeof(JOBOBJECT_BASIC_LIMIT_INFORMATION));
				jobExtendedLimitInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
				if(SetInformationJobObjectPtr(jobObject, JobObjectExtendedLimitInformation, &jobExtendedLimitInfo, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION)))
				{
					m_handle_jobObject = jobObject;
					m_jobObjRefCount = 1U;
				}
				else
				{
					qWarning("Failed to set job object information!");
					CloseHandle(jobObject);
				}
			}
			else
			{
				qWarning("Failed to create the job object!");
			}
		}
	}
	else
	{
		m_jobObjRefCount++;
	}

	m_firstLaunch = true;
}

/*
 * Destructor
 */
AbstractTool::~AbstractTool(void)
{
	QMutexLocker lock(m_mutex_startProcess);

	if(m_jobObjRefCount >= 1U)
	{
		m_jobObjRefCount--;
		if((m_jobObjRefCount < 1U) && m_handle_jobObject)
		{
			CloseHandle(m_handle_jobObject);
			m_handle_jobObject = NULL;
		}
	}
}

/*
 * Initialize and launch process object
 */
bool AbstractTool::startProcess(QProcess &process, const QString &program, const QStringList &args)
{
	static AssignProcessToJobObjectFun AssignProcessToJobObjectPtr = NULL;
	
	QMutexLocker lock(m_mutex_startProcess);

	if(currentTime() <= m_lastLaunchTime)
	{
		Sleep(START_DELAY);
	}

	emit messageLogged(commandline2string(program, args) + "\n");

	QProcessEnvironment env = process.processEnvironment();
	if(env.isEmpty()) env = QProcessEnvironment::systemEnvironment();
	env.insert("TEMP", QDir::toNativeSeparators(lamexp_temp_folder2()));
	env.insert("TMP", QDir::toNativeSeparators(lamexp_temp_folder2()));
	process.setProcessEnvironment(env);
	
	if(!AssignProcessToJobObjectPtr)
	{
		QLibrary Kernel32Lib("kernel32.dll");
		AssignProcessToJobObjectPtr = (AssignProcessToJobObjectFun) Kernel32Lib.resolve("AssignProcessToJobObject");
	}
	
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.setReadChannel(QProcess::StandardOutput);
	process.start(program, args);
	
	if(process.waitForStarted())
	{
		if(AssignProcessToJobObjectPtr && m_handle_jobObject)
		{
			if(!AssignProcessToJobObjectPtr(m_handle_jobObject, process.pid()->hProcess))
			{
				qWarning("Failed to assign process to job object!");
			}
		}
		if(!SetPriorityClass(process.pid()->hProcess, BELOW_NORMAL_PRIORITY_CLASS))
		{
			SetPriorityClass(process.pid()->hProcess, IDLE_PRIORITY_CLASS);
		}
		
		lock.unlock();
		
		if(m_firstLaunch)
		{
			emit statusUpdated(0);
			m_firstLaunch = false;
		}
		
		m_lastLaunchTime = currentTime() + START_DELAY_NANO;
		return true;
	}

	emit messageLogged("Process creation has failed :-(");
	QString errorMsg= process.errorString().trimmed();
	if(!errorMsg.isEmpty()) emit messageLogged(errorMsg);

	process.kill();
	process.waitForFinished(-1);

	m_lastLaunchTime = currentTime() + START_DELAY_NANO;
	return false;
}

/*
 * Convert program arguments to single string
 */
QString AbstractTool::commandline2string(const QString &program, const QStringList &arguments)
{
	QString commandline = (program.contains(' ') ? QString("\"%1\"").arg(program) : program);
	
	for(int i = 0; i < arguments.count(); i++)
	{
		commandline += (arguments.at(i).contains(' ') ? QString(" \"%1\"").arg(arguments.at(i)) : QString(" %1").arg(arguments.at(i)));
	}

	return commandline;
}

/*
 * Convert long path to short path
 */
QString AbstractTool::pathToShort(const QString &longPath)
{
	QString shortPath;
	DWORD buffSize = GetShortPathNameW(reinterpret_cast<const wchar_t*>(longPath.utf16()), NULL, NULL);
	
	if(buffSize > 0)
	{
		wchar_t *buffer = new wchar_t[buffSize];
		DWORD result = GetShortPathNameW(reinterpret_cast<const wchar_t*>(longPath.utf16()), buffer, buffSize);

		if(result > 0 && result < buffSize)
		{
			shortPath = QString::fromUtf16(reinterpret_cast<const unsigned short*>(buffer));
		}

		delete[] buffer;
	}

	return (shortPath.isEmpty() ? longPath : shortPath);
}

const quint64 AbstractTool::currentTime(void)
{
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);

	ULARGE_INTEGER temp;
	temp.HighPart = fileTime.dwHighDateTime;
	temp.LowPart = fileTime.dwLowDateTime;

	return temp.QuadPart;
}

//
// Created by wangyaofu on 2018/6/4.
//

#ifndef VIDEO_PUSH_LOGTHREAD_H
#define VIDEO_PUSH_LOGTHREAD_H

#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include "singleton.h"

using std::ostringstream;
using std::string;

//
// max interval between writing
//
#define MAX_WRITEINTERVAL 2000000

//
// log write policy
//
enum ELogPolicy {
    kLogPolicyByRecords = 1, 	//write by records
    kLogPolicyByFileSize		//write by file size
};

/*
* log level comments.
* LL_ERROR: error
* LL_WARN:  warning
* LL_NOTICE:  notice
* LL_INFO:  important information
* LL_VARS:  variable info
* LL_DBG:  debug info
* LL_ALL:   all info, lowest level
*/
enum ELogLevel {
    LL_NONE = 0,
    LL_ERROR = 1,
    LL_WARN,
    LL_NOTICE,
    LL_INFO,
    LL_VARS,
    LL_DBG,
    LL_ALL
};

// the threhold that refresh logs
//
#define DEF_REFRESHLINES 1000

static const unsigned int kLogPrefixTime   = 0x01;
static const unsigned int kLogPrefixFileLineFunc = 0x02;
static const unsigned int kLogPrefixPidTid = 0x04;
static const unsigned int kLogNoThreadTag = 0x08;
static const unsigned int kLogPrefixDefault = (kLogPrefixTime | kLogPrefixFileLineFunc | kLogPrefixPidTid | kLogNoThreadTag);
//
// log thread class
//
// Note:
//   we must call the initLogFile(logfile, loglevel)
//   before run the log thread.
//
class LogThread {
public:
    //constructor: info from configuration
    //used for singleton mode
    LogThread() {
        mPolicy = kLogPolicyByRecords;
        mLPParam = 1000;
        mSwitchCount = 0;
        mCurRecords = 0;
        mLogFD = -1;
        mLevel = LL_INFO;
        mLogOption = kLogPrefixDefault;
        mLogFile = "log";
        mStopRequested = false;
        switchStream();
    }

    //constructor
    LogThread(const string& logfile, ELogLevel level,
              ELogPolicy policy, unsigned int lpparam)
            : mPolicy(policy),
              mLPParam(lpparam), mLogFile(logfile),
              mLevel(level), mLogOption(kLogPrefixDefault)
    {
        mSwitchCount = 0;
        mCurRecords = 0;
        mLogFD = -1;
        mStopRequested = false;
        switchStream();
    }

    virtual ~LogThread() {
        clean();
    }

    //copy constructor to init singleton LogThread
    LogThread(const LogThread &logthd)  {
        mPolicy = logthd.mPolicy;
        mLPParam = logthd.mLPParam;
        mSwitchCount = logthd.mSwitchCount;
        mCurRecords = logthd.mCurRecords;
        mLogFD = logthd.mLogFD;
        mLevel = logthd.mLevel;
        mStopRequested = logthd.mStopRequested;
        mLogOption = logthd.mLogOption;
        switchStream();
    }

public:
    int genLogHeaderInfo(const string& filename, int line, const string& funcName,
                         ELogLevel level, char* aOutBuf);
    //user interface
    bool coutLog(const string& filename, int line, const string& funcName,
                 ELogLevel level, const char *fmt, ...);
    //write normal log info
    bool writeLog(const string& filename, int line, const string& funcName,
                  ELogLevel level, const char *fmt, ...);
    void logHexAsc(const string& filename, int line, const string& funcName,
                   ELogLevel level, const void *aBuf, int aBufLen);
    void logRaw(ELogLevel level, const char* aRawBuf);

public:
    // start the thread.
    void run();
    void start();

    // stop the thread.
    void stop();

    // the condition of thread to start or stop.
    bool mStopRequested;

    //init log file with mLogFile and mLevel
    int initLogFile();
    int initLogFile(const string& logfile,
                    ELogLevel level,
                    unsigned int refreshlines = DEF_REFRESHLINES);

public:
    //Helpers
    ELogPolicy &logPolicy() {
        return mPolicy;
    }

    unsigned int &policyParam() {
        return mLPParam;
    }

    ELogLevel &logLevel() {
        return mLevel;
    }
    // set log level
    void setLogLevel(int aLevel) {
        mLevel = static_cast<ELogLevel>(aLevel);
    }
    // set log file name
    void setLogFileName(const string& aLogFileName) {
        mLogFile = aLogFileName;
    }
    // log option is set.
    void setLogOption(const unsigned int aOption) {
        mLogOption |= aOption;
    }

    // log option is cleared.
    void clearLogOption(const unsigned int aOption) {
        mLogOption &= ~aOption;
    }

protected:
    //clean
    void clean();

private:
    //switch the string stream
    //return old string stream
    ostringstream *switchStream();

    //check whether log info should write to file
    bool checkWritable();

    //get level string
    string getLevelString(ELogLevel level);

private:
    //log policy
    ELogPolicy mPolicy;

    //log policy parameter
    unsigned int mLPParam;

    //current writed log records
    //used by kLogPolicyByRecords
    unsigned int mCurRecords;

    //last write time
    struct timeval mLastWriteTime;

    string createFileNameWithDayStamp(const string& filename);
    //write to file
    void write2file(const string &loginfo);

private:
    //log file name
    string mLogFile;

    //log level that permitted output
    ELogLevel mLevel;

    // log option 1:file+line+func,2:datetime
    unsigned int mLogOption;

    //log file descriptor
    int mLogFD;

    //current log file with day stamp
    string mDSLogFile;

private:
    //string stream related
    //switch counter
    unsigned int mSwitchCount;

    //log info container
    ostringstream mInfoStream[2];

    //current log info container
    ostringstream *mCurInfoStream;

    // mutex for lock/unlock
    std::mutex mMutex;

    // mutex for cout lock/unlock
    std::mutex mCoutMutex;
};

// scope cost.
class ScopeCostLog {
    struct timeval timeval_;
    std::string funcName_;
    ELogLevel level_;
public:
    ScopeCostLog(const std::string& aFunc, ELogLevel level = LL_INFO);
    ~ScopeCostLog();
};

//singleton declaration
typedef Singleton<LogThread> LOGTHD_S;

// log instance
#define LOGTHD() LOGTHD_S::Instance()

#define LOG_COUT(level, fmt, arg...) do { \
    LOGTHD()->coutLog(__FILE__, __LINE__, __FUNCTION__, level, fmt, ## arg); \
} while (0)
//
// global routine to init or clean log thread
//
// start routine
bool startLogThread();

// stop routine
bool stopLogThread();

#endif //VIDEO_PUSH_LOGTHREAD_H

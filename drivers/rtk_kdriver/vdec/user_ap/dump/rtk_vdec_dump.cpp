#include <iostream>
#include <cstring>
#include <thread>
#include <deque>
#include <map>
using namespace std;

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <rtk_vdec.h>

// #define OUTPUT_STRING_TO_RTDLOG

#define LOG_PREFIX          "dump_pid_"
#define PROC_NAME           "rtk_vdec_dump"
#define PROC_FILE_PATH      "/tmp/"
#define VDEC_DRV_NAME       "rtkvdec"
#define VDEC_DRV_FILE_PATH  "/dev/"

class DumpCtrl : public VDEC_DUMP
{
public:
    DumpCtrl(VDEC_DUMP_CMD newCmd, unsigned long newDataAddr, unsigned int newDataSize)
    {
        cmd = newCmd;
        dataAddr = newDataAddr;
        dataSize = newDataSize;
    }
};

class DumpBase
{
protected:
    static int currVdecDrvFd;
    long currPid = -1;
    string dumpName = "";

    template<typename... Args>
    void outputString(const char *format, Args... args)
    {
        size_t formatLength = snprintf(nullptr, 0, format, args...);

        if (formatLength)
        {
            size_t prefixLength = snprintf(nullptr, 0, "[%s%ld]%s ", LOG_PREFIX, currPid, dumpName.c_str());
            int formatBufSize = formatLength + 1;
            int finalBufSize = formatBufSize + prefixLength;
            char *formatBuf = new char[formatBufSize];
            char *finalBuf = new char[finalBufSize];

            snprintf(formatBuf, formatBufSize, format, args...);
            snprintf(finalBuf, finalBufSize, "[%s%ld]%s %s", LOG_PREFIX, currPid, dumpName.c_str(), formatBuf);
            #ifdef OUTPUT_STRING_TO_RTDLOG
            DumpCtrl dumpCtrl(VDEC_DUMP_LOG, (unsigned int)finalBuf, finalBufSize);
            vdecDumpCtrl(&dumpCtrl);
            #else
            cout << string(finalBuf);
            #endif

            delete[] formatBuf;
            delete[] finalBuf;
        }
    }
    void vdecDumpCtrl(DumpCtrl *dumpCtrl)
    {
        if (currVdecDrvFd == -1)
            return;

        switch(dumpCtrl->cmd)
        {
            default:
                break;
        }

        ioctl(currVdecDrvFd, VDEC_IOC_DUMP, dynamic_cast<VDEC_DUMP *>(dumpCtrl));
    }

public:
    DumpBase()
    {
        currPid = getpid();
    }
};

int DumpBase::currVdecDrvFd = -1;

enum class DumpDirection : int
{
    DUMP_NONE,
    DUMP_WRITE,
    DUMP_READ,
};

class DumpObj : public DumpBase
{
private:
    deque<VDEC_DUMP_SIG_CMD> sigCmdQue;
    pthread_t dumpThread;
    int fileFd = -1;
    int fileSize = 0;
    bool isFileLoad = false;
    DEBUG_BUFFER_HEADER *virDbgBufHdr = nullptr;
    unsigned int phyDbgBufHdrAddr = 0;
    bool isPostRelease = false;

    static void *dumpThreadHandler(void *ctx)
    {
        if (ctx)
        {
            DumpObj *dumpObj = static_cast<DumpObj *>(ctx);
            dumpObj->dumpThreadProc();
        }
    }
    void startDumpSignal()
    {
        DumpCtrl dumpCtrl(getDumpSignalCmdStart(), (unsigned int)this, 0);
        vdecDumpCtrl(&dumpCtrl);
    }
    void stopDumpSignal()
    {
        DumpCtrl dumpCtrl(getDumpSignalCmdStop(), 0, 0);
        vdecDumpCtrl(&dumpCtrl);
    }
    void startDumpThread()
    {
        if (!isDumpThreadRun)
        {
            isDumpThreadRun = true;
            pthread_create(&dumpThread, nullptr, dumpThreadHandler, static_cast<void *>(this));
        }
    }
    void stopDumpThread()
    {
        if (isDumpThreadRun)
        {
            isDumpThreadRun = false;
            pthread_join(dumpThread, nullptr);
        }
    }
    void dumpWriteOperation()
    {
        if (fileFd > -1 && virDbgBufHdr && phyDbgBufHdrAddr)
        {
            unsigned int virDbgBufHdrAddr, magic, size, wr, rd;
            char *wrPtr, *rdPtr, *basePtr, *limitPtr;

            virDbgBufHdrAddr = (unsigned int)virDbgBufHdr;
            magic = __builtin_bswap32(virDbgBufHdr->magic);
            size = __builtin_bswap32(virDbgBufHdr->size);
            rd = __builtin_bswap32(virDbgBufHdr->rd);
            wr = __builtin_bswap32(virDbgBufHdr->wr);

            wrPtr = (char *)(virDbgBufHdrAddr + wr - phyDbgBufHdrAddr); /* make virtual address */
            rdPtr = (char *)(virDbgBufHdrAddr + rd - phyDbgBufHdrAddr); /* make virtual address */
            basePtr  = (char *)(virDbgBufHdrAddr + sizeof(DEBUG_BUFFER_HEADER));
            size -= sizeof(DEBUG_BUFFER_HEADER);
            limitPtr = basePtr+ size;

            if (wrPtr < rdPtr)
                wrPtr = wrPtr + size;

            if (wrPtr > rdPtr)
            {
                if (wrPtr > limitPtr)
                {
                    write(fileFd, rdPtr, limitPtr -rdPtr);
                    write(fileFd, basePtr, wrPtr - limitPtr);
                }
                else
                    write(fileFd, rdPtr, wrPtr - rdPtr);
                virDbgBufHdr->rd = __builtin_bswap32(wr) ;
            }
        }
    }
    void dumpReadOperation()
    {
        if (fileFd > -1 && !isFileLoad && virDbgBufHdr && phyDbgBufHdrAddr)
        {
            unsigned int virDbgBufHdrAddr;
            char *basePtr;

            virDbgBufHdrAddr = (unsigned int)virDbgBufHdr;
            basePtr  = (char *)(virDbgBufHdrAddr + sizeof(DEBUG_BUFFER_HEADER));
            read(fileFd, basePtr, fileSize);

            DumpCtrl dumpCtrl(VDEC_DUMP_IMPORT_FILE_LOAD, 0, 0);
            vdecDumpCtrl(&dumpCtrl);
            isFileLoad = true;
        }
    }
    void dumpDbgBufHdr()
    {
        DumpCtrl dumpCtrlSetCtx(VDEC_DUMP_SET_MMAP_USER_CTX, (unsigned int)this, 0);
        vdecDumpCtrl(&dumpCtrlSetCtx);

        virDbgBufHdr = (DEBUG_BUFFER_HEADER *)mmap(0, DUMP_ES_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, currVdecDrvFd, 0);
        if (virDbgBufHdr == MAP_FAILED)
            outputString("mmap debug buffer header failed!\n");
        else
        {
            unsigned int targetAddr;
            unsigned int ctxAddr = (unsigned int)static_cast<void *>(this);
            memcpy(&targetAddr, &ctxAddr, sizeof(unsigned int));
            DumpCtrl dumpCtrl(VDEC_DUMP_DBG_BUF_HDR, (unsigned int)&targetAddr, sizeof(targetAddr));
            vdecDumpCtrl(&dumpCtrl);
            phyDbgBufHdrAddr = targetAddr;
        }

        DumpCtrl dumpCtrlResetCtx(VDEC_DUMP_RESET_MMAP_USER_CTX, 0, 0);
        vdecDumpCtrl(&dumpCtrlResetCtx);
    }
    void dumpSignalCmd(VDEC_DUMP_SIG_CMD vdecDumpSigCmd)
    {
        if (vdecDumpSigCmd == VDEC_DUMP_SIG_FILE_OPEN)
        {
            char filePath[100];
            unsigned int ctxAddr = (unsigned int)static_cast<void *>(this);
            memcpy(filePath, &ctxAddr, sizeof(unsigned int));
            DumpCtrl dumpCtrlFile(VDEC_DUMP_FILE_PATH, (unsigned int)filePath, sizeof(filePath));
            vdecDumpCtrl(&dumpCtrlFile);

            DumpDirection dumpDirection = getDumpDirection();
            if (dumpDirection == DumpDirection::DUMP_WRITE)
                fileFd = open(filePath, O_CREAT | O_WRONLY, S_IRWXU);
            else if (dumpDirection == DumpDirection::DUMP_READ)
                fileFd = open(filePath, O_RDONLY);
            if (fileFd < 0)
            {
                outputString("open %s failed!\n", filePath);
                return;
            }

            if (dumpDirection == DumpDirection::DUMP_READ)
            {
                fileSize = lseek(fileFd, 0, SEEK_END);
                lseek(fileFd, 0, SEEK_SET);
                DumpCtrl dumpCtrl(VDEC_DUMP_IMPORT_FILE_SIZE, (unsigned int)&fileSize, sizeof(fileSize));
                vdecDumpCtrl(&dumpCtrl);
            }
            else
                dumpDbgBufHdr();
        }
        else if (vdecDumpSigCmd == VDEC_DUMP_SIG_FILE_LOAD)
        {
            dumpDbgBufHdr();
        }
        else if (vdecDumpSigCmd == VDEC_DUMP_SIG_FILE_CLOSE)
        {
            if (virDbgBufHdr)
            {
                if (virDbgBufHdr != MAP_FAILED)
                    munmap(virDbgBufHdr, DUMP_ES_SIZE);
                virDbgBufHdr = nullptr;
            }
            if (fileFd > -1)
            {
                close(fileFd);
                fileFd = -1;
            }
            isPostRelease = true;
        }
    }

protected:
    bool isDumpThreadRun = false;

    void startObjs()
    {
        startDumpThread();
        startDumpSignal();
    }
    void stopObjs()
    {
        if (virDbgBufHdr)
        {
            if (virDbgBufHdr != MAP_FAILED)
                munmap(virDbgBufHdr, DUMP_ES_SIZE);
            virDbgBufHdr = nullptr;
        }
        if (fileFd > -1)
        {
            close(fileFd);
            fileFd = -1;
        }
        sigCmdQue.clear();
        stopDumpSignal();
        stopDumpThread();
    }
    virtual int getDumpSignalNum()
    {
        return 0;
    }
    virtual VDEC_DUMP_CMD getDumpSignalCmdStart()
    {
        return VDEC_DUMP_DEFAULT;
    }
    virtual VDEC_DUMP_CMD getDumpSignalCmdStop()
    {
        return VDEC_DUMP_DEFAULT;
    }
    virtual DumpDirection getDumpDirection()
    {
        return DumpDirection::DUMP_NONE;
    }
    virtual void dumpThreadProc()
    {
        while(isDumpThreadRun)
        {
            VDEC_DUMP_SIG_CMD vdecDumpSigCmd = VDEC_DUMP_SIG_DEFAULT;
            sigset_t newSigSet, oldSigSet;
            sigemptyset(&newSigSet);
            sigaddset(&newSigSet, getDumpSignalNum());
            sigprocmask(SIG_BLOCK, &newSigSet, &oldSigSet);
            if (sigCmdQue.size())
            {
                vdecDumpSigCmd = sigCmdQue.front();
                sigCmdQue.pop_front();
            }
            sigprocmask(SIG_SETMASK, &oldSigSet, NULL);
            dumpSignalCmd(vdecDumpSigCmd);

            DumpDirection dumpDirection = getDumpDirection();
            if (dumpDirection == DumpDirection::DUMP_WRITE)
                dumpWriteOperation();
            else if (dumpDirection == DumpDirection::DUMP_READ)
                dumpReadOperation();

            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }

public:
    bool getPostRelease()
    {
        return isPostRelease;
    }
    virtual void dumpSignalProc(VDEC_DUMP_SIG_CMD vdecDumpSigCmd)
    {
        sigCmdQue.push_back(vdecDumpSigCmd);
    }
    virtual ~DumpObj()
    {
    }
};

class DumpEs : public DumpObj
{
private:
    int getDumpSignalNum()
    {
        return RTKVDEC_SIG_NUM_DUMP_ES;
    }
    VDEC_DUMP_CMD getDumpSignalCmdStart()
    {
        return VDEC_DUMP_ES_SIG_START;
    }
    VDEC_DUMP_CMD getDumpSignalCmdStop()
    {
        return VDEC_DUMP_ES_SIG_STOP;
    }
    DumpDirection getDumpDirection()
    {
        return DumpDirection::DUMP_WRITE;
    }

public:
    DumpEs()
    {
        dumpName = "[es]";
        startObjs();
    }
    virtual ~DumpEs()
    {
        stopObjs();
    }
};

class CrcExport : public DumpObj
{
private:
    int getDumpSignalNum()
    {
        return RTKVDEC_SIG_NUM_CRC_EXPORT;
    }
    VDEC_DUMP_CMD getDumpSignalCmdStart()
    {
        return VDEC_DUMP_CRC_EXPORT_SIG_START;
    }
    VDEC_DUMP_CMD getDumpSignalCmdStop()
    {
        return VDEC_DUMP_CRC_EXPORT_SIG_STOP;
    }
    DumpDirection getDumpDirection()
    {
        return DumpDirection::DUMP_WRITE;
    }

public:
    CrcExport()
    {
        dumpName = "[crc_export]";
        startObjs();
    }
    virtual ~CrcExport()
    {
        stopObjs();
    }
};

class CrcImport : public DumpObj
{
private:
    int getDumpSignalNum()
    {
        return RTKVDEC_SIG_NUM_CRC_IMPORT;
    }
    VDEC_DUMP_CMD getDumpSignalCmdStart()
    {
        return VDEC_DUMP_CRC_IMPORT_SIG_START;
    }
    VDEC_DUMP_CMD getDumpSignalCmdStop()
    {
        return VDEC_DUMP_CRC_IMPORT_SIG_STOP;
    }
    DumpDirection getDumpDirection()
    {
        return DumpDirection::DUMP_READ;
    }

public:
    CrcImport()
    {
        dumpName = "[crc_import]";
        startObjs();
    }
    virtual ~CrcImport()
    {
        stopObjs();
    }
};

enum class DumpMainState : int
{
    DUMP_MAIN_STATE_VDEC_DRV,
    DUMP_MAIN_STATE_LOCK_FILE,
    DUMP_MAIN_STATE_PROC,
};

class DumpMain : private DumpBase
{
private:
    static DumpObj *currDumpObj;
    int vdecDrvFd = -1;
    int lockFileFd = -1;
    map<int, string> fileMap;
    DumpMainState dumpMainState=DumpMainState::DUMP_MAIN_STATE_VDEC_DRV;

    static void dumpSignalHandler(int n, siginfo_t *info, void *unused)
    {
        if (info)
        {
            if (n == RTKVDEC_SIG_NUM_DUMP_ES)
            {
                if (!currDumpObj)
                    currDumpObj = new DumpEs();
            }
            else if (n == RTKVDEC_SIG_NUM_CRC_EXPORT)
            {
                if (!currDumpObj)
                    currDumpObj = new CrcExport();
            }
            else if (n == RTKVDEC_SIG_NUM_CRC_IMPORT)
            {
                if (!currDumpObj)
                    currDumpObj = new CrcImport();
            }

            if (currDumpObj)
                currDumpObj->dumpSignalProc((VDEC_DUMP_SIG_CMD)info->si_value.sival_int);
        }
    }
    void freeFileRes()
    {
        if (lockFileFd > -1)
        {
            outputString("close %s!\n", fileMap[lockFileFd].c_str());
            close(lockFileFd);
            lockFileFd = -1;
        }
        if (vdecDrvFd > -1)
        {
            outputString("close %s!\n", fileMap[vdecDrvFd].c_str());
            close(vdecDrvFd);
            vdecDrvFd = -1;
        }
        fileMap.clear();
    }
    bool linkVdecDrv()
    {
        bool isLinkVdecDrv = false;
        char fileName[128] = {0};

        snprintf(fileName, sizeof(fileName), "%s%s", VDEC_DRV_FILE_PATH, VDEC_DRV_NAME);
        vdecDrvFd = open(fileName, O_RDWR);
        if (vdecDrvFd >= 0)
        {
            fileMap[vdecDrvFd] = string(fileName);
            currVdecDrvFd = vdecDrvFd;
            isLinkVdecDrv = true;
        }
        outputString("open %s %s!\n", fileName, (isLinkVdecDrv)? "successfully" : "failed");

        return isLinkVdecDrv;
    }
    bool lockFile()
    {
        bool isLockFile = false;
        char buf[16] = {0};
        char fileName[128] = {0};

        snprintf(fileName, sizeof(fileName), "%s.file_lock_%s", PROC_FILE_PATH, PROC_NAME);
        lockFileFd = open(fileName, O_CREAT | O_RDWR, S_IRWXU);

        if (lockFileFd >= 0)
        {
            struct flock fl;

            fl.l_start = 0;
            fl.l_len = 0;
            fl.l_type = F_WRLCK;
            fl.l_whence = SEEK_SET;

            fileMap[lockFileFd] = string(fileName);
            outputString("open %s successfully!\n", fileName);

            if (fcntl(lockFileFd, F_SETLK, &fl) == -1)
            {
                if (fcntl(lockFileFd, F_GETLK, &fl) == 0)
                    outputString("%s has been run by pid %ld!\n", PROC_NAME, fl.l_pid);
                freeFileRes();
            }
            else
            {
                // ftruncate(lockFileFd, 0);
                snprintf(buf, sizeof(buf), "%ld", currPid);
                write(lockFileFd, buf, strlen(buf) + 1);
                DumpCtrl dumpCtrl(VDEC_DUMP_SET_USER_TASK, 0, 0);
                vdecDumpCtrl(&dumpCtrl);
                outputString("lock %s done!\n", fileName);
                isLockFile = true;
            }
        }
        else
            outputString("open %s failed!\n", fileName);

        return isLockFile;
    }

public:
    ~DumpMain()
    {
        if (currDumpObj)
            if (currDumpObj->getPostRelease())
            {
                delete currDumpObj;
                currDumpObj = nullptr;
            }
        DumpCtrl dumpCtrl(VDEC_DUMP_RESET_USER_TASK, 0, 0);
        vdecDumpCtrl(&dumpCtrl);
        freeFileRes();
    }
    void runStateLoop()
    {
        while(1)
        {
            switch(dumpMainState)
            {
                case DumpMainState::DUMP_MAIN_STATE_VDEC_DRV:
                {
                    if (linkVdecDrv())
                        dumpMainState = DumpMainState::DUMP_MAIN_STATE_LOCK_FILE;
                    else
                        return;
                }
                case DumpMainState::DUMP_MAIN_STATE_LOCK_FILE:
                {
                    if (lockFile())
                        dumpMainState = DumpMainState::DUMP_MAIN_STATE_PROC;
                    else
                        return;
                }
                case DumpMainState::DUMP_MAIN_STATE_PROC:
                {
                    struct sigaction act;
                    sigemptyset(&act.sa_mask);
                    // act.sa_flags = (SA_SIGINFO | SA_RESTART);
                    act.sa_flags = (SA_SIGINFO | SA_NODEFER);
                    act.sa_sigaction = dumpSignalHandler;
                    sigaction(RTKVDEC_SIG_NUM_DUMP_ES, &act, NULL);
                    sigaction(RTKVDEC_SIG_NUM_CRC_EXPORT, &act, NULL);
                    sigaction(RTKVDEC_SIG_NUM_CRC_IMPORT, &act, NULL);
                    break;
                }
                default:
                    break;
            }

            if (currDumpObj)
                if (currDumpObj->getPostRelease())
                {
                    delete currDumpObj;
                    currDumpObj = nullptr;
                }
        }
    }
};

DumpObj *DumpMain::currDumpObj = nullptr;

int main()
{
    DumpMain dumpMain;
    dumpMain.runStateLoop();
    return 0;
}
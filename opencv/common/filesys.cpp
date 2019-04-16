/*
** Copyright (C) 2014 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class FileSys.
*/

#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "filesys.h"

using namespace std;

namespace common {

// \param file �ļ�·����
// \retval true �ļ�����
// \retval false ������
bool FileSys::IsFileExist(const string &file) {
    //return (access(file.c_str(), F_OK) == 0) ? true : false;
    struct stat buff;
    int err = stat(file.c_str(), &buff);

    if (err == -1) {
        if (errno == ENOENT || errno == ENOTDIR) {
            return false;
        }
        return false;
    }

    return true;
}

// \param file �ļ�·����
// \retval true �ļ�������Ϊ��������
// \retval false �����ڻ��߲��Ƿ�������
bool FileSys::IsLink(const string &file) {
    struct stat statbuf;

    if(lstat(file.c_str(), &statbuf) == 0 && S_ISLNK(statbuf.st_mode) != 0) {
        return true;
    }
    return false;
}

// \param dile Ŀ¼·����
// \retval true Ŀ¼����
// \retval false �����ڻ��߲���Ŀ¼
bool FileSys::IsDir(const string &file) {
    struct stat statbuf;

    if(stat(file.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode) != 0) {
        return true;
    }
    return false;
}

FileSys::Type getType(const std::string& path) {
    struct stat st;
    if (0 != ::stat(path.c_str(), &st)) {
        return FileSys::Invalid;
    }

    if (S_ISREG(st.st_mode)) {
        return FileSys::File;
    }
    else if (S_ISDIR(st.st_mode)) {
        return FileSys::Directory;
    }
    else if (S_ISCHR(st.st_mode)) {
        return FileSys::Chardev;
    }
    else if (S_ISBLK(st.st_mode)) {
        return FileSys::Blockdev;
    }
    else if (S_ISFIFO(st.st_mode)) {
        return FileSys::Fifo;
    }
    else if (S_ISSOCK(st.st_mode)) {
        return FileSys::Symlink;
    }

    return FileSys::File;
}
// \param srcfile ԭ�ļ���
// \param destfile ��������
// \retval true �����ɹ�
// \retval false ���ɹ�
bool FileSys::link(const string &srcfile, const string &destfile) {
    if (::link(srcfile.c_str(), destfile.c_str()) == 0)
        return true;
    else
        return false;
}

bool FileSys::symlink(const string &srcfile, const string &destfile) {
    if (::symlink(srcfile.c_str(), destfile.c_str()) == 0)
        return true;
    else
        return false;
}

// \param file �ļ�·����
// \return ���ļ������򷵻ش�С,���򷵻�-1
size_t FileSys::FileSize(const string &file) {
    struct stat statbuf;

    if(stat(file.c_str(), &statbuf)==0)
        return statbuf.st_size;
    else
        return 0;
}

bool FileSys::resize(const string& path, size_t newSize) {
    int ret = 0;
    do {
        ret = truncate(path.c_str(), newSize);
    } while (ret == EINTR);

    return ret == 0;
}

uint64_t FileSys::fileNode(const string &file) {
    struct stat statbuf;

    if (stat(file.c_str(), &statbuf) == 0)
        return statbuf.st_ino;
    else
        return 0;
}
// \param file �ļ�·����
// \return ���ļ������򷵻���������ʱ��,���򷵻�-1
time_t FileSys::FileTime(const string &file) {
    struct stat statbuf;

    if(stat(file.c_str(), &statbuf)==0)
        return statbuf.st_mtime;
    else
        return -1;
}

// \param file �ļ�·����
// \return ����ȡ���ļ�·���򷵻�,���򷵻ؿ��ַ���
string FileSys::FilePath(const string &file) {
    size_t p;
    if ((p=file.rfind("/")) != file.npos)
        return file.substr(0, p);
    else if ((p=file.rfind("\\")) != file.npos)
        return file.substr(0, p);
    return string("");
}

// \param file �ļ�·����
// \return ����ȡ���ļ������򷵻�,���򷵻�ԭ�ļ�·������
string FileSys::FileName(const string &file) {
    size_t p;
    if ((p=file.rfind("/")) != file.npos)
        return file.substr(p+1);
    else if ((p=file.rfind("\\")) != file.npos)
        return file.substr(p+1);
    return file;
}

// libm.so --> m   libmylib.so --> mylib
string FileSys::OnlyLibName(const string& libName) {
    string fileName = FileName(libName);
    if (fileName.compare(0, 3, "lib") == 0) {
        fileName.erase(0, 3);
    }
    size_t p;
    if ((p = fileName.rfind(".so")) != fileName.npos) {
        fileName.erase(p, 3);
    }
    if ((p = fileName.rfind(".sl")) != fileName.npos) {
        fileName.erase(p, 3);
    }
    return fileName;
}

// \param oldname ԭ�ļ���
// \param newname ���ļ���
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::RenameFile(const string &oldname, const string &newname) {
    if (::rename(oldname.c_str(), newname.c_str()) != -1)
        return true;
    else
        return false;
}

// \param srcfile ԭ�ļ���
// \param destfile Ŀ���ļ���,�ļ�����Ϊ0666
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::CopyFile(const string &srcfile, const string &destfile) {
    FILE *src=NULL, *dest=NULL;
    if ((src=fopen(srcfile.c_str(),"rb")) == NULL) {
        return false;
    }
    if ((dest=fopen(destfile.c_str(),"wb+")) == NULL) {
        fclose(src);
        return false;
    }

    const static int bufsize = 8192;
    char buf[bufsize];
    size_t n;
    while ((n=fread(buf, 1, bufsize, src)) >= 1) {
        if (fwrite(buf, 1, n, dest) != n) {
            fclose(src);
            fclose(dest);
            return false;
        }
    }

    fclose(src);
    fclose(dest);

    //chmod to 0666
    mode_t mask = umask(0);
    chmod(destfile.c_str(), S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    umask(mask);

    return true;
}

// \param file �ļ�·����
// \retval true ɾ���ɹ�
// \retval false �ļ������ڻ���ɾ��ʧ��
bool FileSys::DelFile(const string &file) {
    if (remove(file.c_str()) == 0)
        return true;
    else
        return false;
}

// \param srcfile ԭ�ļ���
// \param destfile ���ļ���
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::MoveFile(const string &srcfile, const string &destfile) {
    if (RenameFile(srcfile,destfile))
        return true;

    // rename fail, copy and delete file
    if (CopyFile(srcfile,destfile)) {
        if (DelFile(srcfile))
            return true;
    }

    return false;
}

// \param dir ����ΪĿ¼·����
// \return ���ؽ��Ϊ�ļ�����Ŀ¼�б�,��Ŀ¼�ĵ�һ���ַ�Ϊ '/',
// ���ؽ���в���������ǰ����һ��Ŀ¼�� "/.", "/.."
void FileSys::ListFiles(const string &dir, vector<string> *files) {
    string file;
    DIR *pdir = NULL;
    dirent *pdirent = NULL;

    if ((pdir=opendir(dir.c_str())) != NULL) {
        while ((pdirent=readdir(pdir)) != NULL) {
            file = pdirent->d_name;
            if (file!="." && file!="..") {
                if (IsDir(dir+"/"+file))
                    file = "/"+file;
                files->push_back(file);
            }
        }
        closedir(pdir);
    }
}

// \param dir Ҫ������Ŀ¼,���ϲ�Ŀ¼���������Զ�����
// \param mode ����Ŀ¼Ȩ��,Ĭ��ΪS_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH(0755)
// �ɹ�����true, ���򷵻�false.
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::CreateDir(const string &dir, const mode_t mode) {
    // check
    size_t len = dir.length();
    if (len <= 0) return false;

    string tomake;
    char curchr;
    for(size_t i=0; i<len; ++i) {
        // append
        curchr = dir[i];
        tomake += curchr;
        if (curchr=='/' || i==(len-1)) {
            // need to mkdir
            if (!IsFileExist(tomake) && !IsDir(tomake)) {
                // able to mkdir
                mode_t mask = umask(0);
                if (mkdir(tomake.c_str(),mode) == -1) {
                    umask(mask);
                    return false;
                }
                umask(mask);
            }
        }
    }

    return true;
}

// \param srcdir ԭĿ¼
// \param destdir Ŀ��Ŀ¼
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::CopyDir(const string &srcdir, const string &destdir) {
    vector<string> files;
    ListFiles(srcdir, &files);
    string from;
    string to;

    // ����Ŀ��Ŀ¼
    if (!IsFileExist(destdir))
        CreateDir(destdir);

    for (size_t i=0; i<files.size(); ++i) {
        from = srcdir + "/" + files[i];
        to = destdir + "/" + files[i];

        // ��Ŀ¼,�ݹ����
        if (files[i][0] == '/') {
             if (!CopyDir(from,to))
                return false;
        } else if (!CopyFile(from,to)) {
            return false;
        }
    }

    return true;
}

// \param dir Ҫɾ����Ŀ¼
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::DelDir(const string &dir) {
    vector<string> files;
    ListFiles(dir, &files);
    string todel;

    // ɾ���ļ�
    for (size_t i=0; i<files.size(); ++i) {
        todel = dir + "/" + files[i];

        // ��Ŀ¼,�ݹ����
        if (files[i][0] == '/') {
             if (!DelDir(todel))
                return false;
        } else if (!DelFile(todel)) {
            return false;
        }
    }

    // ɾ��Ŀ¼
    if (rmdir(dir.c_str()) == 0)
        return true;

    return false;
}

// \param srcdir ԭĿ¼
// \param destdir Ŀ��Ŀ¼
// \retval true �����ɹ�
// \retval false ʧ��
bool FileSys::MoveDir(const string &srcdir, const string &destdir) {
    if (RenameFile(srcdir,destdir))
        return true;

    // rename fail, copy and delete dir
    if (CopyDir(srcdir,destdir)) {
        if (DelDir(srcdir))
            return true;
    }

    return false;
}

// \param fd �ļ����
// \param type ��ģʽ����ѡF_WRLCK��F_RDLCK��F_UNLCK
void FileSys::LockFile(int fd, const int type) {
    struct flock lck;
    lck.l_start = 0;
    lck.l_len = 0;
    lck.l_whence = SEEK_SET;

    lck.l_type = type;
    if (type==F_WRLCK || type==F_RDLCK)
        lck.l_type = type;
    else
        lck.l_type = F_UNLCK;

    while (fcntl(fd,F_SETLKW,&lck)==-1 && errno==EINTR);
    return;
}

// \param fd �ļ����
// \retval true �ļ��ѱ���
// \retval false �ļ�δ����
bool FileSys::IsLocked(int fd) {
    struct flock lck;
    lck.l_start = 0;
    lck.l_len = 0;
    lck.l_whence = SEEK_SET;
    lck.l_type = F_WRLCK;

    while ((fcntl(fd, F_GETLK, &lck))==-1 && errno==EINTR);

    if (lck.l_type == F_UNLCK)
        return false;
    else
        return true;
}

// \param file �ļ�·��
// \param mode �ļ���ģʽ����fopen()ͬ����������ͬ
// \param type ��ģʽ����ѡF_WRLCK��F_RDLCK��F_UNLCK
// \return �ļ������ʧ�ܷ���NULL
FILE* FileSys::LockFileOpen(const string &file, const char* mode, const int type) {
    FILE *fp;
    mode_t mask = umask(0);
    int fd = open(file.c_str(), O_CREAT|O_EXCL|O_RDWR,
                   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if (fd >= 0) {
        // file not exist, create success
        LockFile(fd, type);
        fp = fdopen(fd, mode);
    } else {
        // file exist
        fp = fopen(file.c_str(), mode);
        if (fp == NULL)
            return NULL;
        LockFile(fileno(fp), type);
    }
    umask(mask);

    return fp;
}

std::string FileSys::loadFile(const std::string& file) {
    size_t buffer_length = 0;
    std::ifstream data_file(file, std::ifstream::binary);
    if (data_file.is_open()) {
        std::stringstream buffer;
        buffer << data_file.rdbuf();
        return buffer.str();
    }
    else {
        return "";
    }
}

int FileSys::appendFile(const string& file, const string& data) {
    return appendFile(file, data.data(), data.length());
}

int FileSys::overWriteFile(const string &file, const string &data) {
    return overWriteFile(file, data.data(), data.length());
}

int FileSys::appendFile(const string& file, const char* dataPtr, int len) {
    std::ofstream ofs(file, std::ofstream::app | std::ofstream::binary);
    if (ofs.is_open() && ofs.good()) {
        ofs.write(dataPtr, len);
    }
	ofs.close();
    return len;
}

int FileSys::overWriteFile(const string& file, const char* dataPtr, int len) {
    std::ofstream ofs(file, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
    if (ofs.is_open() && ofs.good()) {
        ofs.write(dataPtr, len);
    }
	ofs.close();
    return len;
}

bool FileSys::chdir(const std::string& path) {
    return (0 == ::chdir(path.c_str()));
}

std::string FileSys::cwd() {
    std::vector<char> cwd(1024);

    while (getcwd(&cwd[0], cwd.size()) == 0) {
        if (errno == ERANGE)
            cwd.resize(cwd.size() * 2);
    }

    return std::string(&cwd[0]);
}


std::string FileSys::curdir() {
    return ".";
}


std::string FileSys::updir() {
    return "..";
}


std::string FileSys::rootdir() {
    return "/";
}


std::string FileSys::tmpdir() {
    const char* tmpdir = getenv("TEMP");

    if (tmpdir) {
        return tmpdir;
    }

    tmpdir = getenv("TMP");
    if (tmpdir) {
        return tmpdir;
    }

    return FileSys::IsFileExist("/tmp") ? "/tmp" : curdir();
}


std::string FileSys::sep()
{
    return "/";
}

std::string FileSys::getExecPath() {
    char result[PATH_MAX];
    ssize_t count = ::readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
}
} // namespace common

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
using namespace std;
string log_name = "";
void gettime(char *buf,bool withms=true)
{
    struct tm *tt;
    struct timeval tv;
    char t[50]={0};
    char mt[10]={0};
    gettimeofday(&tv,NULL);
    tt = localtime(&(tv.tv_sec));     
    strftime(t,50,"%Y%m%d_%H%M%S",tt); 
    if (withms)
    {
        sprintf(mt,"%03ld",tv.tv_usec/1000);  //毫秒提取                 
        sprintf(buf,"%s.%s",t,mt);
    }
    else
    {            
        sprintf(buf,"%s",t);
    }
    
    tt=NULL;
}
int vscprintf (const char * format, va_list pargs) { 
      int retval; 
      va_list argcopy; 
      va_copy(argcopy, pargs); 
      retval = vsnprintf(NULL, 0, format, argcopy); 
      va_end(argcopy); 
      return retval; 
}
void hvtelog(const char *cmd, ...)
{
    char timenow[100]={0};
    std::string str;
    va_list args;
    va_start(args, cmd);
    {
        int nLength = vscprintf(cmd, args);
        nLength += 1; //上面返回的长度是包含\0，这里加�?
        std::vector<char> vectorChars(nLength);
        vsnprintf(vectorChars.data(), nLength, cmd, args);
        str.assign(vectorChars.data());
    }
    va_end(args);
    gettime(timenow);
    printf("[%s] %s\n", timenow,str.c_str()); //stdout输出
    if (log_name != "")
    {
        try
        {
            FILE *m_file = fopen(log_name.c_str(), "a+");
            if (m_file == NULL)
            {
                perror("open (log)");
            }
            else
            {
                fprintf(m_file, "[%s] %s\n", timenow, str.c_str());
                fclose(m_file);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void setLogName(string s)
{
    char timeStr[100] = {0};
	gettime(timeStr,false);
    log_name=timeStr;
    log_name=s+"."+log_name;
    printf("config hvtelog file with %s\n", log_name.c_str());
}

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <bits/stdc++.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "csapp.h"
#include <stdio.h>
#include <fstream>
using namespace std;

#define CLEARLIST lists.clear();
#define CLEARFILES files.clear();
#define CLEARDIRECTORIES directories.clear();
#define CLEARDIRCHECK dircheck.clear();
#define WINDOW 20

#define CLEARSCREEN cout<<"\033c";
#define SETTERMINAL cout << "\e[8;150;200t";
#define SETCURSOR00 cout << "\033[H";
#define cursorright(x) cout<<"\033[1D"
#define cursorleft(x) cout<<"\033[1C"
#define cursorup(x) cout<<"\033[1A"
#define cursordown(x) cout<<"\033[1B"

#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000a
#define BACKSPACE   0x0008
#define KEY_UP      0x0105
#define KEY_DOWN    0x0106
#define KEY_LEFT    0x0107
#define KEY_RIGHT   0x0108
#define HOME        0X0104
#define COLON       0X0058

int start1, list_start, list_end, end1, flag=0;
stack <string> leftstack, rightstack;
vector <string> lists, files, directories;
vector <int> dircheck;
string str1="",str2="";
static struct termios term, oterm;

static int getch(void);
static int kbhit(void);
static int kbesc(void);
static int kbget(void);
void command_mode();
void normal_mode(string);

static int getch(void)
{
    int c = 0;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    return c;
}

static int kbhit(void)
{
    int c = 0;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    if (c != -1) ungetc(c, stdin);
    return ((c != -1) ? 1 : 0);
}

static int kbesc(void)
{
    int c;

    if (!kbhit()) return KEY_ESCAPE;
    c = getch();

    if (c == '[') 
    {
        switch (getch()) 
        {
            case 'A':
                c = KEY_UP;
                break;
            case 'B':
                c = KEY_DOWN;
                break;
            case 'C':
                c = KEY_RIGHT;
                break;
            case 'D':
                c = KEY_LEFT;
                break;
            default:
                c = 0;
                break;
        }
    } 
    else 
    {
        c = 0;
    }
    if (c == 0) while (kbhit()) getch();
    return c;
}

static int kbget(void)
{
    int c;

    c = getch();
    //cout<<c;
    if(c==127)
    {
        c=BACKSPACE;
        return c;
    }
    if(c==104 && flag==1)
    {
        c=HOME;
        return c;
    }
    if(c==58)
    {
        c=COLON;
        return c;
    }
    return (c == KEY_ESCAPE) ? kbesc() : c;
}

string get_current_dir() 
{
   char buff[FILENAME_MAX]; 
   getcwd(buff, FILENAME_MAX);
   string current_working_dir(buff);
   return current_working_dir;
}

void meta(const char *filepath)
{
	struct stat stats;	
	struct passwd *pwdd;
   	struct group *grp;
	stat(filepath, &stats); 
	string s=filepath;
	size_t pos;
	pos = s.find_last_of('/');
	string name1 = s.substr(pos+1,s.length());
	string name="";
	for(int i=0;i<20&&i<name1.size();i++)
	{
		if(i<=16)
		name+=name1[i];
		else
		name+='.';
	}
	char tmp[500];
	strcpy(tmp,name.c_str());
    printf("%-20s\t",tmp); 
    
    printf( (S_ISDIR(stats.st_mode)) ? "d" : "-");
    printf( (stats.st_mode & S_IRUSR) ? "r" : "-");
    printf( (stats.st_mode & S_IWUSR) ? "w" : "-");
    printf( (stats.st_mode & S_IXUSR) ? "x" : "-");
    printf( (stats.st_mode & S_IRGRP) ? "r" : "-");
    printf( (stats.st_mode & S_IWGRP) ? "w" : "-");
    printf( (stats.st_mode & S_IXGRP) ? "x" : "-");
    printf( (stats.st_mode & S_IROTH) ? "r" : "-");
    printf( (stats.st_mode & S_IWOTH) ? "w" : "-");
    printf( (stats.st_mode & S_IXOTH) ? "x" : "-"); 
    printf("     ");
    string strxd="meta files of data";
    string ser="group id";
    string userrr="user id";
    pwdd=getpwuid(stats.st_uid);    
    grp=getgrgid(stats.st_gid);
    if(pwdd != 0) printf("\t%-8s", pwdd->pw_name);
	if(grp != 0) printf(" %-8s", grp->gr_name);
	string str123="meta files of data";
    string s123="group id";
    string u123="user id";
    printf("%10.2fK", ((double)stats.st_size) / 1024);

    char *c=ctime(&stats.st_mtime);
    c[strlen(c) - 1] = '\0';
    printf("%30s\n", c);   
}

void ListFiles(string path1)
{
	CLEARLIST
	CLEARFILES
	CLEARDIRECTORIES
	CLEARDIRCHECK
	//cout<<path1;
	
	const char *path=path1.c_str();
	struct dirent *dp;
    DIR *dir = opendir(path);

    if (!dir) 
        return; //if directory doesnt exist then Return
    
    while ((dp = readdir(dir) ) != NULL)
    {
    	
    	string filepath = path1 + dp->d_name;
    	const char *filepth= filepath.c_str();//path is converted from string to constant char.
    	struct stat stats;
    	if (stat(filepth, &stats) == 0)
        {
			if( stats.st_mode & S_IFDIR )
			{
				   //cout<<dp->d_name<<" is a directory path"<<endl;//it's a directory
				   directories.push_back(filepth);
				   //meta(filepth);
			}
			else if( stats.st_mode & S_IFREG )
			{
				    //cout<<dp->d_name<<" is a filepath"<<endl;//it's a file
					//meta(filepth);
					//cout<<dp->d_name<<endl;
					files.push_back(filepth);
			}
		}
	}	
		sort(files.begin(),files.end());
		sort(directories.begin(), directories.end());
		for(int i=0;i<directories.size();i++)
		{
			lists.push_back(directories[i]);
			dircheck.push_back(0);
		}
		for(int i=0;i<files.size();i++)
		{
			lists.push_back(files[i]);
			dircheck.push_back(1);
		}
		closedir(dir);
		/*for(int i=0;i<lists.size();i++)
		{
			cout<<lists[i]<<endl;//.push_back(files[i]);
		} */             
}

void printFiles(int start1, int end1)
{
	CLEARSCREEN
	SETTERMINAL
	SETCURSOR00
	int i;
	for(i=start1;i<=end1;i++)
	{
		meta(lists[i].c_str());
	}
}

void scrolling()
{
	int c;
	int pos=0;
    while (1) 
    {
    	c = kbget();
    		if(c == BACKSPACE)
            {
                string str2="";
                string str=leftstack.top();
                if(str!="/home/")
                {    
                    while(leftstack.empty()!=1)
                    {
                        leftstack.pop();
                    }
                    while(rightstack.empty()!=1)
                    {
                        rightstack.pop();
                    }
                    for(int i=0;i<str.length();i++)
                    {
                        if(str[i]=='/' && i==str.length()-1)
                        {

                        }
                        else if(str[i]=='/' && i<str.length()-1 && i!=0)
                        {
                            str2=str2+str[i];
                            leftstack.push(str2);
                        }
                        else
                        {
                            str2=str2+str[i];
                        }
                    }
                normal_mode(leftstack.top());
                }
            }
    		else if (c == KEY_UP) //for up key movement
            {
                if(pos>start1)
                {
                    pos--;
                    cout<<"\033[1A";
                }     
            } 
            else if (c == int('k'))// for scrolling up in overflow condition
            {
                if(pos==start1 && start1>list_start)
                {
                    pos--;
                    start1--;
                    end1--;
                    printFiles(start1, end1);
                    SETCURSOR00
                    
                }
            }
            else if (c == KEY_DOWN) //for down key movement
            {
                if(pos<end1)
                {
                    //flag=1;
                    pos++;
                    cout<<"\033[1B";
                }
            }
            else if (c == int('l'))//for scrolling down in overflow condition
            {
                if(pos==end1 && end1<list_end)
                {
                    pos++;
                    start1++;
                    end1++;
                    printFiles(start1, end1);
                    cout<<"\033[1A";
                }   
            }	
            else if (c == KEY_ENTER)
            {
                 
            	if(dircheck[pos]==0)
            	{
            			leftstack.push(lists[pos]+'/');
            			normal_mode(leftstack.top());	
					
            	}
            	if(dircheck[pos]==1)
            	{
            		char path[500];
            		strcpy(path,lists[pos].c_str());
            		int fileOpen=open(path,O_WRONLY);
                	dup2(fileOpen,2);
                	close(fileOpen);
                	pid_t processID = fork();
	                if(processID == 0)
	                {
	                    execlp("xdg-open","xdg-open",path,NULL);
	                    exit(0);
	                }
            	}

            }
            else if (c == KEY_LEFT)
            {
                string str1=leftstack.top(); 
                if(leftstack.size()>1)
                {
                    rightstack.push(str1);
                    leftstack.pop();
                    normal_mode(leftstack.top());
                }
            }
            else if (c == KEY_RIGHT)
            {
                if(rightstack.empty()!=1)
                {
                string str1=rightstack.top(); 
                //cout<<str1;
                rightstack.pop();
                leftstack.push(str1);
                normal_mode(leftstack.top());   
                }
            }
            else if(c == HOME)
            {
                while(rightstack.empty()!=1)
                {
                    rightstack.pop();
                }
                leftstack.push("/home/");
                normal_mode(leftstack.top());    
            }
            else if (c==COLON)
            {
            	command_mode();
            	normal_mode(leftstack.top());
            }

    }	
}
void normal_mode(string path)
{
	flag=1;
	ListFiles(path);
	list_start=0;
	list_end=lists.size()-1;
	start1=0;
	if(list_end<WINDOW)
		end1=list_end;
	else
		end1=WINDOW;
	printFiles(start1, end1);
	SETCURSOR00
	scrolling();
}
vector <string> getcmnd(string str1)
{
    vector <string> cmd;
    string strz=str1;
    int len = strz.length();
    string str2="";
    int i=0;
    while(i<len)
    {
        if(strz[i] == ' ')
        {
            cmd.push_back(str2);
            str2="";
            i++;
        }

        if(strz[i] == '~')
        {
            str2=str2+"/home";
        }
        else
        {
            str2=str2+strz[i];
        }
        i++;
    }
    cmd.push_back(str2);
    return cmd;
}

int check(string str1)
{
    if(str1=="create_file")
        return 1;
    else if(str1=="search")
        return 2;
    else if(str1=="goto")
        return 3;
    else if(str1=="delete_file")
        return 4;
    else if(str1=="copy")
        return 5;
    else if(str1=="move")
        return 6;
    else if(str1=="rename")
        return 7;
    else if(str1=="delete_dir")
    	return 8;
    else
        return 0;
}
void initialize()
{
	cout<<"\033[24;0H";
	cout<<"COMMAND MODE";
	cout<<"\033[25;0H";
}

void search(string path1, string srch)
{
    dirent *dirPtr;
    struct dirent *dp;
    const char *path=path1.c_str();
    DIR *dir = opendir(path);

    if (!dir) 
        return; //if directory doesnt exist then Return
    
    while ((dp = readdir(dir) ) != NULL)
    {
        
        string filepath = path1 + dp->d_name;
        const char *filepth=filepath.c_str();//path is converted from string to constant char.
        struct stat stats;
        if (stat(filepth, &stats) == 0)
        {
            if( stats.st_mode & S_IFDIR )
            {
                string str2=dp->d_name;
                    if(str2==srch)
                    {
                        cout<<"    YES\n";
                    }
                   if(str2!="." && str2!="..")
                   {
                   string str1=filepath+'/';
                    search(str1, srch);
                    }

            }
            else if( stats.st_mode & S_IFREG )
            {
                    if(srch==dp->d_name)
                    cout<<"    YES\n";
            }
        }
    }
    closedir(dir);
    return;
   
}

void create_file(string src)
{
	char source[100];
    strcpy(source, src.c_str());
    int fd1 = open(source, O_CREAT, 0);
    chmod(source,S_IRUSR|S_IWUSR);
    close(fd1);
}
void delete_dir(string dirPath)
{
    const char *mdpath = dirPath.c_str();
    char path[500];
    dirent *directoryPointer;
    DIR *currentdirectory= opendir(mdpath);
    if (!currentdirectory)  
        return;
    while ((directoryPointer = readdir(currentdirectory)) != NULL)
    {
        string hg="directory to be deleted";
        if (strcmp(directoryPointer->d_name, ".") != 0 && strcmp(directoryPointer->d_name, "..") != 0)
        {
            strcpy(path, mdpath);
            strcat(path, "/");
            strcat(path, directoryPointer->d_name);
            delete_dir(path);
            if(directoryPointer->d_type==DT_REG)
            {
                if(remove(path))
                cout<<"could not delete "<<path<<endl;
            }
        }
    }
    remove(mdpath);
    closedir(currentdirectory);
}
void delete_file(string path)
{
    char Path[100];
        strcpy(Path, path.c_str());
    int fd=remove(Path);
        if(fd==-1){
            cout<<"error in deleting file "<<endl;
            exit(0);
        }
}

void copy_file(string src, string dest)
{
    char source[100],destn[100];
    strcpy(destn, dest.c_str());
    strcpy(source, src.c_str());
    //cout<<endl<<src<<endl;
    //cout<<dest<<endl;
    int fd1 = open(source, O_RDONLY, 0);
    int fd2 = open(destn, O_WRONLY| O_CREAT, 0);
    char c;
    //cout<<endl;
    while(read(fd1, &c, 1) != 0){
        write(fd2, &c, 1);
    }
    close(fd1);
    close(fd2);
    struct stat stats;
    if (stat(source, &stats) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

}

void goto_path(string str1)
{
	while(rightstack.empty()!=1)
	{
		rightstack.pop();
	}
    leftstack.push(str1);
    normal_mode(leftstack.top());
}

void command_mode()
{
	flag=0;
	initialize();
	cout<<str1;
    int flag=0,i,len;
    while(flag==0)
    {
    int c=kbget();
        if(c==KEY_ESCAPE)
        {
            break;
        }
        if(c==BACKSPACE)
        {
            if(str1.empty()==0)
            {
            str1.pop_back();
            printFiles(start1, end1);
            command_mode();
        	}
        }
        else if(c==KEY_ENTER)
        {
        	
        	//printFiles(start1, end1);
        	//command_mode();
        	string srch="",path1="";
        	vector <string> cmnd=getcmnd(str1);
        	str1="";
        	int cases=check(cmnd[0]);
        	switch(cases) 
                {
                case 1:
                	 //cout<<cmnd[2]+cmnd[1];
                     create_file(cmnd[2]+'/'+cmnd[1]);
                break;
        		case 2:
                     srch=cmnd[1];
                     path1=leftstack.top();
                     search(path1,srch);
                     
                break;
                case 3:
                     goto_path(cmnd[1]+'/');
                break;
                case 4:
                		//delete_file
                		//cout<<"YES";
                		len=cmnd.size();
                       	srch=cmnd[len-1];
                       	//cout<<srch<<endl;
                       	delete_file(srch);
                       
                break;
                case 5:
                	//copy_file
                		i=0;
                    len=cmnd.size();
                    for(i=1;i<len-1;i++)
                    {
                        str1=leftstack.top()+cmnd[i];
                        str2=cmnd[len-1]+'/'+cmnd[i];
                        copy_file(str1,str2);
                    }
                break;
                case 6:
                	//move
              		 i=0;
                    len=cmnd.size();
                    for(i=1;i<len-1;i++)
                    {
                    str1=leftstack.top()+cmnd[i];
                    str2=cmnd[len-1]+'/'+cmnd[i];
                    //cout<<str1<<" "<<str2;
                    copy_file(str1,str2);
                    delete_file(str1);
                    }
                   
                break;
                case 7:
              		str1=leftstack.top()+cmnd[1];
                    str2=leftstack.top()+cmnd[2];
                    //cout<<str1;
                    //cout<<str2;
                    copy_file(str1,str2);
                    delete_file(str1);
                break;
                case 8:
                	//cout<<cmnd[1];
                	delete_dir(cmnd[1]);
       		 	}
       	}
        else
        {
        cout<<char(c);
        str1=str1+char(c);
        }
    }
}
int main()
{
	string cur_path=get_current_dir();
	cur_path.push_back('/');	
	leftstack.push(cur_path);
	//cout<<leftstack.top();
	normal_mode(leftstack.top());
}

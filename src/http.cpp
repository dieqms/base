#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <string>
#include <base.h>

struct resp_header//保持相应头信息
{
    int status_code;//HTTP/1.1 '200' OK
    char content_type[128];//Content-Type: application/gzip
    long content_length;//Content-Length: 11683079
    char file_name[256];
};

struct resp_header resp;//全剧变量以便在多个进程中使用

//void parse_url(const char *url, char *domain, int *port, char *file_name)
void parse_url(const char *url, char *domain, int *port)
{
    /*通过url解析出域名, 端口, 以及文件名*/
    int j = 0,i = 0;
    int start = 0;
    *port = 80;
    const char *patterns[] = {"http://", "https://", NULL};

    for (i = 0; patterns[i]; i++)
        if (strncmp(url, patterns[i], strlen(patterns[i])) == 0)
            start = strlen(patterns[i]);

    //解析域名, 这里处理时域名后面的端口号会保留
    for (i = start; url[i] != '/' && url[i] != '\0'; i++, j++)
        domain[j] = url[i];
    domain[j] = '\0';

    //解析端口号, 如果没有, 那么设置端口为80
    char *pos = strstr(domain, ":");
    if (pos)
        sscanf(pos, ":%d", port);

    //删除域名端口号

    for (i = 0; i < (int)strlen(domain); i++)
    {
        if (domain[i] == ':')
        {
            domain[i] = '\0';
            break;
        }
    }

    //获取下载文件名
/*    j = 0;
    for (i = start; url[i] != '\0'; i++)
    {
        if (url[i] == '/')
        {
            if (i !=  strlen(url) - 1)
                j = 0;
            continue;
        }
        else
            file_name[j++] = url[i];
    }
    file_name[j] = '\0';
*/
}

struct resp_header get_resp_header(const char *response)
{
    /*获取响应头的信息*/
//    struct resp_header resp;

    const char *pos = strstr(response, "HTTP/");
    if (pos)
        sscanf(pos, "%*s %d", &resp.status_code);//返回状态码

    pos = strstr(response, "Content-Type:");//返回内容类型
    if (pos)
        sscanf(pos, "%*s %s", resp.content_type);

    pos = strstr(response, "Content-Length:");//内容的长度(字节)
    if (pos)
        sscanf(pos, "%*s %ld", &resp.content_length);

    return resp;
}

void get_ip_addr(char *domain, char *ip_addr)
{
    int i = 0;
    /*通过域名得到相应的ip地址*/
    struct hostent *host = gethostbyname(domain);// 这个函数的传入值是域名或者主机名，例如"www.google.cn"等等。传出值，是一个hostent的结构。如果函数调用失败，将返回NULL。
    if (!host)
    {
        ip_addr = NULL;
        return;
    }

    for (i = 0; host->h_addr_list[i]; i++)
    {
        strcpy(ip_addr, inet_ntoa( * (struct in_addr*) host->h_addr_list[i]));
        break;
    }
}


void progressBar(long cur_size, long total_size)
{
    /*用于显示下载进度条*/
    float percent = (float) cur_size / total_size;
    const int numTotal = 50;
    int numShow = (int)(numTotal * percent);

    if (numShow == 0)
        numShow = 1;

    if (numShow > numTotal)
        numShow = numTotal;

    char sign[51] = {0};
    memset(sign, '=', numTotal);

    PRINT_DEBUG("\r%.2f%%\t[%-*.*s] %.2f/%.2fMB\n", percent * 100, numTotal, numShow, sign, cur_size / 1024.0 / 1024.0, total_size / 1024.0 / 1024.0);
    fflush(stdout);

    if (numShow == numTotal)
        PRINT_DEBUG("\n");
}

int download(int client_socket)
{
    /*下载文件函数*/
    int length = 0;
    int mem_size = 4096;//mem_size might be enlarge, so reset it
    int buf_len = mem_size;//read 4k each time
    int len;

    //创建文件描述符
    int fd = open(resp.file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXG | S_IRWXO | S_IRWXU);
    if (fd < 0)
    {
        PRINT_DEBUG("http: Create file failed\n");
        return -1;
    }

    char *buf = (char *) malloc(mem_size * sizeof(char));

    //从套接字中读取文件流
    while ((len = read(client_socket, buf, buf_len)) != 0 && length < resp.content_length)
    {
        write(fd, buf, len);
        length += len;
        progressBar(length, resp.content_length);
    }

    if (length == resp.content_length){
    	PRINT_DEBUG("http: length == resp.content_length %d\n", length);
    	return 0;
    }
    else {
    	PRINT_DEBUG("http: length != resp.content_length\n");
    	return -1;
    }
}

bool Md5Check(char *file_name, unsigned char *md5) {
	//md5 check
	std::string file = Base::FileUtil::ReadAll(file_name);
	Base::Md5Util file_md5;
	file_md5.GenerateMD5((uint8_t*)file.c_str(), (size_t)file.size());
	PRINT_DEBUG("File: %s\n", file_name);
	PRINT_DEBUG("MD5: %s\n", file_md5.ToString().c_str());

	//校验MD5
	std::string compare_md5_str;
	compare_md5_str.append((const char*)md5, (string::size_type)32);
	if (file_md5.ToString() == compare_md5_str) {
		return true;
	}
	else {
		PRINT_WARN("Update file md5 not match: %s vs %s\n", compare_md5_str.c_str(),
				file_md5.ToString().c_str());
		return false;
	}
}

//int main(int argc, char const *argv[])
int HttpDownloadFile(const char *url, char *file_name, unsigned char *md5 = NULL)
{
    /*
        test url:
        1. https://nodejs.org/dist/v4.2.3/node-v4.2.3-linux-x64.tar.gz
        2. http://img.ivsky.com/img/tupian/pre/201312/04/nelumbo_nucifera-009.jpg
    */
//    char url[2048] = "127.0.0.1";
    char domain[64] = {0};//域名
    char ip_addr[16] = {0};//ip地址
    int port = 80;//端口
    int ret;
//    char file_name[256] = {0};//文件名

    PRINT_DEBUG("http: 1: Parsing url...\n");
    parse_url(url, domain, &port);//获取域名，端口

    PRINT_DEBUG("http: 2: Get ip address...\n");
    get_ip_addr(domain, ip_addr);//获取ip地址
    if (strlen(ip_addr) == 0)
    {
        PRINT_WARN("can not get ip address\n");
        return -1;
    }

    PRINT_DEBUG("\n>>>>Detail<<<<\n");
    PRINT_DEBUG("URL: %s\n", url);
    PRINT_DEBUG("DOMAIN: %s\n", domain);
    PRINT_DEBUG("IP: %s\n", ip_addr);
    PRINT_DEBUG("PORT: %d\n", port);
    PRINT_DEBUG("FILENAME: %s\n\n", file_name);

    //设置http请求头信息
    char header[2048] = {0};
    sprintf(header, \
            "GET %s HTTP/1.1\r\n"\
            "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
            "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
            "Host:%s\r\n"\
            "Connection:close\r\n"\
            "\r\n"\
        ,url, domain);

    //PRINT_DEBUG("%s\n%d", header, (int) strlen(header));

    //创建套接字
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0)
    {
    	PRINT_WARN("http: invalid socket descriptor: %d\n", client_socket);
        return (-1);
    }

    //创建地址结构体
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    addr.sin_port = htons(port);

    //连接服务器
    PRINT_DEBUG("http: 3: Connect server...\n");
    int res = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
    if (res == -1)
    {
    	PRINT_WARN("http: connect failed, return: %d\n", res);
        return (-1);
    }

    PRINT_DEBUG("http: 4: Send request...\n");//向服务器发送下载请求
    write(client_socket, header, strlen(header));

    int mem_size = 4096;
    int length = 0;
    int len;
    int i = 0;
    char *buf = (char *) malloc(mem_size * sizeof(char));
    char *response = (char *) malloc(mem_size * sizeof(char));

    //每次单个字符读取响应头信息, 仅仅读取的是响应部分的头部, 后面单独开线程下载
    while ((len = read(client_socket, buf, 1)) != 0)
    {
        if (length + len > mem_size)
        {
            //动态内存申请, 因为无法确定响应头内容长度
            mem_size *= 2;
            char * temp = (char *) realloc(response, sizeof(char) * mem_size);
            if (temp == NULL)
            {
            	PRINT_WARN("http: realloc failed\n");
                return (-1);
            }
            response = temp;
        }

        buf[len] = '\0';
        strcat(response, buf);

        //找到响应头的头部信息, 两个"\n\r"为分割点
        int flag = 0;
        for (i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++);
        	if (flag == 4)
            		break;

        length += len;
    }

    resp = get_resp_header(response);
    strcpy(resp.file_name, file_name);

    if (resp.status_code != 200) {
    	PRINT_WARN("Download fail:\n");
    	PRINT_WARN(">>>>Response header:<<<<\n%s", response);
		return -1;
    }

    PRINT_DEBUG("http: 5: Start to download...\n");
    ret = download(client_socket);
    if (ret != 0)
    	return ret;
    
    if (md5) {
    	if (!Md5Check(file_name, md5)) {
    		PRINT_WARN("Download fail: md5 not match!\n");
    		Base::FileUtil::Delete(file_name);
			return -1;
    	}
    }
    
    return 0;
}

#ifdef HTTP_TEST
int main()
{
	char * url = "http://192.168.10.23:8080/busGateway/lineFile/2017/08/14/K19.txt";
	char * save = "./testfile.txt";

	//if(HttpDownloadFile("http://117.34.118.31:10000/software/hash.exe","/opt/myapp/data/hash.exe") == true){
	if(HttpDownloadFile(url,save) == true){
		PRINT_DEBUG("Download successful ^_^\n\n");
	}else{
		PRINT_DEBUG("Download error\n\n");
	}

	return 0;
}
#endif

#include "pch.h"
#include "http_socket.h"
#include "errors.h"
#include "converter.h"
#include "system.h"
#include <map>
#include <iostream>
#include <fstream>

namespace http_client {
unsigned http_socket::m_connect_timeout = 30000;
unsigned http_socket::m_response_timeout = 30000;
const unsigned  http_socket::m_buffLen = 0xfff;

http_socket::http_socket(const std::wstring& ipport,
                         unsigned connectRetryCount,
                         unsigned connectRetryTimeout):
    m_host(),
    m_port(80),
    m_ip(),
    m_connectRetryCount(connectRetryCount),
    m_connectRetryTimeout(connectRetryTimeout),
    m_socket(INVALID_SOCKET),
    m_lastMsg(),
    m_lastStatusCode(0),
    m_buff(new char[m_buffLen + 1]),
    m_bytesInBuff(0),
    m_dataLen(0),
    m_currentDataLen(0),
    m_errStr(),
    m_query(),
    m_header()
{
    std::wstring::size_type n = ipport.find(L':');
    if(std::string::npos == n) {
        utils::utf16_to_ascii(ipport.c_str(), m_host);
    } else {
        utils::utf16_to_ascii(ipport.substr(0, n).c_str(), m_host);
        if((n+1) < ipport.size()) {
            bool err = false;
            m_port = (unsigned short)utils::str2int(utils::utf16_to_ascii(ipport.substr(n + 1).c_str()), err); 
            if(err) {
                m_port = 80;
            }
        }
    }
}

http_socket::~http_socket()
{
    delete[] m_buff;
    if(INVALID_SOCKET != m_socket) {
        closesocket(m_socket);
    }
}

bool http_socket::send(const std::string& url, const std::string& range)
{
    m_lastMsg = "";
    m_header = "";
    m_query = "";
    m_lastStatusCode = 0xffffffff;
    m_bytesInBuff = 0;
    m_dataLen = 0;
    m_currentDataLen = 0;
    m_error = 0;
    if(m_error) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    m_error = 0;
    m_errStr = "";
    m_query = ("GET ");
    m_query += "/";
    m_query += url;
    m_query += " HTTP/1.1\r\n"
               "Host: ";
    m_query += m_host;
    if(80 != m_port) {
        m_query += ":";
        m_query += utils::int2str(m_port);
    }
    m_query += "\r\n";
    m_query += range;
    m_query += "Accept: */*\r\n"
               "Accept-Charset: ISO-8859-1,utf-8\r\n"
               "Pragma: no-cache\r\n"
               "Cache-Control: no-cache\r\n";
    m_query += "Connection: keep-alive\r\n"
               "\r\n";
    if(INVALID_SOCKET == m_socket) {
        if(connect()) {
            return true;
        }
    }
    int l = ::send(m_socket, m_query.c_str(), (int)(m_query.size()), 0);
    bool err = false;
    if(l != (int)(m_query.size())) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        err = true;
    }
    return err;
}

bool http_socket::pars_html_errors(const std::string& header)
{
    if(header.size() < 6) return false;
    if(_stricmp(header.substr(0, 6).c_str(), "<html>")) return false;
    std::string::size_type n = 0, m = 0;
    while(m < header.size()) {
        n = header.find("</", m);
        if(n == std::string::npos) return false;
        if(((n + 7)<header.size()) &&
           !_stricmp(header.substr(n, 7).c_str(),"</html>")) {
            m_lastMsg = header.substr(0, n+7);
            m_lastStatusCode = 123456;
            m_error = INN_UPDATER_HTTP_ERROR;
            return true;
        }
        m = n + 1;
    }
    return false;
}

struct ignorCaseCmp
{
    bool operator() (const std::string& s1, const std::string& s2) const 
    {
        return 0 > _stricmp(s1.c_str(), s2.c_str());
    }
};

void http_socket::pars_header()
{
    std::vector<std::string> v, l;
    std::string::size_type i =0, j = 0;
    while(j < m_header.size()) {
        i = m_header.find("\r\n", j);
        if(i != std::string::npos) {
            v.push_back(m_header.substr(j, i - j));
            j = i + 2;
        } else {
            v.push_back(m_header.substr(j));
            break;
        }
    }
    std::map<std::string, std::string, ignorCaseCmp> m;
    for(size_t k=0; k<v.size(); ++k) {
        i = v[k].find(":");
        if(i != std::string::npos) {
            m.insert(std::make_pair(utils::trim(v[k].substr(0, i)), utils::trim(v[k].substr(i+1))));
        } else {
            l.push_back(v[k].c_str());
        }
    }
    v.clear();
    for(size_t n=0; n<l.size(); ++n) {
        utils::to_words(l[n], v);
        if(v.size() > 1 && 
           v[0].size() > 4 &&
           !_stricmp(v[0].substr(0, 4).c_str(), "http")) {
            bool err = false;
            unsigned st = (unsigned)utils::str2int(v[1], err);
            if(!err) {
                m_lastStatusCode = st;
            }
            for(size_t x = 2; x<v.size(); ++x) {
                m_lastMsg += v[x];
                if((x+1) < v.size()) {
                m_lastMsg += " ";
                }
            }
        }
    }
    std::map<std::string, std::string, ignorCaseCmp>::iterator 
    b=m.begin(), e = m.end();
    bool err = false;
    unsigned __int64 dataLen = utils::str2int(m["Content-Length"], err);
    if(!err) {
        m_dataLen = dataLen;
    }
}

unsigned __int64 http_socket::get_conntent_size() const
{
    return m_dataLen;
}

void http_socket::recv_header(std::vector<char>& fileBuff)
{
    fileBuff.clear();
    std::string header;
    unsigned recvTimeout = 0;
    while(recvTimeout < m_response_timeout) {
        if(header.size() > 2000) {
            break; //bad header
        }
        fd_set socks;
        FD_ZERO(&socks);
        FD_SET(m_socket, &socks);
        timeval tm;
        tm.tv_sec = 1;
        tm.tv_usec = 0;
        int r = ::select(0, &socks, 0, 0, &tm);
        if((1 == r) && FD_ISSET(m_socket, &socks)) {
            int len = ::recv(m_socket, m_buff, m_buffLen, 0);
            if(len > 0) {
                header.append(m_buff, m_buff + len);
            } else {
                closesocket(m_socket);
                m_socket = INVALID_SOCKET;
                return;
            }
        } else if(0 == r) {
            recvTimeout += 1000;
        }
        if(pars_html_errors(header)) {
            return;
        }
        std::string::size_type n = header.find("\r\n\r\n");
        if(n != std::string::npos) {
            fileBuff.insert(fileBuff.end(), header.begin() + n + 4, header.end());
            m_header = header.substr(0, n);
            pars_header();
            break;
        }
    }
}

void http_socket::recv_file(unsigned __int64 size,
                            const std::wstring& fname,
                            const std::vector<char>& fileBuff)
{
	FILE* out = _wfopen(fname.c_str(), L"ab");
    //std::ofstream out(fname.c_str(), std::ios::binary | std::ios::app);
    if(!out) { //.is_open()) {
        m_error = INN_UPDATER_FAILED_FILE_OPEN;
        return;
    }
	try {
		if(!fileBuff.empty()) {
			//out.write(&(fileBuff[0]), fileBuff.size());
			fwrite(&(fileBuff[0]), 1, fileBuff.size(), out);
			m_currentDataLen += fileBuff.size();
		}
		unsigned recvTimeout = 0;
		bool done = false;
		while(!done && (recvTimeout < m_response_timeout)) {
			fd_set socks;
			FD_ZERO(&socks);
			FD_SET(m_socket, &socks);
			timeval tm;
			tm.tv_sec = 1;
			tm.tv_usec = 0;
			int len = 0;
			int r = ::select(0, &socks, 0, 0, &tm);
			if((1 == r) && FD_ISSET(m_socket, &socks)) {
				recvTimeout = 0;
				len = ::recv(m_socket, m_buff, m_buffLen, 0);
				if(len > 0) {
					//out.write(m_buff, len);
					fwrite(m_buff, 1, len, out);
					m_currentDataLen += len;
					if((m_currentDataLen >= m_dataLen) && m_dataLen) {
						done = true;
					}
				} else {
					if(len) {
						closesocket(m_socket);
						m_socket = INVALID_SOCKET;
						m_error = INN_UPDATER_RECV_ERROR;
					} else {
						m_error = INN_UPDATER_HTTP_SERVER_CLOSED;
						m_errStr = "server closed after recving ";
						m_errStr += utils::int2str(m_currentDataLen);
						m_errStr += " bytes";
						if(m_currentDataLen < m_dataLen) {
							m_errStr += ", http query = ";
							m_errStr += m_query;
							m_errStr += ", http header = ";
							m_errStr += m_header;
						}
					}
					len = 0;
					return;
				}
			} else if (0 == r) {
				recvTimeout += 1000;
				//out.flush();
				fflush(out);
				if(m_currentDataLen >= m_dataLen) {
					done = true;
				}
			} else {
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				m_error = INN_UPDATER_SELECT_ERROR;
				m_errStr = "function select returned error: ";
				m_errStr += utils::int2str(WSAGetLastError());
				break;
			}
		}
		if(recvTimeout >= m_response_timeout) {
			m_errStr = "recv timeouted... ";
			m_error = INN_UPDATER_RECV_TIMEOUTED_ERROR;
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
	} catch (...) {
	}
	fclose(out);
}

int http_socket::recv(unsigned __int64 size,
                        const std::wstring& fName,
                        unsigned __int64* actualSize)
{
    if(!m_error) {
        std::vector<char> fileBuff;
        recv_header(fileBuff);
        if(!m_error &&
           (INVALID_SOCKET != m_socket) &&
           (m_lastStatusCode == 200 || m_lastStatusCode == 206)) {
           recv_file(size, fName, fileBuff);
           if(actualSize) *actualSize += m_currentDataLen;
        } else if(!m_error) {
            if(m_lastStatusCode == 404) {
                m_error = INN_UPDATER_HTTP_ERROR_404;
            } else {
                m_error = INN_UPDATER_HTTP_ERROR;
            }
        }
    }
    return m_error; 
}

bool http_socket::connect()
{
    m_error = 0;
    if(m_socket != INVALID_SOCKET) {
        return false;
    }
    int iMode = 1;
    m_socket  = INVALID_SOCKET;
    for(unsigned i=0; i<m_connectRetryCount; ++i) {
        if(!utils::get_host_ip(m_host, m_ip)) {
            continue;
        }
        m_socket  = INVALID_SOCKET;
        sockaddr_in clnt;
        clnt.sin_family = AF_INET;
        clnt.sin_addr.s_addr = ::inet_addr(m_ip.c_str());
        clnt.sin_port = htons(m_port);
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(INVALID_SOCKET != m_socket) {
            ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode);
            int err = ::connect(m_socket, (sockaddr*)&clnt, sizeof(clnt));
            if(0 != err){
                if(WSAEWOULDBLOCK != WSAGetLastError()) {
                    closesocket(m_socket);
                    m_socket = INVALID_SOCKET;
                    if((i + 1) < m_connectRetryCount) {
                        Sleep(m_connectRetryTimeout);
                    } else {
                        m_error = INN_UPDATER_CONNECT_ERROR;
                        return true;
                    }
                } else {
                    unsigned connTimeout = 0;
                    while(connTimeout < m_connect_timeout) {
                        fd_set socks;
                        FD_ZERO(&socks);
                        FD_SET(m_socket, &socks);
                        timeval tm;
                        tm.tv_sec = 1;
                        tm.tv_usec = 0;
                        int r = ::select(0, 0, &socks, 0, &tm);
                        if((1 == r) && FD_ISSET(m_socket, &socks)) {
                            break;
                        }
                        connTimeout += 1000;
                    }
                    if(connTimeout < m_connect_timeout) {
                        break;
                    }
                    closesocket(m_socket);
                    m_socket = INVALID_SOCKET;
                    if((i + 1) < m_connectRetryCount) {
                        Sleep(m_connectRetryTimeout);
                    } else {
                        m_error = INN_UPDATER_CONNECT_ERROR;
                        return true;
                    }
                }
            } else {
                break;
            }
        } else {
            m_error = INN_UPDATER_CREATE_SOCKET_ERROR;
            return true;
        }
    }
    if(INVALID_SOCKET == m_socket) {
        if(m_ip.empty()) {
            m_error = INN_UPDATER_RESOLVE_HOST_ERROR;
        } else {
            m_error = INN_UPDATER_CONNECT_ERROR;
        }
        return true;
    }
    iMode = 0;
    ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode);
    return false;
}

unsigned __int64 http_socket::get_current_downloaded_size() const
{
    return m_currentDataLen;
}

std::string http_socket::get_errors()
{
    if(!m_errStr.empty()) {
        return m_lastMsg + "/" + m_errStr + "/" + utils::int2str(m_lastStatusCode);
    }
    return m_lastMsg + "/" + utils::int2str(m_lastStatusCode);
}

}

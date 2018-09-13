/**
 *  _  _ ____ _____
 * | \| | ___|_   _|
 * |    | ___| | |
 * |_|\_|____| |_|
 */
#ifndef OYOUNG_NET_HPP
#define OYOUNG_NET_HPP

#include <oyoung/any.hpp>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <csignal>
#include <cstddef>
#include <exception>
#include <stdexcept>

#if defined(__unix__) ||defined(__unix) || defined(__GNUC__) || defined(__GNUG__)

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>


#endif



namespace oyoung {

namespace net {

using Int8 = std::int8_t;
using Int16 = std::int16_t;
using Int32 = std::int32_t;
using Int64 = std::int64_t;

using UInt8 = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;

using Byte = UInt8;
using Bytes = std::vector<Byte>;
using String = std::string;

struct socket
{
    constexpr static int bad_descriptor = -1;
public:
    socket(const std::string& address, Int32 port)
        : _address(address)
        , _port(port)
        , _descriptor(bad_descriptor)
    {

    }

    const std::string& address() const
    {
        return _address;
    }

    Int32 port() const
    {
        return _port;
    }

    Int32 descriptor() const
    {
        return _descriptor;
    }

    void set_descriptor(Int32 descriptor)
    {
        _descriptor = descriptor;
    }

    virtual bool good() const noexcept = 0;

protected:

    const std::string _address;
    Int32 _descriptor, _port;

};

enum class socket_error
{
    query_failed,
    connection_closed,
    connection_timeout,
    unix_system_error,
    unknown_error
};

struct result
{
    bool success() const
    {
        return _success;
    }

    socket_error error() const
    {
        if(_error) return *_error;
        throw std::runtime_error("error is null");
    }

    result(): _success(true) {}

    explicit result(const socket_error& err, const String& message = "")
        : _success(false), _message(message)
        , _error(std::make_shared<socket_error>(err))
    {

    }

    explicit result(std::size_t n, UInt8 repeat = 0)
        : _success(true), _data_bytes(n, repeat)
    {

    }

    operator bool() const
    {
        return success();
    }

    const String message() const
    {
        return _message;
    }


    Bytes& bytes()
    {
        return _data_bytes;
    }

    const Bytes& bytes() const
    {
        return _data_bytes;
    }

    UInt8& operator[](std::size_t index)
    {
        return _data_bytes[index];
    }
    const UInt8& operator [](std::size_t index) const
    {
        return _data_bytes[index];
    }

    const UInt8 *data() const
    {
        return _data_bytes.data();
    }

    UInt8 *data()
    {
        return _data_bytes.data();
    }

    Bytes::iterator begin()
    {
        return _data_bytes.begin();
    }

    Bytes::const_iterator begin() const
    {
        return _data_bytes.begin();
    }

    Bytes::iterator end()
    {
        return _data_bytes.end();
    }

    Bytes::const_iterator end() const
    {
        return _data_bytes.end();
    }

    template<typename It>
    void assign(It begin, It end)
    {
        _data_bytes.assign(begin, end);
    }

    void resize(std::size_t size)
    {
        _data_bytes.resize(size);
    }

    void reserve(std::size_t capacity)
    {
        _data_bytes.reserve(capacity);
    }

    bool empty() const
    {
        return _data_bytes.empty();
    }

    void clear()
    {
        _data_bytes.clear();
    }

private:
    bool _success;
    Bytes _data_bytes;
    String _message;
    std::shared_ptr<socket_error> _error;


};

namespace tcp {
    template<typename T>
    struct client: public socket
    {

        client(const std::string& address, Int32 port): socket(address, port) {}

        result connect(int timeout = -1)
        {
            Int32 descriptor = _ctrl.connect(_address, _port, timeout);
            if(descriptor > 0) {
                _descriptor = descriptor;
                return result {};
            } else {
                switch (descriptor) {
                case -1:
                    return result(socket_error::query_failed, "query failed");
                case -2:
                    return result(socket_error::connection_closed, "connection closed");
                case -3:
                    return result(socket_error::connection_timeout, "connnection timeout");
                default:
                    return result(socket_error::unix_system_error, _ctrl.to_string(descriptor));
                }
            }
        }

        bool good() const noexcept override
        {
            return _ctrl.good(_descriptor);
        }

        void close() {
            if(_descriptor == bad_descriptor) return;
            _ctrl.close(_descriptor);
            _descriptor = bad_descriptor;
        }

        void reset() {
            _descriptor = bad_descriptor;
        }

        result send(const Bytes& data)
        {
            if(_descriptor == bad_descriptor) return result(socket_error::connection_closed);

            Int32 sendsize = _ctrl.send(_descriptor, data.data(), data.size());

            return sendsize == data.size() ? result{} : result(socket_error::unknown_error);
        }

        result send(const String& string)
        {
            if(_descriptor == bad_descriptor) return result(socket_error::connection_closed);

            Int32 sendsize = _ctrl.send(_descriptor, string.data(), string.size());

            return sendsize == string.size() ? result{} : result(socket_error::unknown_error);
        }

        result send(const void *data, UInt32 size)
        {
            if(_descriptor == bad_descriptor) return result(socket_error::connection_closed);

            Int32 sendsize = _ctrl.send(_descriptor, data, size);

            return sendsize == size ? result{} : result(socket_error::unknown_error);
        }


        result read(Int32 expected , Int32 timeout = -1)
        {
            if(_descriptor == bad_descriptor) return result(socket_error::connection_closed);

            result r(expected, 0);

            Int32 readsize = _ctrl.pull(_descriptor, r.data(), expected, timeout);

            if(readsize == 0) {
                return result(socket_error::connection_closed, "connection closed");
            }

            if(readsize < 0) {
                return result(socket_error::unix_system_error, _ctrl.to_string(errno));
            }
            r.resize(readsize);

            return r;
        }

        Int32 bytes_available() const
        {
            if(_descriptor == bad_descriptor) return -1;

            return _ctrl.bytes_available(_descriptor);
        }

    private:
        T _ctrl;
    };

    template<typename T>
    struct server: public socket
    {
        server(const std::string& address, Int32 port): socket(address, port) {}

        result listen()
        {
            Int32 descriptor = _ctrl.listen(_address.c_str(), _port);

            if(descriptor > 0) {
               _descriptor = descriptor;

               if(_port == 0) {
                   Int32 port = _ctrl.port(_descriptor);
                   if(port < 0) {
                       return result(socket_error::unknown_error);
                   }

                   _port = port;
               }
               return result {};
            }

            return result(socket_error::unknown_error);
        }

        std::shared_ptr<client<T>> accept(Int32 timeout = 0)
        {
            Int32 port = 0;
            String ip(16, '\0');
            if(_descriptor == bad_descriptor) return nullptr;

            Int32 client_descriptor = _ctrl.accept(_descriptor, ip, port, timeout);

            if(client_descriptor >= 0) {

                auto tcpclient = std::make_shared<client<T>>(ip, port);

                tcpclient->set_descriptor(client_descriptor);

                return tcpclient;
            }

            return nullptr;
        }

        bool good() const noexcept override
        {
            return _ctrl.good(_descriptor);
        }

        void close()
        {
            if(_descriptor == bad_descriptor) return;

            _ctrl.close(_descriptor);

            _descriptor = bad_descriptor;
        }


    private:
        T _ctrl;
    };
#if defined(__unix__) ||defined(__unix) || defined(__GNUC__) || defined(__GNUG__)
    struct unix_tcp_socket_ctrl
    {
        void set_block(Int32 descriptor, Int32 on) const
        {
            int flags = fcntl(descriptor, F_GETFL, 0);
            if (on == 0) {
                fcntl(descriptor, F_SETFL, flags | O_NONBLOCK);
            } else {
                flags &= ~ O_NONBLOCK;
                fcntl(descriptor, F_SETFL, flags);
            }
        }

        Int32 connect(const char *host, int port, int timeout) const {

            struct sockaddr_in sa;
            struct hostent *hp;

            int sockfd = -1;

            hp = gethostbyname(host);
            if (hp == nullptr) {
                return -1;
            }

            bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
            sa.sin_family = hp->h_addrtype;
            sa.sin_port = htons(port);
            sockfd = ::socket(hp->h_addrtype, SOCK_STREAM, 0);

            set_block(sockfd, 0);
            ::connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));
            fd_set fdwrite;
            struct timeval  tvSelect;
            FD_ZERO(&fdwrite);
            FD_SET(sockfd, &fdwrite);
            tvSelect.tv_sec = timeout;
            tvSelect.tv_usec = 0;

            int retval = ::select(sockfd + 1, NULL, &fdwrite, NULL, &tvSelect);
            if (retval < 0) {
                ::close(sockfd);
                return -2;
            } else if(retval == 0) {//timeout
                ::close(sockfd);
                return -3;
            } else {
                int error = 0;
                int errlen = sizeof(error);
                ::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&errlen);
                if (error != 0) {
                    ::close(sockfd);
                    return error;//connect fail
                }


                return sockfd;
            }
        }

        String to_string(int error)
        {
            return strerror(error);
        }

        Int32 close(Int32 socketfd) const {
            return ::close(socketfd);
        }

        Int32 pull(int socketfd, void *data, int len, int timeout_msec) const {
            int readlen = 0;
            int datalen = 0;
            if (timeout_msec > 0) {
                fd_set fdset;
                struct timeval timeout;
                timeout.tv_sec = timeout_msec / 1000 ;
                timeout.tv_usec = timeout_msec % 1000 * 1000UL;
                FD_ZERO(&fdset);
                FD_SET(socketfd, &fdset);
                int ret = ::select(socketfd + 1, &fdset, NULL, NULL, &timeout);
                if (ret <= 0) {
                    return ret; // select-call failed or timeout occurred (before anything was sent)
                }
            }
            // use loop to make sure receive all data
            readlen = (int)::recv(socketfd, (char *)data + datalen, len - datalen, 0);

            if(readlen <= 0) return readlen;

            while (readlen > 0) {
                datalen += readlen;
                readlen = (int)::recv(socketfd, (char *)data + datalen, len - datalen, 0);
            }

            return datalen;
        }

        Int32 bytes_available(int socketfd) const {
            int count;
            int callResult = ioctl(socketfd, FIONREAD, &count);

            if (callResult < 0) {
                return callResult;
            }

            return count;
        }

        Int32 send(int socketfd, const void *data, int len) const {
            int byteswrite = 0;
            while (byteswrite < len) {
                int writelen = (int)::write(socketfd, (const char *)data + byteswrite, len - byteswrite);
                if (writelen < 0) {
                    return -1;
                }
                byteswrite += writelen;
            }
            return byteswrite;
        }

        //return socket fd
        Int32 listen(const char *address, int port) const {
            //create socket
            int socketfd = ::socket(AF_INET, SOCK_STREAM, 0);
            int reuseon = 1;
            setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuseon, sizeof(reuseon));
            set_block(socketfd, 0);

            //bind
            struct sockaddr_in serv_addr;
            memset( &serv_addr, '\0', sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr(address);
            serv_addr.sin_port = htons(port);
            int r = ::bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
            if (r == 0) {
                if (::listen(socketfd, 128) == 0) {
                    return socketfd;
                } else {
                    return -2;//listen error
                }
            } else {
                return -1;//bind error
            }
        }

        bool good(int fd) const noexcept
        {
            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            return ::getsockname(fd, (struct sockaddr *)&sin, &len) == 0;
        }

        //return client socket fd
        Int32 accept(Int32 onsocketfd, String& remoteip, Int32& remoteport, Int32 milliseconds) const {
            socklen_t clilen;
            struct sockaddr_in  cli_addr;
            clilen = sizeof(cli_addr);
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(onsocketfd, &fdset);
            struct timeval *timeptr = NULL;
            struct timeval timeout;
            if (milliseconds > 0) {
              timeout.tv_sec = milliseconds / 1000;
              timeout.tv_usec = milliseconds % 1000 * 1000UL;
              timeptr = &timeout;
            }
            int status = ::select(FD_SETSIZE, &fdset, NULL, NULL, timeptr);
            if (status != 1) {
              return -1;
            }
            int newsockfd = ::accept(onsocketfd, (struct sockaddr *) &cli_addr, &clilen);

            if (newsockfd > 0) {
                char clientip[16];
                remoteip = inet_ntop(AF_INET, &cli_addr.sin_addr, clientip, sizeof(clientip));
                remoteport = cli_addr.sin_port;
                set_block(newsockfd, 0);
                return newsockfd;
            } else {
                return -1;
            }
        }

        //return socket port
        Int32 port(Int32 socketfd) const {
            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (::getsockname(socketfd, (struct sockaddr *)&sin, &len) == -1) {
                return -1;
            } else {
                return ntohs(sin.sin_port);
            }
        }


    };


    using default_client = client<unix_tcp_socket_ctrl>;



    template<typename L>
    struct default_server: public server<unix_tcp_socket_ctrl>
    {


        default_server(const String& address, Int32 port, L& loop)
            : server(address, port), loop(loop), _read_event(0) {}

        void set_read_event(int event)
        {
            _read_event = event;
        }

        void start()
        {
            loop.on("io", [=](const any& argument ) {
                auto tuple = any_cast<std::tuple<int, int>>(argument);

                auto fd = std::get<0>(tuple);

                if(fd == descriptor()) {
                    auto client = accept(10);
                    if(client) {
                        loop.start(client->descriptor(), _read_event);
                        _accepted_clients[client->descriptor()] = client;
                        loop.emit ("accept", client);
                    }
                } else {
                    auto client = _accepted_clients[fd];
                    if(client) {
                        auto length = client->bytes_available();
                        auto result = client->read(length);
                        if(result.success()) {
                            auto bytes = std::make_shared<Bytes>(std::move(result.bytes()));
                            loop.emit ("data",  std::make_tuple(client, bytes));
                        } else if(result.error() == socket_error::connection_closed) {
                            client->set_descriptor(fd);
                            loop.emit ("close", client);
                        } else {
                            loop.emit ("error", result.message());
                        }
                    }
                }

            });


            loop.on("close", [=](const any& argument){
                auto client = any_cast<std::shared_ptr<default_client>>(argument);
                auto descriptor = client ? client->descriptor() : bad_descriptor;
                if(descriptor != bad_descriptor) {
                    loop.stop(descriptor);
                    _accepted_clients.erase(descriptor);
                }
            });

            if(listen().success()) {
                loop.start(descriptor(), _read_event);
            }
        }

        void close_client(std::shared_ptr<default_client> client)
        {
            if(client) {
                auto descriptor = client->descriptor();

                loop.stop(descriptor);
                _accepted_clients.erase(descriptor);

                loop.emit ("close", client);
                client->close();
            }
        }

        Int32 count() const
        {
            return _accepted_clients.size();
        }

    private:
        L& loop;
        int _read_event;
        std::map<int, std::shared_ptr<default_client>> _accepted_clients;
    };
#endif

}


}

}
#endif //! OYOUNG_NET_HPP

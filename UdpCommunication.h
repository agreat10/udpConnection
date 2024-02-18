#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>

class UdpCommunication {
    public:
    UdpCommunication(unsigned short localPort, const std::string& remoteIp, unsigned short remotePort):
    thread_(&UdpCommunication::SecondThread, this),
    remoteIp_(remoteIp),
    remotePort_(remotePort),
    localPort_(localPort){
        haveNewMessageToSend_ = false;
        running_ = true;
        thread_.launch();
    }
    std::string GetMessage(){
        mutex_.lock();
        std::string tmp = messageFromRemoteSocket_;
        mutex_.unlock();
        return tmp;
    }
    void SendMessage(const std::string& message)
    {
        mutex_.lock();
        haveNewMessageToSend_ = true;
        messageFromLocalSocket_ = message;
        mutex_.unlock();        
    }
    ~UdpCommunication(){
        running_ = false;
    }
    private:
    void SecondThread()
    {
        sf::UdpSocket socket_;
        if(socket_.bind(localPort_) != sf::Socket::Done)
        {
            std::cerr << "Failed to bind to port "  << localPort_ << std::endl;
        }
        socket_.setBlocking(false);

        while(running_)
        {
            if(haveNewMessageToSend_)
            {
                sf::Packet packet;
                mutex_.lock();
                packet.append(messageFromLocalSocket_.c_str(), messageFromLocalSocket_.size());
                mutex_.unlock();
                if(socket_.send(packet, sf::IpAddress(remoteIp_), remotePort_) != sf::Socket::Done)
                {
                    std::cout << "Failed to send message" << std::endl;
                }
                haveNewMessageToSend_ = false;
            }
            sf::Packet packet1;
            sf::IpAddress ip;
            unsigned short port;
            std::size_t received;
            char bufferTmp[1024] = "";
            if(socket_.receive(bufferTmp, 1024, received, ip, port) == sf::Socket::Done)
            {
                mutex_.lock();
                messageFromRemoteSocket_ = bufferTmp;
                mutex_.unlock();
            }
        }
    }
    private:
    unsigned short remotePort_;
    std::string remoteIp_;
    unsigned short localPort_;
    sf::Thread thread_;
    std::mutex mutex_;
    std::string messageFromRemoteSocket_;
    std::string messageFromLocalSocket_;
    std::atomic_bool haveNewMessageToSend_;
    std::atomic_bool running_;
};
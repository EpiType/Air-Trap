/**
 * File   : AsioEngine.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "plugins/net/asio/include/AsioEngine.hpp"

#include "engine/core/Logger.hpp"
#include "engine/net/ANetwork.hpp"

#include <asio.hpp>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>
#include <vector>

namespace engine::net::asio {
namespace {
using tcp = ::asio::ip::tcp;
using udp = ::asio::ip::udp;

constexpr std::size_t kHeaderSize = sizeof(std::uint32_t);
constexpr std::uint32_t kMaxMessageSize = 4 * 1024 * 1024;

std::uint32_t to_network_u32(std::uint32_t value) {
  if constexpr (std::endian::native == std::endian::little) {
    return std::byteswap(value);
  }
  return value;
}

std::uint32_t from_network_u32(std::uint32_t value) {
  return to_network_u32(value);
}

bool send_with_length(tcp::socket &socket, engine::net::ByteSpan payload) {
  std::uint32_t size = static_cast<std::uint32_t>(payload.size());
  std::uint32_t net = to_network_u32(size);
  std::array<std::uint8_t, kHeaderSize> header{};
  std::memcpy(header.data(), &net, kHeaderSize);

  ::asio::error_code ec;
  ::asio::write(socket, ::asio::buffer(header), ec);
  if (ec) {
    return false;
  }
  if (size > 0) {
    ::asio::write(socket, ::asio::buffer(payload.data(), payload.size()), ec);
    if (ec) {
      return false;
    }
  }
  return true;
}

class AsioClientNetwork final : public engine::net::ANetwork {
public:
  explicit AsioClientNetwork(engine::net::ClientConfig config)
      : _config(std::move(config)), _tcp(_io), _udp(_io) {}

  ~AsioClientNetwork() override { stop(); }

  bool start(void) override {
    if (_running) {
      return true;
    }

    ::asio::error_code ec;
    tcp::resolver resolver(_io);
    auto endpoints =
        resolver.resolve(_config.host, std::to_string(_config.tcpPort), ec);
    if (ec) {
      return false;
    }

    ::asio::connect(_tcp, endpoints, ec);
    if (ec) {
      return false;
    }

    _udp.open(udp::v4(), ec);
    if (ec) {
      return false;
    }
    _udp.bind(udp::endpoint(udp::v4(), 0), ec);
    if (ec) {
      return false;
    }

    udp::resolver uresolver(_io);
    auto uendpoints =
        uresolver.resolve(_config.host, std::to_string(_config.udpPort), ec);
    if (ec) {
      return false;
    }
    _udpEndpoint = *uendpoints.begin();

    startTcpReadHeader();
    startUdpRead();

    _running = true;
    _thread = std::thread([this]() {
      try {
        _io.run();
      } catch (...) {
        _running = false;
      }
    });
    return true;
  }

  void stop(void) override {
    _running = false;

    ::asio::error_code ec;
    _tcp.close(ec);
    _udp.close(ec);
    _io.stop();

    if (_thread.joinable()) {
      _thread.join();
    }
  }

  void sendPacket(uint32_t, engine::net::ByteSpan payload,
                  engine::net::NetChannel channel) override {
    if (!_running) {
      return;
    }

    auto data = std::make_shared<engine::net::ByteBuffer>(payload.begin(),
                                                          payload.end());
    ::asio::post(_io, [this, data, channel]() {
      if (channel == engine::net::NetChannel::TCP) {
        std::lock_guard lock(_writeMutex);
        send_with_length(_tcp, *data);
      } else {
        ::asio::error_code ec;
        _udp.send_to(::asio::buffer(data->data(), data->size()), _udpEndpoint,
                     0, ec);
      }
    });
  }

private:
  void startTcpReadHeader() {
    auto self = this;
    ::asio::async_read(_tcp, ::asio::buffer(_tcpHeader),
                       [this, self](const ::asio::error_code &ec, std::size_t) {
                         if (ec || !_running) {
                           return;
                         }
                         std::uint32_t netSize = 0;
                         std::memcpy(&netSize, _tcpHeader.data(), kHeaderSize);
                         const std::uint32_t size = from_network_u32(netSize);
                         if (size > kMaxMessageSize) {
                           return;
                         }
                         _tcpBody.resize(size);
                         startTcpReadBody();
                       });
  }

  void startTcpReadBody() {
    auto self = this;
    if (_tcpBody.empty()) {
      pushEvent(engine::net::NetworkEvent{0, {}, engine::net::NetChannel::TCP});
      startTcpReadHeader();
      return;
    }
    ::asio::async_read(_tcp, ::asio::buffer(_tcpBody.data(), _tcpBody.size()),
                       [this, self](const ::asio::error_code &ec, std::size_t) {
                         if (ec || !_running) {
                           return;
                         }
                         pushEvent(engine::net::NetworkEvent{
                             0, _tcpBody, engine::net::NetChannel::TCP});
                         startTcpReadHeader();
                       });
  }

  void startUdpRead() {
    auto self = this;
    _udpBuffer.fill(0);
    _udp.async_receive_from(
        ::asio::buffer(_udpBuffer), _udpSender,
        [this, self](const ::asio::error_code &ec, std::size_t size) {
          if (!ec && size > 0 && _running) {
            engine::net::ByteBuffer payload(_udpBuffer.begin(),
                                            _udpBuffer.begin() + size);
            pushEvent(engine::net::NetworkEvent{0, std::move(payload),
                                                engine::net::NetChannel::UDP});
          }
          if (_running) {
            startUdpRead();
          }
        });
  }

private:
  engine::net::ClientConfig _config{};
  ::asio::io_context _io;
  tcp::socket _tcp;
  udp::socket _udp;
  udp::endpoint _udpEndpoint;

  std::atomic<bool> _running{false};
  std::thread _thread;
  std::mutex _writeMutex;

  std::array<std::uint8_t, kHeaderSize> _tcpHeader{};
  engine::net::ByteBuffer _tcpBody{};

  std::array<std::uint8_t, kMaxMessageSize> _udpBuffer{};
  udp::endpoint _udpSender{};
};

class AsioServerNetwork final : public engine::net::ANetwork {
public:
  explicit AsioServerNetwork(engine::net::ServerConfig config)
      : _config(std::move(config)), _acceptor(_io), _udp(_io) {}

  ~AsioServerNetwork() override { stop(); }

  bool start(void) override {
    if (_running) {
      return true;
    }

    ::asio::error_code ec;
    tcp::endpoint endpoint(tcp::v4(), _config.tcpPort);
    _acceptor.open(endpoint.protocol(), ec);
    if (ec) {
      return false;
    }
    _acceptor.set_option(tcp::acceptor::reuse_address(true), ec);
    _acceptor.bind(endpoint, ec);
    if (ec) {
      return false;
    }
    _acceptor.listen(static_cast<int>(_config.maxSessions), ec);
    if (ec) {
      return false;
    }

    _udp.open(udp::v4(), ec);
    if (ec) {
      return false;
    }
    _udp.bind(udp::endpoint(udp::v4(), _config.udpPort), ec);
    if (ec) {
      return false;
    }

    startAccept();
    startUdpRead();

    _running = true;
    _thread = std::thread([this]() {
      try {
        _io.run();
      } catch (...) {
        _running = false;
      }
    });
    return true;
  }

  void stop(void) override {
    _running = false;

    ::asio::error_code ec;
    _acceptor.close(ec);
    _udp.close(ec);

    {
      std::lock_guard lock(_sessionsMutex);
      for (auto &[id, session] : _sessions) {
        (void)id;
        session->socket.close(ec);
      }
      _sessions.clear();
    }

    _io.stop();
    if (_thread.joinable()) {
      _thread.join();
    }
  }

  void sendPacket(uint32_t sessionId, engine::net::ByteSpan payload,
                  engine::net::NetChannel channel) override {
    if (!_running) {
      return;
    }

    auto session = getSession(sessionId);
    if (!session) {
      return;
    }

    auto data = std::make_shared<engine::net::ByteBuffer>(payload.begin(),
                                                          payload.end());
    ::asio::post(_io, [this, session, data, channel]() {
      if (!session) {
        return;
      }
      if (channel == engine::net::NetChannel::TCP) {
        std::lock_guard lock(session->writeMutex);
        send_with_length(session->socket, *data);
        return;
      }

      if (!session->udpEndpoint.has_value()) {
        return;
      }
      ::asio::error_code ec;
      _udp.send_to(::asio::buffer(data->data(), data->size()),
                   session->udpEndpoint.value(), 0, ec);
    });
  }

private:
  struct Session {
    uint32_t id{0};
    tcp::socket socket;
    std::mutex writeMutex;
    std::optional<udp::endpoint> udpEndpoint;
    std::array<std::uint8_t, kHeaderSize> header{};
    engine::net::ByteBuffer body{};

    Session(uint32_t sid, tcp::socket sock)
        : id(sid), socket(std::move(sock)) {}
  };

  std::shared_ptr<Session> getSession(uint32_t sessionId) {
    std::lock_guard lock(_sessionsMutex);
    auto it = _sessions.find(sessionId);
    if (it == _sessions.end()) {
      return nullptr;
    }
    return it->second;
  }

  std::shared_ptr<Session> matchSession(const udp::endpoint &endpoint) {
    std::lock_guard lock(_sessionsMutex);
    for (auto &[id, session] : _sessions) {
      (void)id;
      if (session->udpEndpoint.has_value() &&
          session->udpEndpoint.value() == endpoint) {
        return session;
      }
    }

    for (auto &[id, session] : _sessions) {
      (void)id;
      ::asio::error_code ec;
      const auto addr = session->socket.remote_endpoint(ec).address();
      if (ec) {
        continue;
      }
      if (addr == endpoint.address() && !session->udpEndpoint.has_value()) {
        session->udpEndpoint = endpoint;
        return session;
      }
    }
    return nullptr;
  }

  void startAccept() {
    auto socket = std::make_shared<tcp::socket>(_io);
    _acceptor.async_accept(*socket, [this,
                                     socket](const ::asio::error_code &ec) {
      if (!ec && _running) {
        engine::core::info("AsioServerNetwork: New TCP connection accepted");
        const uint32_t id = _nextSessionId++;
        auto session = std::make_shared<Session>(id, std::move(*socket));
        {
          std::lock_guard lock(_sessionsMutex);
          _sessions[id] = session;
        }
        startTcpReadHeader(session);
      }
      if (_running) {
        startAccept();
      }
    });
  }

  void startTcpReadHeader(const std::shared_ptr<Session> &session) {
    ::asio::async_read(
        session->socket, ::asio::buffer(session->header),
        [this, session](const ::asio::error_code &ec, std::size_t) {
          if (ec || !_running) {
            removeSession(session->id);
            return;
          }
          std::uint32_t netSize = 0;
          std::memcpy(&netSize, session->header.data(), kHeaderSize);
          const std::uint32_t size = from_network_u32(netSize);
          if (size > kMaxMessageSize) {
            removeSession(session->id);
            return;
          }
          session->body.resize(size);
          startTcpReadBody(session);
        });
  }

  void startTcpReadBody(const std::shared_ptr<Session> &session) {
    if (session->body.empty()) {
      pushEvent(engine::net::NetworkEvent{
          session->id, {}, engine::net::NetChannel::TCP});
      startTcpReadHeader(session);
      return;
    }

    ::asio::async_read(
        session->socket,
        ::asio::buffer(session->body.data(), session->body.size()),
        [this, session](const ::asio::error_code &ec, std::size_t) {
          if (ec || !_running) {
            removeSession(session->id);
            return;
          }
          pushEvent(engine::net::NetworkEvent{session->id, session->body,
                                              engine::net::NetChannel::TCP});
          startTcpReadHeader(session);
        });
  }

  void startUdpRead() {
    _udpBuffer.fill(0);
    _udp.async_receive_from(
        ::asio::buffer(_udpBuffer), _udpSender,
        [this](const ::asio::error_code &ec, std::size_t size) {
          if (!ec && size > 0 && _running) {
            auto session = matchSession(_udpSender);
            if (session) {
              engine::net::ByteBuffer payload(_udpBuffer.begin(),
                                              _udpBuffer.begin() + size);
              pushEvent(
                  engine::net::NetworkEvent{session->id, std::move(payload),
                                            engine::net::NetChannel::UDP});
            }
          }
          if (_running) {
            startUdpRead();
          }
        });
  }

  void removeSession(uint32_t sessionId) {
    std::shared_ptr<Session> session;
    {
      std::lock_guard lock(_sessionsMutex);
      auto it = _sessions.find(sessionId);
      if (it == _sessions.end()) {
        return;
      }
      session = it->second;
      _sessions.erase(it);
    }

    if (session) {
      ::asio::error_code ec;
      session->socket.close(ec);
    }
  }

private:
  engine::net::ServerConfig _config{};
  ::asio::io_context _io;
  tcp::acceptor _acceptor;
  udp::socket _udp;

  std::atomic<bool> _running{false};
  std::atomic<uint32_t> _nextSessionId{1};

  std::mutex _sessionsMutex;
  std::unordered_map<uint32_t, std::shared_ptr<Session>> _sessions;
  std::thread _thread;

  std::array<std::uint8_t, kMaxMessageSize> _udpBuffer{};
  udp::endpoint _udpSender{};
};

} // namespace

std::string AsioEngine::getName(void) const { return "asio"; }

INetwork *AsioEngine::createClient(const ClientConfig &config) {
  return new AsioClientNetwork(config);
}

INetwork *AsioEngine::createServer(const ServerConfig &config) {
  return new AsioServerNetwork(config);
}

void AsioEngine::destroy(INetwork *network) { delete network; }
} // namespace engine::net::asio

extern "C" {
    engine::net::INetworkEngine *CreateNetworkEngine() {
        return new engine::net::asio::AsioEngine();
    }

    void DestroyNetworkEngine(engine::net::INetworkEngine *engine) {
        delete engine;
    }
}

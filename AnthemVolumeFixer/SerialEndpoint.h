#pragma once

#define ASIO_STANDALONE
#include "asio\include\asio.hpp"

#include "CommandParser.h"

namespace Anthem {

class SerialEndpoint final
{
public:
	typedef std::function<void(Command)> Sink;

	SerialEndpoint(std::string&& port);
	~SerialEndpoint();

	void OnCommand(std::weak_ptr<Sink> sink);
	void Send(Command c);

	void Connect();

private:
	void ReadSomeAsync();
	void OnParsedCommand(Command c);

	std::string				m_port;
	std::thread				m_thread;
	asio::io_context		m_context;
	asio::io_context::work	m_workguard;
	asio::serial_port		m_serialport;
	asio::streambuf			m_readbuffer;

	std::shared_ptr<CommandParser::Sink>
							m_parserSink;

	std::weak_ptr<Sink>
							m_notificationSink;
	CommandParser			m_parser;

};

}
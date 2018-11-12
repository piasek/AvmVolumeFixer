#include "stdafx.h"
#include "SerialEndpoint.h"

namespace Anthem {

static const int MAX_READ_MESSAGE = 258;

SerialEndpoint::SerialEndpoint(std::string&& port) :
	m_port(std::move(port)),
	m_context(),
	m_workguard(m_context),
	m_serialport(m_context),
	m_readbuffer(MAX_READ_MESSAGE),
	m_parserSink(std::make_shared<CommandParser::Sink>([this](Command c) { OnParsedCommand(c); })),
	m_parser(m_parserSink)
{
}

void SerialEndpoint::Connect()
{
	m_serialport.open(m_port);

	m_serialport.set_option(asio::serial_port::baud_rate(115200));
	m_serialport.set_option(asio::serial_port::character_size(8));
	m_serialport.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
	m_serialport.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
	m_serialport.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

	m_thread = std::thread([this]()
	{
		m_context.run();
	});

	ReadSomeAsync();
}

void SerialEndpoint::ReadSomeAsync()
{
	asio::async_read(m_serialport, m_readbuffer, asio::transfer_at_least(1), [this](const asio::error_code& e, std::size_t bytesRead)
	{
		m_parser.Push(std::string_view(static_cast<const char*>(m_readbuffer.data().data()), bytesRead));
		m_readbuffer.consume(bytesRead);
		ReadSomeAsync();
	});
}

SerialEndpoint::~SerialEndpoint()
{
	m_serialport.close();
	m_context.stop();
	m_thread.join();
}

void SerialEndpoint::OnCommand(std::weak_ptr<Sink> sink)
{
	m_notificationSink = sink;
}

void SerialEndpoint::Send(Command c)
{
	asio::write(m_serialport, asio::buffer(c), asio::transfer_at_least(c.size()));
}

void SerialEndpoint::OnParsedCommand(Command c)
{
	auto notificationSink = m_notificationSink.lock();
	if (notificationSink)
	{
		(*notificationSink)(c);
	}
}

}
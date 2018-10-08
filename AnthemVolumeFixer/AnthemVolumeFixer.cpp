// AnthemVolumeFixer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		throw std::exception("Expected parameters <serial port name> <volume level>");
	}

	char* portname = argv[1];
	int volume = ::atoi(argv[2]);

	asio::io_service service;
	asio::serial_port serialport(service);

	serialport.open(portname);

	serialport.set_option(asio::serial_port::baud_rate(115200));
	serialport.set_option(asio::serial_port::character_size(8));
	serialport.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
	serialport.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
	serialport.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

	std::string command{ "HELLO;" };
	asio::write(serialport, asio::buffer(command.c_str(), command.length()));

	char response;
	while (asio::read(serialport, asio::buffer(&response, 1)))
	{
		std::cout << response;
		if (response == ';')
		{
			std::cout << std::endl;
		}
	}
}


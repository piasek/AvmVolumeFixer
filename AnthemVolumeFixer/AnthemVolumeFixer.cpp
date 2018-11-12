// AnthemVolumeFixer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SerialEndpoint.h"

#include <memory>

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		throw std::exception("Expected parameters <serial port name> <volume level>");
	}

	char* portname = argv[1];
	int volume = ::atoi(argv[2]);

	const std::string volumePrefix = "Z1VOL";
	std::string preferredVolumeCommand;
	{
		std::stringstream ss;
		ss << volumePrefix << volume << ";";
		preferredVolumeCommand = ss.str();
	}

	Anthem::SerialEndpoint avm(portname);

	auto handler = std::make_shared<Anthem::SerialEndpoint::Sink>([&](Anthem::Command command) 
	{ 
		std::cout << command << std::endl; 
		if (command.find(volumePrefix) == 0 && command != preferredVolumeCommand)
		{
			avm.Send(preferredVolumeCommand);
		}
	});

	avm.OnCommand(handler);
	avm.Connect();
	
	std::string s;
	while (s != "q")
	{
		std::cin >> s;
		if (s != "q")
		{
			avm.Send(s);
		}
	}
}

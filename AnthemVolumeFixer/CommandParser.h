#pragma once

#include <functional>
#include <string_view>

namespace Anthem {

typedef std::string Command;

class CommandParser
{
public:
	typedef std::function<void(Command)> Sink;

	CommandParser(std::shared_ptr<Sink> sink);
	~CommandParser();

	void Push(std::string_view data);

private:
	std::weak_ptr<Sink>	m_sink;
	std::string			m_buffer;
};

}
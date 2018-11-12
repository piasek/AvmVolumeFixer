#include "stdafx.h"
#include "CommandParser.h"

namespace Anthem {

CommandParser::CommandParser(std::shared_ptr<Sink> sink) :
	m_sink(sink)
{
}

CommandParser::~CommandParser()
{
}

void CommandParser::Push(std::string_view data)
{
	for (auto c : data)
	{
		m_buffer.push_back(c);

		if (c == ';')
		{
			auto m_sharedsink = m_sink.lock();
			if (m_sharedsink)
			{
				auto& sink = *m_sharedsink;
				sink(m_buffer);
			}
			m_buffer.clear();
		}
	}
}

}
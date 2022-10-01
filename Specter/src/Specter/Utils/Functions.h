#ifndef SPEC_FUNCTIONS_H
#define SPEC_FUNCTIONS_H

namespace Specter {

	namespace Utilities
	{
		//Use Szudzik pairing function to convert board/channel number pair to a single universal unqiue identifier (UUID)
		//Allows us to unqiuely id board-channel combo even with a set of boards that don't have the same number of channels
		//It's constexpr, since for each board/channel pair the value should be evaluated at compile time (and allows us to use values in a switch)
		//This is mostly syntactic sugar. When board and channel are constexpr (read: literals) it gives a constexpr value, otherwise behaves same as any other function
		constexpr uint32_t GetBoardChannelUUID(uint32_t board, uint32_t channel)
		{
			return board >= channel ? (board * board + board + channel) : (channel * channel + board);
		}
	}
}

#endif
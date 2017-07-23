#pragma once
#include <array>

namespace mix {

class Computer;
class Command;
class Register;
class Word;

class CommandProcessor
{
public:
	static constexpr std::size_t k_commands_count = 64;

	explicit CommandProcessor(Computer& mix);

	void process(const Command& command);

	void nop(const Command& command);
	void lda(const Command& command);
	void ld1(const Command& command);
	void ld2(const Command& command);
	void ld3(const Command& command);
	void ld4(const Command& command);
	void ld5(const Command& command);
	void ld6(const Command& command);
	void ldx(const Command& command);

	void sta(const Command& command);
	void st1(const Command& command);
	void st2(const Command& command);
	void st3(const Command& command);
	void st4(const Command& command);
	void st5(const Command& command);
	void st6(const Command& command);
	void stx(const Command& command);
	void stz(const Command& command);
	void stj(const Command& command);

private:
	void load_register(Register& r, const Command& command);
	void store_register(Register& r, const Command& command);
	void load_index_register(std::size_t index, const Command& command);

	Word& memory(const Command& command);

private:
	Computer& mix_;

	using CommandAction = void (CommandProcessor::*)(const Command& command);
	static const std::array<CommandAction, k_commands_count> k_command_actions;
};

} // namespace mix


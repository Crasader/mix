#include <mix/command_processor.h>
#include <mix/computer.h>
#include <mix/command.h>

using namespace mix;

/*static*/ const std::array<
	CommandProcessor::CommandAction,
	CommandProcessor::k_commands_count>
CommandProcessor::k_command_actions = {
	/*00*/&CommandProcessor::nop,
	/*01*/&CommandProcessor::add,
	/*02*/nullptr,
	/*03*/nullptr,
	/*04*/nullptr,
	/*05*/nullptr,
	/*06*/nullptr,
	/*07*/nullptr,
	/*08*/&CommandProcessor::lda,
	/*09*/&CommandProcessor::ld1,
	/*10*/&CommandProcessor::ld2,
	/*11*/&CommandProcessor::ld3,
	/*12*/&CommandProcessor::ld4,
	/*13*/&CommandProcessor::ld5,
	/*14*/&CommandProcessor::ld6,
	/*15*/&CommandProcessor::ldx,
	/*16*/&CommandProcessor::ldan,
	/*17*/&CommandProcessor::ld1n,
	/*18*/&CommandProcessor::ld2n,
	/*19*/&CommandProcessor::ld3n,
	/*20*/&CommandProcessor::ld4n,
	/*21*/&CommandProcessor::ld5n,
	/*22*/&CommandProcessor::ld6n,
	/*23*/&CommandProcessor::ldxn,
	/*24*/&CommandProcessor::sta,
	/*25*/&CommandProcessor::st1,
	/*26*/&CommandProcessor::st2,
	/*27*/&CommandProcessor::st3,
	/*28*/&CommandProcessor::st4,
	/*29*/&CommandProcessor::st5,
	/*30*/&CommandProcessor::st6,
	/*31*/&CommandProcessor::stx,
	/*32*/&CommandProcessor::stj,
	/*33*/&CommandProcessor::stz,
	/*34*/nullptr,
};


CommandProcessor::CommandProcessor(Computer& mix)
	: mix_{mix}
{
}

void CommandProcessor::process(const Command& command)
{
	static_assert(k_commands_count == (Byte::k_max_value + 1),
		"Command actions array should have all possible variations of command IDs");

	auto callback = k_command_actions[command.id()];
	if (!callback)
	{
		throw std::exception{"Not implemented"};
	}

	(*this.*callback)(command);
}

Word& CommandProcessor::memory(const Command& command)
{
	return mix_.memory_with_index(command.address(), command.address_index());
}

void CommandProcessor::nop(const Command& /*command*/)
{
}

void CommandProcessor::load_register(Register& r, const Command& command)
{
	const auto& word = memory(command);
	const auto& source_field = command.word_field();

#if(1)
	auto dest_field = source_field.shift_bytes_right();
	r.set_value(word.value(source_field), dest_field);
	
	// Zero rest of bytes.
	// #TODO: this way of setting the value looks like
	// very inefficient. Maybe it's better to iterate over
	// `source_field` bytes directly ?
	std::size_t until_byte = dest_field.has_only_sign()
		? Word::k_bytes_count
		: dest_field.left_byte_index() - 1;
	for (std::size_t i = 1; i <= until_byte; ++i)
	{
		r.set_byte(i, Byte{0});
	}
#else
	if (source_field.includes_sign())
	{
		r.set_sign(word.sign());
	}

	if (source_field.has_only_sign())
	{
		std::size_t until_byte = Word::k_bytes_count;
		for (std::size_t i = 1; i <= until_byte; ++i)
		{
			r.set_byte(i, Byte{0});
		}

		return;
	}

	auto dest_field = source_field.shift_bytes_right();
	auto diff = dest_field.right_byte_index() - source_field.right_byte_index();

	std::size_t until_byte = dest_field.left_byte_index() - 1;
	for (std::size_t i = 1; i <= until_byte; ++i)
	{
		r.set_byte(i, Byte{0});
	}

	for (auto i = source_field.left_byte_index();
		i <= source_field.right_byte_index();
		++i)
	{
		r.set_byte(i + diff, word.byte(i));
	}
#endif
}

void CommandProcessor::load_register_reverse_sign(Register& r, const Command& command)
{
	const auto& word = memory(command);
	const auto& source_field = command.word_field();

	r.set_value(word.value(source_field).reverse_sign(), source_field.shift_bytes_right());
	
	// #TODO: set to zero rest of bytes as in `load_register()`
}

void CommandProcessor::store_register(Register& r, const Command& command)
{
	auto& word = memory(command);
	const auto& source_field = command.word_field();

	word.set_value(
		r.value(source_field.shift_bytes_right()),
		source_field,
		false/*do not overwrite sign*/);
}

void CommandProcessor::load_index_register(std::size_t index, const Command& command)
{
	load_register(mix_.index_register(index), command);
}

void CommandProcessor::load_index_register_reverse_sign(std::size_t index, const Command& command)
{
	load_register_reverse_sign(mix_.index_register(index), command);
}

void CommandProcessor::lda(const Command& command)
{
	load_register(mix_.ra_, command);
}

void CommandProcessor::ldx(const Command& command)
{
	load_register(mix_.rx_, command);
}

void CommandProcessor::ld1(const Command& command)
{
	load_index_register(1, command);
}

void CommandProcessor::ld2(const Command& command)
{
	load_index_register(2, command);
}

void CommandProcessor::ld3(const Command& command)
{
	load_index_register(3, command);
}

void CommandProcessor::ld4(const Command& command)
{
	load_index_register(4, command);
}

void CommandProcessor::ld5(const Command& command)
{
	load_index_register(5, command);
}

void CommandProcessor::ld6(const Command& command)
{
	load_index_register(6, command);
}

void CommandProcessor::ldan(const Command& command)
{
	load_register_reverse_sign(mix_.ra_, command);
}

void CommandProcessor::ldxn(const Command& command)
{
	load_register_reverse_sign(mix_.rx_, command);
}

void CommandProcessor::ld1n(const Command& command)
{
	load_index_register_reverse_sign(1, command);
}

void CommandProcessor::ld2n(const Command& command)
{
	load_index_register_reverse_sign(2, command);
}

void CommandProcessor::ld3n(const Command& command)
{
	load_index_register_reverse_sign(3, command);
}

void CommandProcessor::ld4n(const Command& command)
{
	load_index_register_reverse_sign(4, command);
}

void CommandProcessor::ld5n(const Command& command)
{
	load_index_register_reverse_sign(5, command);
}

void CommandProcessor::ld6n(const Command& command)
{
	load_index_register_reverse_sign(6, command);
}

void CommandProcessor::sta(const Command& command)
{
	store_register(mix_.ra_, command);
}

void CommandProcessor::stx(const Command& command)
{
	store_register(mix_.rx_, command);
}

void CommandProcessor::st1(const Command& command)
{
	store_register(mix_.index_register(1), command);
}

void CommandProcessor::st2(const Command& command)
{
	store_register(mix_.index_register(2), command);
}

void CommandProcessor::st3(const Command& command)
{
	store_register(mix_.index_register(3), command);
}

void CommandProcessor::st4(const Command& command)
{
	store_register(mix_.index_register(4), command);
}

void CommandProcessor::st5(const Command& command)
{
	store_register(mix_.index_register(5), command);
}

void CommandProcessor::st6(const Command& command)
{
	store_register(mix_.index_register(6), command);
}

void CommandProcessor::stz(const Command& command)
{
	auto& word = memory(command);
	word.set_value(0, command.word_field());
}

void CommandProcessor::stj(const Command& command)
{
	// #TODO: default WordField should be (0, 2) instead of (0, 5)
	store_register(mix_.rj_, command);
}

void CommandProcessor::add(const Command& command)
{
	const int value = memory(command).value(command.word_field());
	const int prev_value = mix_.ra_.value();

	// #TODO: overflow flag
	mix_.ra_.set_value(value + prev_value);
}


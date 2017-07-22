#include <mix/command.h>

using namespace mix;


Command::Command(const Word& word)
	: word_{word}
	, field_{Field::FromByte(field_byte())}
	, address_{word.sign(), word.byte(1), word.byte(2)}
{
}

Command::Command(
	const Byte& id,
	const AddressRegister& address,
	const Byte& address_index,
	const Field& field)
		: word_{}
		, field_{field}
		, address_{address}
{
	word_.set_sign(address_.sign());
	word_.set_byte(1, address_.byte(4));
	word_.set_byte(2, address_.byte(5));
	word_.set_byte(3, address_index);
	word_.set_byte(4, field_.to_byte());
	word_.set_byte(5, id);
}

const Byte& Command::id() const
{
	return word_.byte(5);
}

const Byte& Command::field_byte() const
{
	return word_.byte(4);
}

const Byte& Command::address_index() const
{
	return word_.byte(3);
}

const Field& Command::field() const
{
	return field_;
}

const Word& Command::to_word() const
{
	return word_;
}

const AddressRegister& Command::address() const
{
	return address_;
}

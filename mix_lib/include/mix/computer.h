#include <mix/registers.h>
#include <mix/general_enums.h>

namespace mix {

class Command;
struct IComputerListener;

class Computer
{
public:
	static constexpr std::size_t k_index_registers_count = 6;
	static constexpr std::size_t k_memory_words_count = 4000;

	explicit Computer(IComputerListener* listener = nullptr);

	void set_listener(IComputerListener* listener);

	void execute(const Command& command);

	void set_memory(int, const Word& value);
	const Word& memory(int address) const;

	const Register& ra() const;
	void set_ra(const Register& ra);

	const Register& rx() const;
	void set_rx(const Register& rx);

	const IndexRegister& ri(std::size_t index) const;
	void set_ri(std::size_t index, const IndexRegister& ri);

	const AddressRegister& rj() const;

	OverflowFlag overflow_flag() const;
	bool has_overflow() const;
	void set_overflow();

	ComparisonIndicator comparison_state() const;
	void set_comparison_state(ComparisonIndicator comparison);

private:


private:
	Register ra_;
	Register rx_;
	std::array<IndexRegister, k_index_registers_count> rindexes_;
	AddressRegister rj_;

	ComparisonIndicator comparison_state_;
	OverflowFlag overflow_flag_;

	std::array<Word, k_memory_words_count> memory_;

	IComputerListener* listener_;
};


} // namespace mix

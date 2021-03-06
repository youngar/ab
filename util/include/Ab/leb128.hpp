#ifndef AB_LEB128_HPP_
#define AB_LEB128_HPP_

#include <cstddef>
#include <cstdint>
#include <istream>

namespace Ab {

class ReaderError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class BadNumber : public ReaderError {
public:
	BadNumber() : ReaderError{"Could not decode number"} {}
};

struct ReaderInput {
	ReaderInput(std::istream& in) : in_{in}, offset_{0} {}

	ReaderInput(const ReaderInput& other) : ReaderInput{other.in_} {}

	std::uint8_t get() {
		std::uint8_t x = in_.get();
		offset_ += in_.gcount();
		return x;
	}

	void read(char* buffer, std::streamsize bytes) {
		in_.read(buffer, bytes);

		if (in_.gcount() != bytes)
			throw ReaderError("didn't read all the bytes like we were hoping");
		// TODO:

		offset_ += bytes;
	}

	std::size_t offset() const { return offset_; }

	std::istream& stream() const { return in_; }

private:
	std::istream& in_;
	std::size_t offset_;
};

template <typename Integer, std::size_t bytes = sizeof(Integer)>
Integer readNumber(ReaderInput& in) {
	Integer result = 0;
	auto buffer    = (char*)&result;
	in.read(buffer, bytes);
	return result;
}

inline std::int64_t leb128(ReaderInput& in) {
	const std::uint8_t FLAG = 0b1000'0000;
	const std::uint8_t MASK = 0b0111'1111;
	const std::uint8_t SIGN = 0b0100'0000;

	std::uint64_t result = 0;
	std::size_t shift    = 0;
	std::uint8_t byte    = 0;

	do {
		if (shift > 63)
			throw BadNumber();

		byte = in.get();
		result |= std::uint64_t(byte & MASK) << shift;
		shift += 7;

	} while (byte & FLAG && (shift < 64) && !in.stream().eof());

	if (byte & SIGN) {
		result |= ~std::uint64_t(0) << shift;
	}

	return result;
}

inline std::int64_t leb128(std::istream& s) {
	ReaderInput in(s);
	return leb128(in);
}

/// Read a uleb128 from a buffer. Reads up to n characters
inline std::uint64_t uleb128(ReaderInput& in) {
	constexpr std::uint8_t FLAG = 0b1000'0000;
	constexpr std::uint8_t MASK = 0b0111'1111;

	std::uint64_t result = 0;
	std::size_t shift    = 0;
	std::uint8_t byte    = 0;

	do {
		if (shift > 63)
			throw BadNumber();

		byte = in.get();
		result |= std::uint64_t(byte & MASK) << shift;
		shift += 7;
	} while ((byte & FLAG) != 0 && shift < 64 && !in.stream().eof());

	return result;
}

inline std::int64_t uleb128(std::istream& s) {
	ReaderInput in(s);
	return uleb128(in);
}

inline std::uint64_t varuint32(ReaderInput& in) { return uleb128(in); }

}  // namespace Ab

#endif  // AB_LEB128_HPP_

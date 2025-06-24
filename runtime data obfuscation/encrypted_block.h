#pragma once
#include "cpu.h" 
#include <memory>

// A lightweight obfuscation wrapper that encrypts any data type T at runtime,
// storing it in an unreadable format in memory and allowing explicit decryption.
template <typename T>
struct encrypted_block {

	// Encrypted bytes of the original value
	uint8_t memory_block[sizeof(T)]{};

	// Random per-byte keys used in the encryption process
	uint8_t key_block[sizeof(T)]{};

	// Default constructor: does nothing by design
	__forceinline encrypted_block()
	{
		/* No initialization required for empty state */
	}

	// Construct from a plaintext value (automatically encrypts on store)
	__forceinline encrypted_block(const T& value) {
		store(value);
	}

	// Copy constructor: copies encrypted state (not decrypted value)
	__forceinline encrypted_block(const encrypted_block& other) {
		__memcpy(memory_block, other.memory_block, sizeof(T));
		__memcpy(key_block, other.key_block, sizeof(T));
	}

	// Move constructor: transfers encrypted state (without clearing source)
	__forceinline encrypted_block(encrypted_block&& other) noexcept {
		__memcpy(memory_block, other.memory_block, sizeof(T));
		__memcpy(key_block, other.key_block, sizeof(T));
	}

	// Destructor: optionally wipes memory for security (commented out by default)
	__forceinline ~encrypted_block() {
		// __memzero(memory_block, sizeof(T));
		// __memzero(key_block, sizeof(T));
	}

	// Assignment from plaintext value
	__forceinline void operator=(const T& value) {
		store(value);
	}

	// Assignment from another encrypted_block (copies raw encrypted data)
	__forceinline void operator=(const encrypted_block& other) {
		__memcpy(memory_block, other.memory_block, sizeof(T));
		__memcpy(key_block, other.key_block, sizeof(T));
	}

	// Equality comparison (decrypts and compares to plaintext)
	__forceinline bool operator==(const T& rhs) const {
		T temp = get_decrypted();
		return memcmp(&temp, &rhs, sizeof(T)) == 0;
	}

	// Inequality comparison (inverted ==)
	__forceinline bool operator!=(const T& rhs) const {
		return !(*this == rhs);
	}

	// Decrypts the stored value and returns a copy
	__forceinline T get_decrypted() const {
		uint8_t decrypted[sizeof(T)];

		for (size_t i = 0; i < sizeof(T); ++i) {
			uint8_t k = key_block[i];

			// Fixed value used for additional variability
			uint8_t g = static_cast<uint8_t>((35 >> (i % 64)) & 0xFF);

			// Mixing key and g for per-byte mutation
			uint8_t m = k ^ g;

			// Rotation amount derived from mutation
			uint8_t r = m & 7;

			// Decode: rotate back, then XOR with mutation
			uint8_t c = memory_block[i];
			decrypted[i] = ((c >> r) | (c << ((8 - r) & 7))) ^ m;
		}

		T result;
		__memcpy(&result, decrypted, sizeof(T));
		return result;
	}

	// Returns the encrypted form as-is (raw copy of memory_block)
	__forceinline T get_encrypted() const {
		T result;
		__memcpy(&result, memory_block, sizeof(T));
		return result;
	}

private:
	// Encrypts and stores a given value into memory_block using randomized key_block
	__forceinline void store(const T& value) {

		const uint8_t* plain = reinterpret_cast<const uint8_t*>(&value);

		for (size_t i = 0; i < sizeof(T); ++i) {
			// Random key byte
			uint8_t k = (_rand_uchar());

			// Constant-derived value to prevent predictable key patterns
			uint8_t g = static_cast<uint8_t>((35 >> (i % 64)) & 0xFF);

			// XOR mutation of key and derived value
			uint8_t m = k ^ g;

			// Rotation amount from lower 3 bits of m
			uint8_t r = m & 7;

			// XOR input byte with mutation
			uint8_t x = plain[i] ^ m;

			// Rotate result and store
			key_block[i] = k;
			memory_block[i] = (x << r) | (x >> ((8 - r) & 7));
		}
	}
};
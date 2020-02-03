#include <iostream>
#include <array>
#include <string>
#include <cmath>
#include <bitset>
#include <cstdint>
#include <sstream>
#include <chrono>
#include <vector>
#include <cmath>
using namespace std;
typedef uint64_t int64;
typedef uint32_t int32;
typedef uint8_t int8;
class sha256 {
public:
	std::string binary_data, result;
	int32 k[64] = { 1116352408, 1899447441, 3049323471, 3921009573, 961987163, 1508970993, 2453635748, 2870763221, 3624381080, 310598401, 607225278, 1426881987, 1925078388, 2162078206, 2614888103, 3248222580, 3835390401, 4022224774, 264347078, 604807628, 770255983, 1249150122, 1555081692, 1996064986, 2554220882, 2821834349, 2952996808, 3210313671, 3336571891, 3584528711, 113926993, 338241895, 666307205, 773529912, 1294757372, 1396182291, 1695183700, 1986661051, 2177026350, 2456956037, 2730485921, 2820302411, 3259730800, 3345764771, 3516065817, 3600352804, 4094571909, 275423344, 430227734, 506948616, 659060556, 883997877, 958139571, 1322822218, 1537002063, 1747873779, 1955562222, 2024104815, 2227730452, 2361852424, 2428436474, 2756734187, 3204031479, 3329325298 };
	int32 hash[8] = { 1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225 };
	sha256(string data);
	inline uint32_t maj(int32 a, int32 b, int32 c);
	inline uint32_t ch(int32 a, int32 b, int32 c);
	inline uint32_t sigma0256(int32 data);
	inline uint32_t sigma1256(int32 data);
	inline uint32_t Csigma0256(int32 data);
	inline uint32_t Csigma1256(int32 data);
	inline int32 shr(int32 data, unsigned m);
	inline int32 rotr(int32 data, unsigned m);
	inline uint32_t hex_int_to_dec_int(int32 data);
	inline void assignhex(int32 &a, int32 &b);
	inline uint32_t bin_str_to_int32(std::string data);
	inline std::string compute(int32 **m, int64 current);
	inline std::string get_hash();
	~sha256();
};
inline uint32_t sha256::bin_str_to_int32(std::string data) {
	std::bitset<32> a(data);
	return a.to_ulong();
}
inline uint32_t sha256::hex_int_to_dec_int(int32 data) {
	std::stringstream x;
	x << std::hex << data;
	int32 y;
	x >> y;
	return y;
}
inline int32 sha256::rotr(int32 data, unsigned m) {
	data = data >> m | data << (32 - m);
	return data;
}
inline int32 sha256::shr(int32 data, unsigned m) {
	data >>= m;
	return data;
}
inline uint32_t sha256::Csigma1256(int32 data) { return (rotr(data, 6) ^ rotr(data, 11) ^ rotr(data, 25)); }
inline uint32_t sha256::Csigma0256(int32 data) { return (rotr(data, 2) ^ rotr(data, 13) ^ rotr(data, 22)); }
inline uint32_t sha256::sigma1256(int32 data) { return (rotr(data, 17) ^ rotr(data, 19) ^ shr(data, 10)); }
inline uint32_t sha256::sigma0256(int32 data) { return (rotr(data, 7) ^ rotr(data, 18) ^ shr(data, 3)); }
inline uint32_t sha256::ch(int32 a, int32 b, int32 c) { return (a & b) ^ (~a & c); }
inline uint32_t sha256::maj(int32 a, int32 b, int32 c) { return ((a & b) ^ (a & c) ^ (b & c)); }
inline void sha256::assignhex(int32& a, int32& b) {
	std::bitset<32> x(b);
	int32 y = bin_str_to_int32(x.to_string());
	a = y;
}
sha256::sha256(string data) {
	binary_data = "1";
	int current = data.length() * 8;
	int x = (448 - 1 - data.length() * 8) % 512;
	while (x < 0)
		x += 512;
	std::string temp(x, '0');
	binary_data += temp;
	binary_data += std::bitset<64>(current).to_string();
	int r = binary_data.length();
	current = ((data.length() * 8) + binary_data.length()) / 512;
	int32** m = new int32 * [current];
	for (int i = 0; i < current; i++)
		m[i] = new int32[16];
	if ((data.length() * 8) % 32 != 0) {
		if ((data.length() * 8 + 24) % 32 == 0) {
			unsigned char i = std::bitset<8>(binary_data.substr(0, 8)).to_ulong();
			unsigned char j = std::bitset<8>(binary_data.substr(8, 8)).to_ulong();
			unsigned char k = std::bitset<8>(binary_data.substr(16, 8)).to_ulong();
			data.append(1, static_cast<unsigned char>(i));
			data.append(1, static_cast<unsigned char>(j));
			data.append(1, static_cast<unsigned char>(k));
			binary_data.erase(0, 24);
		}
		else if ((data.length() * 8 + 16) % 32 == 0) {
			unsigned char i = std::bitset<8>(binary_data.substr(0, 8)).to_ulong();
			unsigned char j = std::bitset<8>(binary_data.substr(8, 8)).to_ulong();
			data.append(1, static_cast<unsigned char>(i));
			data.append(1, static_cast<unsigned char>(j));
			binary_data.erase(0, 16);
		}
		else {
			unsigned char i = std::bitset<8>(binary_data.substr(0, 8)).to_ulong();
			data.append(1, static_cast<unsigned char>(i));
			binary_data.erase(0, 8);
		}
	}
	int n = data.length();
	int j = 0;
	int k = 0;
	for (int i = 0; i < n; i += 4, j++) {
		if (j == 16) {
			j = 0;
			k++;
		}
		m[k][j] = std::bitset<32>(std::bitset<8>(data[i]).to_string() + std::bitset<8>(data[i + 1]).to_string() + std::bitset<8>(data[i + 2]).to_string() + std::bitset<8>(data[i + 3]).to_string()).to_ulong();
	}
	n = binary_data.length();
	for (int i = 0; i < n; i += 32, j++) {
		if (j == 16) {
			j = 0;
			k++;
		}
		m[k][j] = bin_str_to_int32(binary_data.substr(i, 32));
	}
	result = compute(m, current);
	for (int i = 0; i < current; i++)
		delete[] m[i];
	delete[] m;
}
sha256::~sha256() { }
inline std::string sha256::compute(int32** m, int64 current) {
	int32 newhash[8];
	for (int64 i = 0; i < current; i++) {
		for (int64 j = 0; j < 8; j++)
			newhash[j] = hash[j];
		int32 w[64];
		for (int64 j = 0; j < 16; j++) {
			w[j] = m[i][j];
		}
		for (int64 j = 16; j < 64; j++)
			w[j] = (sigma1256(w[j - 2]) + w[j - 7] + sigma0256(w[j - 15]) + w[j - 16]);
		for (int64 j = 0; j < 64; j++) {
			int32 t1 = (newhash[7] + Csigma1256(newhash[4]) + ch(newhash[4], newhash[5], newhash[6]) + k[j] + w[j]);
			int32 t2 = (Csigma0256(newhash[0]) + maj(newhash[0], newhash[1], newhash[2]));
			newhash[7] = newhash[6];
			newhash[6] = newhash[5];
			newhash[5] = newhash[4];
			newhash[4] = (newhash[3] + t1);
			newhash[3] = newhash[2];
			newhash[2] = newhash[1];
			newhash[1] = newhash[0];
			newhash[0] = (t1 + t2);
		}
		for (int64 j = 0; j < 8; j++) {
			hash[j] = (hash[j] + newhash[j]);
		}
	}
	std::string final = "";
	std::stringstream x;
	for (int64 i = 0; i < 8; i++) {
		x << std::hex << hash[i];
		std::string temp = x.str();
		if (temp.length() < 8) {
			while (temp.length() != 8)
				temp = "0" + temp;
		}
		final += temp;
		x.str(std::string());
	}
	return final;
}
inline std::string sha256::get_hash() { return result; };

#include <sstream>
#include <condition_variable>
#include <future>
#include "blockchain.h"
#include <vector>
#include "json.h"
#include "sha256.h"
using namespace std;
using nlohmann::json;
condition_variable wait_point;
void blockchain::addBlock(Block& currBlock) {
	chain.push_back(currBlock);
	if(printMode == 0)
		print(chain);
}
string blockchain::calculate_bitoin_hash(Block &block, int id) {
	string hash_hex_str_0, hash_hex_str_1, src_str = to_string(block.index) + block.timestamp + block.data + block.prev_hash + block.nonce;
	sha256 obj(src_str);
	hash_hex_str_0 = obj.get_hash();
	sha256 obj1(hash_hex_str_0);
	hash_hex_str_1 = obj1.get_hash();
	return hash_hex_str_1;
}
bool blockchain::starts_with(std::string mainStr, std::string toMatch) {
	if (mainStr.find(toMatch) == 0)
		return true;
	else
		return false;
}
bool blockchain::is_hash_valid(string hash, int difficulty) {
	string prefix = "";
	while (difficulty--)
		prefix += "0";
	return starts_with(hash, prefix);
}
bool blockchain::is_block_valid(Block new_block, Block old_block) {
	if (old_block.index + 1 != new_block.index)
		return false;
	if (old_block.hash != new_block.prev_hash)
		return false;
	if (calculate_bitoin_hash(new_block, thread::hardware_concurrency()) != new_block.hash)
		return false;
	return true;
}
void blockchain::replace_chain(vector<Block> &new_blockchain) {
	if (new_blockchain.size() > chain.size()) {
		chain.clear();
		chain = new_blockchain;
	}
}
void blockchain::nonce_threaded(int index, string data, string prev_hash, string timestamp, int id) {
	while (!found) {
		mtx.lock();
		int x = range[id].first;
		int y = range[id].second;
		mtx.unlock();
		for (int i = x; i <= y; i++) {
			if (!found) {
				stringstream temp2;
				temp2 << hex << i;
				Block temp_block = {
					index, difficulty, "", prev_hash, "", data, timestamp
				};
				temp_block.nonce = temp2.str();
				std::string tempx = calculate_bitoin_hash(temp_block, id);
				if (!is_hash_valid(tempx, temp_block.difficulty)) {
					continue;
				}
				else {
					lock_guard<mutex> guard(inside);
					found = true;
					temp.hash = tempx;
					temp.nonce = temp2.str();
					wait_point.notify_one();
					return;
				}
			}
			else
				return;
		}
		lock_guard<mutex> guard(mtx);
		int largest = range[0].first;
		int i = 1;
		for (i; i < range.size(); i++) {
			if (range[i].first > largest)
				largest = range[i].first;
		}
		range[id].first = largest + 8000;
		range[id].second = range[id].first + 4000;
	}
}
bool blockchain::is_ready() {
	return found;
}
Block blockchain::generate_block(Block old_block, string data) {
	Block new_block;
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	new_block.index = old_block.index + 1;
	new_block.timestamp = to_string(ms.count());
	new_block.data = data;
	new_block.prev_hash = old_block.hash;
	new_block.difficulty = difficulty;
	vector<std::future<void>> threads;
	temp = new_block;
	found = false;
	range.push_back(make_pair(0, 4000));
	threads.emplace_back(async(&blockchain::nonce_threaded, this, new_block.index, data, new_block.prev_hash, new_block.timestamp, 0));
	unsigned int conc = thread::hardware_concurrency() - 1;
	for (unsigned int x = 1; x < conc; x++) {
		range.push_back(make_pair(range[x - 1].first + 4000, range[x - 1].second + 4000));
		threads.emplace_back(async(&blockchain::nonce_threaded, this, new_block.index, data, new_block.prev_hash, new_block.timestamp, x));
	}
	for (auto&x : threads)
		x.get();
	unique_lock<mutex> mlock(inside);
	wait_point.wait(mlock, bind(&blockchain::is_ready, this));
	current++;
	return temp;
}
void blockchain::print(vector<Block> &blockchain) {
	json result;
	result["index"] = blockchain[blockchain.size() - 1].index;
	result["data"] = blockchain[blockchain.size() - 1].data;
	result["difficulty"] = blockchain[blockchain.size() - 1].difficulty;
	result["nonce"] = blockchain[blockchain.size() - 1].nonce;
	result["timestamp"] = blockchain[blockchain.size() - 1].timestamp;
	result["hash"] = blockchain[blockchain.size() - 1].hash;
	result["prevhash"] = blockchain[blockchain.size() - 1].prev_hash;
	cout << result.dump(8) << endl;

}
stringstream blockchain::dumpChainAsJson() {
	json result;
	stringstream chainJSON;
	for (auto x : chain) {
		result["index"] = x.index;
		result["data"] = x.data;
		result["difficulty"] = x.difficulty;
		result["nonce"] = x.nonce;
		result["timestamp"] = x.timestamp;
		result["hash"] = x.hash;
		result["prevhash"] = x.prev_hash;
		chainJSON << result.dump() << endl;
	}
	return chainJSON;
}
void blockchain::handleGenesisBlock(Block &genesis) {
	difficulty = genesis.difficulty;
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	genesis.timestamp = to_string(ms.count());
	range.push_back(make_pair(0, 4000));
	nonce_threaded(genesis.index, genesis.data, genesis.prev_hash, genesis.timestamp, 0);
	genesis.hash = temp.hash;
	genesis.nonce = temp.nonce;
}
void blockchain::handleWriteBlock(string data) {
	difficulty = chain[chain.size() - 1].difficulty;
	range.clear();
	Block new_block = generate_block(chain[chain.size() - 1], data);
	//print(chain);
	if (is_block_valid(new_block, chain[chain.size() - 1])) {
		vector<Block> new_blockchain(chain);
		new_blockchain.push_back(new_block);
		replace_chain(new_blockchain);
		print(new_blockchain);
	}
}
Block blockchain::getLastBlock() {
	return chain[chain.size() - 1];
}
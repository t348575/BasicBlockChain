#include <vector>
using namespace std;
struct Block {
	int index, difficulty = 0;
	string hash, prev_hash, nonce, data, timestamp;
};
class blockchain {
	public:
		void handleWriteBlock(string data);
		void handleGenesisBlock(Block& genesis);
		stringstream dumpChainAsJson();
		Block getLastBlock();
		blockchain(Block &genesis) {
			chain.push_back(genesis);
			print(chain);
		}
		void addBlock(Block& currBlock);
		string calculate_bitoin_hash(Block &block, int id);
		blockchain() { }
		void setPrintMode(int mode) {
			this->printMode = mode;
		}
	private: 
		int printMode = 0;
		bool is_ready();
		vector<Block> chain;
		Block temp;
		mutex mtx, inside;
		const int num_threads = thread::hardware_concurrency();
		void nonce_threaded(int index, string data, string prev_hash, string timestamp, int id);
		int difficulty;
		bool found = false;
		vector<pair<int, int>> range;
		string nonce, hash;
		int current=0;
		void print(vector<Block> &blockchain);
		bool starts_with(std::string mainStr, std::string toMatch);
		bool is_hash_valid(string hash, int difficulty);
		bool is_block_valid(Block new_block, Block old_block);
		void replace_chain(vector<Block> &new_blockchain);
		Block generate_block(Block old_block, string data);
};
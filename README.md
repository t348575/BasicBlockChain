# BasicBlockChain

 A very basic blockchain written in c++
  - Multithreaded 
  - Entirely written in C++
  - Save entire blockchain to file
  
  # Building
   ### Linux  
   `make`
    
  # Usage
   `./blockchain [Arguments...]`
   
   ### -interactive
   Runs the blockchain manager in interactive mode, where all the data is fed manually on request  
   
   ### -index
   Index of the new data in the chain  
    `-index 1`
    
   ### -data
   Data held in the genesis, ie. first block in the chain  
    `-data genesisblockdata`
    
   ### -newdata
   Data for the next block in the chain  
    `-newdata secondblockdata`
    
   ### -difficulty
   Difficulty of the generated hash's ( number of 0's from the start of the hash )  
    `-difficulty 3`
   
   ### -hash
   Hash for the genesis block  
    `-hash 00c92de90fe764a9b18179165183fa7020520c986d9154f43d2ad4115195829e`
   
   ### -nonce
   Nonce used to accquire the genesis hash in hex  
    `-nonce 9F1`
    
   ### -prevhash
   Previous hash for the genesis block  
    `-prevhash e41193b1e77a9b744ad46254d13008a17453e5e64f55107b096d09f121cbfdea`
   
   ### -timestamp
   Milliseconds since epoch at which the genesis block was created  
    `-timestamp 1580691002000`

# mini-blockchain-1
a simple representation of a blockchain using pure SHA256 on C language

this is the mini blockchain that represent the blockchain fundamental itself with the feature of adding the Block with the data and chaining it together with the previous block,
Every block contains the hash of the previous block, hash of the current block, timestamp, and the data of the block that we input.
![alt text](https://github.com/kumisaku/mini-blockchain-1/blob/main/img/block.png)

#### In this project, there is two main option that the user can select by inputing the option number:
## 1. Adding the block
When the user input a data formatted as a string, this will process the data to the block. The data will be hashed using SHA256 and 
There will be 2 conditions after that:
1. Best case is when the block is the genesis block, there will be no data of the previous hash of a block, so it will be set to 0000.
![alt text](https://github.com/kumisaku/mini-blockchain-1/blob/main/img/genesis.png)

3. The normal case is when the new block has been hashed, the block will store the hash of the previous hash of the block.
![alt text](https://github.com/kumisaku/mini-blockchain-1/blob/main/img/process.png)

## 2. Printing all the blocks
The printing can happen because there is actually a pointer that points to the latest block added. The pointer will traverse to the previous block and print the user input data inside the block 
with the additional data like timestamps, current hash of a block, and the previous hash of a block. 
![alt text](https://github.com/kumisaku/mini-blockchain-1/blob/main/img/print.png)

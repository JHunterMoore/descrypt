#include<fstream>
#include<iostream>

using namespace std;

unsigned char* keyGeneration(unsigned char[]);
unsigned char* keyShift(unsigned char[], int);
unsigned char* keyCompression(unsigned char[]);
void initialPermutation(unsigned char[]);
void finalPermutation(unsigned char[]);
unsigned char* dataExpansion(unsigned char[]);
unsigned char* XORWhiten( unsigned char[], unsigned char[]);
unsigned char* sBoxPermutation(unsigned char[]);
unsigned char* pPermutation(unsigned char[]);
void printBin(unsigned char[], int);
void printBinStraight(unsigned char[], int);

const unsigned char bitValue[] = {128, 64, 32, 16, 8, 4, 2, 1};

const int key_shift_sizes[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

const int keyCompressionPosition[] =  {13, 16, 10, 23,  0,  4,  2, 27,
					14,  5, 20,  9, 22, 18, 11,  3,
					25,  7, 15,  6, 26, 19, 12,  1,
					40, 51, 30, 36, 46, 54, 29, 39,
					50, 44, 32, 47, 43, 48, 38, 55,
					33, 52, 45, 41, 49, 35, 28, 31};


const unsigned char SBOX[][8][16] = {	{{14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
					{0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
					{4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
					{15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}},

				{{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
				{3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
				{0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
				{13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}},

				{{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
				{13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
				{13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
				{1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}},

				{{7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
				{13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
				{10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
				{3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}},

				{{2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
				{14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
				{4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
				{11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}},

				{{12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
				{10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
				{9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
				{4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}},

				{{4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
				{13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
				{1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
				{6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}},

				{{13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
				{1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
				{7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
				{2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}}};

const int P_PERMUTATION[] = {	15,  6, 19, 20, 28, 11, 27, 16,
				0, 14, 22, 25, 4, 17, 30, 9,
				1,  7, 23, 13, 31, 26,  2,  8,
				18, 12, 29,  5, 21, 10, 3, 24};

int main( int argc, char* argv[] )
{
	unsigned char 	key[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char*	generatedKey;

	ifstream	inputFile;
	ofstream	outputFile;

	bool		programFailure = false;
	int		numOfBlocks, fileSize;

	if(argc < 3 || argc > 4)
	{
		programFailure = true;
		cout << "Improper Arguments! follow: desencrypt [input file] [keyword] <output file>" << endl;
	}
	else
	{
		if(argv[2][8] >= '1' || argv[2][7] < '1')
		{
			cout << "Keyword must be exactly 8 characters long" << endl;
			programFailure = true;
		}
		else
		{
			for(int i = 0; i < 8; i++)
				key[i] = (unsigned char)(argv[2][i]);

			inputFile.open(argv[1]);
			if(inputFile.fail())
			{
				cout << "Input file could not be opened." << endl;
				programFailure = true;
			}
			else
			{
				if(argc == 4)
				{
					outputFile.open(argv[3]);
					if(outputFile.fail())
					{
						cout << "Output file could not be opened." << endl;
						programFailure = true;
					}
				}
				else
				{
					char defaultOutputFile[256];
					char extension[] = ".des";
					int i = 0;
					while(argv[1][i] != 0)
					{
						defaultOutputFile[i] = argv[1][i];
						i++;
					}
					for(int j = 0; j < 4; j++)
					{
						defaultOutputFile[i] = extension[j];
						i++;
					}

					while(i < 256)
						defaultOutputFile[i++] = 0;

					outputFile.open(defaultOutputFile);

					if(outputFile.fail())
					{
						cout << "Output file could not be opened." << endl;
						programFailure = true;
					}
				}
			}
		}
	}

	if(!programFailure)
	{
		inputFile.seekg(0, inputFile.end);
		fileSize = inputFile.tellg();
		numOfBlocks = ((fileSize-1)/8);
		inputFile.seekg(0, inputFile.beg);

		for(int k = 0; k <= numOfBlocks; k++)
		{
			generatedKey = keyGeneration(key);
			unsigned char* currentBlock = new unsigned char[8];

			if(k == numOfBlocks)
			{
				for(int i = 0; i < 8; i++)
				{
					if(inputFile.tellg() < fileSize)
						inputFile.read((char*)(currentBlock+i), 1);
					else
						currentBlock[i] = 0;
				}
			}
			else
				inputFile.read((char*)currentBlock, 8);

			initialPermutation(currentBlock);

			if(currentBlock[0] != 0)
			{
				for(int i = 0; i < 16; i++)
				{
					generatedKey = keyShift(generatedKey, key_shift_sizes[i]);

					unsigned char* roundKey = keyCompression(generatedKey);

					unsigned char* rightBlock = new unsigned char[4];
					unsigned char* whitenedRightBlock = new unsigned char[6];
					unsigned char* newBlock = new unsigned char[8];

					for(int j = 0; j < 4; j++)
						newBlock[j] = currentBlock[j+4];

					whitenedRightBlock = dataExpansion(currentBlock+4);
					whitenedRightBlock = XORWhiten(whitenedRightBlock, roundKey);
					rightBlock = sBoxPermutation(whitenedRightBlock);
					rightBlock = pPermutation(rightBlock);

					for(int j = 0; j < 4; j++)
						newBlock[j+4] = (rightBlock[j] ^ currentBlock[j]);

					for(int j = 0; j < 8; j++)
						currentBlock[j] = newBlock[j];

					delete rightBlock;
					delete whitenedRightBlock;
					delete newBlock;
					delete roundKey;
				}

				for(int i = 0; i < 4; i++)
				{
					unsigned char temp = currentBlock[i];
					currentBlock[i] = currentBlock[i+4];
					currentBlock[i+4] = temp;
				}

				finalPermutation(currentBlock);

				outputFile.write((char*)currentBlock, 8);
				outputFile.flush();

				delete currentBlock;
			}
		}
	}
	inputFile.close();
	outputFile.close();

	for(int i = 0; i < 8; i++)
		key[i] = 0;

	return 0;
}

unsigned char* keyGeneration(unsigned char key[])
{
	unsigned char* finalKey = new unsigned char[7];
	int currentBit = 0;

	for(int i = 0; i < 7; i++)
		finalKey[i] = 0;

	while(currentBit < 28)								//for first half of key
	{
		for(int i = 8; i > 0; i--)						//go through each byte of the original key from last to first
		{
			if(((key[i-1] & bitValue[currentBit/8]) != 0)&&currentBit<28)			//pick up bit in current position, first to fourth*
				finalKey[currentBit/8] += bitValue[currentBit%8];
			currentBit++;							//increment the bit poisition after each comparison
		}
	}

	currentBit = 28;

	while(currentBit < 52)								//for second half until last 4
	{
		for(int i = 8; i > 0; i--)						//go through each byte of the original key, last to first
		{
			if(((key[i-1] & bitValue[6-((currentBit-28)/8)]) != 0)&&currentBit<52)		//pick up the bit in the current position, seventh to fifth
				finalKey[currentBit/8] += bitValue[currentBit%8];
			currentBit++;							//increment the bit position after each comparison
		}
	}

	currentBit = 52;

	//last 4 bits
	for(int i = 4; i > 0; i--)							//for last 4 bits
	{
		if((key[i-1] & bitValue[3]) != 0)					//pick up from the fourth bit, from fourth to first byte
			finalKey[currentBit/8] += bitValue[currentBit%8];
		currentBit++;								//increment the position
	}

	//1  2  3  4  5  6  7  8
	//9  10 11 12 13 14 15 16
	//17 18 19 20 21 22 23 24
	//25 26 27 28 29 30 31 32
	//33 34 35 36 37 38 39 40
	//41 42 43 44 45 46 47 48
	//49 50 51 52 53 54 55 56
	//57 58 59 60 61 62 63 64

	//57 49 41 33 25 17 9  1
	//58 50 42 34 26 18 10 2
	//59 51 43 35 27 19 11 3
	//60 52 44 36 63 55 47 39
	//31 23 15 7  62 54 46 38
	//30 22 14 6  61 53 45 37
	//29 21 13 5  28 20 12 4

	return finalKey;
}

unsigned char* keyShift(unsigned char* key, int shiftDist)
{
	unsigned char leftHalf[] = {0, 0, 0, 0};
	unsigned char rightHalf[] = {0, 0, 0, 0};
	unsigned char* newKey = new unsigned char[7];

	for(int i = 0; i < 7; i++)
		newKey[i] = 0;

	//00000000 11111111 22222222 3333/3333 44444444 55555555 66666666
	//00000000 11111111 22222222 3333XXXX / XXXX3333 44444444 55555555 66666666
	//XX000011 11111122 22222233 333300XX / XX663333 44444444 55555555 666666XX

	//00000111 11111222 22222333 3330|6333 34444444 45555555 56666666
	//00001111 11112222 22223333 3300|6633 33444444 44555555 55666666

	for(int i = 0; i < 3; i++)
	{
		leftHalf[i] = key[i];
		rightHalf[3-i] = key[6-i];
	}

	rightHalf[0] = (key[3]) & 0b00001111;
	leftHalf[3] = (key[3]) & 0b11110000;

	int currentBit = 0;

	while(currentBit < 28)
	{
		if((leftHalf[((currentBit+shiftDist)%28)/8] & bitValue[((currentBit+shiftDist)%28)%8])!=0)
			newKey[currentBit/8] += bitValue[currentBit%8];
		currentBit++;
	}


	while(currentBit < 56)
	{
		if((rightHalf[(((currentBit+shiftDist)%28)+4)/8] & bitValue[(((currentBit+shiftDist)%28)+4)%8])!=0)
			newKey[currentBit/8] += bitValue[currentBit%8];
		currentBit++;
	}


	return newKey;
}

unsigned char* keyCompression(unsigned char* key)
{
	unsigned char* compressedKey = new unsigned char[6];

	for( int i = 0; i < 6; i++)
		compressedKey[i] = 0;

	for( int i = 0; i < 48; i++)
	{
		int sourceBit = keyCompressionPosition[i];
		if( (key[sourceBit/8] & bitValue[sourceBit%8]) != 0)
			compressedKey[i/8] += bitValue[i%8];
	}

	return compressedKey;
}


void initialPermutation(unsigned char block[])
{
	unsigned char permutedBlock[] = {0, 0, 0, 0,
				      0, 0, 0, 0};

	for(int j = 0; j < 4; j++)					//for first 4 bytes
	{
		for(int i = 0; i < 8; i++)				//get each bit by...
		{
			if((block[7-i] & bitValue[2*j+1]) != 0)			//checking for a bit in the key at the correlating column
				permutedBlock[j] += bitValue[i];	//and adding one byte to the end if so
		}
	}

	for(int j = 0; j < 4; j++)					//same process for last for bytes
	{
		for(int i = 0; i < 8; i++)
		{
			if((block[7-i] & bitValue[2*j]) != 0)
				permutedBlock[j+4] += bitValue[i];
		}
	}


	for(int i = 0; i < 8; i++)
		block[i] = permutedBlock[i];

	return;
}

void finalPermutation(unsigned char block[])
{
	unsigned char permutedBlock[] = {0, 0, 0, 0,
				       0, 0, 0, 0};

	for(int i = 0; i < 4; i++)					//for each even column
	{
		for(int j =0; j < 8; j++)
		{
			if((block[i]&bitValue[j]) != 0)
				permutedBlock[7-j] += bitValue[2*i+1];
		}
	}

	for(int i = 0; i < 4; i++)					//same process for odd bits and the last 4 bytes of the original
	{
		for(int j = 0; j < 8; j++)
		{
			if((block[i+4]&bitValue[j]) != 0)
				permutedBlock[7-j] += bitValue[2*i];
		}
	}
	for(int i = 0; i < 8; i++)					//pass the permuted block to the original
		block[i] = permutedBlock[i];
	return;
}

unsigned char* dataExpansion(unsigned char dataBlock[])
{
	unsigned char* expandedBlock = new unsigned char[6];
	int currentSourceBit = 31;
	int currentTargetBit = 0;

	for(int i = 0; i < 6; i++)
		expandedBlock[i] = 0;

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 6; j++)
		{
			if( (dataBlock[currentSourceBit/8] & bitValue[currentSourceBit%8]) != 0 )
				expandedBlock[currentTargetBit/8] += bitValue[currentTargetBit%8];

			currentSourceBit = (currentSourceBit + 1)%32;
			currentTargetBit++;
		}

		currentSourceBit = (currentSourceBit - 2)%32;
	}

	return expandedBlock;
}

unsigned char* XORWhiten( unsigned char expandedBlock[], unsigned char compressedKey[])
{
	unsigned char* whitenedBlock = new unsigned char[6];

	for(int i = 0; i < 6; i++)
		whitenedBlock[i] = expandedBlock[i] ^ compressedKey[i];

	return whitenedBlock;
}

unsigned char* sBoxPermutation(unsigned char whitenedBlock[])
{
	unsigned char* finalBlock = new unsigned char[4];				//to hold the result of the SBox permutation
	int currentBit = 0;								//index for bits

	for(int i = 0; i < 4; i++)
		finalBlock[i] = 0;

	for(int i = 0; i < 8; i++)							//go through each half of each byte of the whitened block
	{
		unsigned char currentSixBits = 0;					//holder of the 6 bits
		int rowPosition = 0;							//index of SBox row
		int colPosition = 0;							//index of SBox col

		for(int j = 0; j < 6; j++)
		{
			if( (whitenedBlock[currentBit/8] & bitValue[currentBit%8]) !=0 )	//add each of 6 bits to the holder
				currentSixBits += bitValue[j];
			currentBit++;								//increment bit index
		}

		//ROW SELECTION
		if( (currentSixBits & bitValue[5]) != 0 ) 				//calculate row position
			rowPosition += 1;

		if( (currentSixBits & bitValue[0]) != 0 )
			rowPosition += 2;

		//COL SELECTION
		for(int j = 0; j < 4; j++)						//calculate col position
		{
			if( (currentSixBits & bitValue[j+1]) != 0 )
				colPosition += bitValue[j+4];
		}

		if((i%2) == 0)
			finalBlock[i/2] += SBOX[i][rowPosition][colPosition] << 4;		//add the low bits on even iterations
		else
			finalBlock[i/2] += SBOX[i][rowPosition][colPosition];	//add the high bits on odd iterations
	}

	return finalBlock;
}

unsigned char* pPermutation(unsigned char sPermutedBlock[])
{
	unsigned char* pPermutedBlock = new unsigned char[4];

	for(int i = 0; i < 4; i++)
		pPermutedBlock[i] = 0;

	for(int i = 0; i < 32; i++)
	{
		if( (sPermutedBlock[P_PERMUTATION[i]/8] & bitValue[P_PERMUTATION[i]%8]) != 0 )
			pPermutedBlock[i/8] += bitValue[i%8];

	}

	return pPermutedBlock;
}

void printBin(unsigned char str[], int size)
{
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if((str[i] & bitValue[j]) != 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;
	}
	cout << endl;
	return;
}

void printBinStraight(unsigned char str[], int size)
{
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if((str[i] & bitValue[j]) != 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << " ";
	}
	cout << endl << endl;
	return;
}




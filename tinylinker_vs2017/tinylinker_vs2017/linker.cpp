#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

#include "linker.h"


FILE *in;   //for read module
FILE *out;  //for output link result

/*offset of the current module
  assume current module is I+1, the offset of I+1
  will be LENGTH(0) + LENGTH(1) + ...... LENGTH(I)
  where the LENGTH(I) represents the length of module I
*/
int g_offset = 0;

/*you can modify this function!*/

/*
  test_num: which file test, it must between 1 and 9
  filename: the file that you should do with
*/
void link(int testNum, const char *filename)
{
	char outputfile[128];
	memset(outputfile, 0, 128);
	sprintf(outputfile, "output-%d.txt", testNum);
	in = fopen(filename, "r");    //open file for read,the file contains module that you should do with
	out = fopen(outputfile, "w"); //open the "output" file to output link result
	if (in == NULL || out == NULL)
	{
		fprintf(stderr, "can not open file for read or write\n");
		exit(-1);
	}
	processOne(); //resolve
	processTwo(); //relocate
	fclose(in);
	fclose(out);

}

////////////////////////////////////////////////////////////////////////////////

struct ListItem {
	std::string variableName;
	int offset;
	ListItem(char *variableName, int offset) {
		this->variableName = variableName;
		this->offset = offset;
	}
};

struct TextItem {
	char addressType;
	int word;
	TextItem(char addressType, int word) {
		this->addressType = addressType;
		this->word = word;
	}
};

struct ObjectFile {
	std::vector<ListItem> definitionList;
	std::vector<ListItem> useList;
	std::vector<TextItem> programText;
};

struct SymbolItem {
	bool isDuplicated;
	bool isUsed;
	bool isOutsideModule;
	int globalOffset;
	int definedModule;
	SymbolItem() {
		this->isDuplicated = false;
		this->isUsed = false;
		this->isOutsideModule = false;
	}
	SymbolItem(int globalOffset, int definedModule) {
		new (this) SymbolItem();
		this->globalOffset = globalOffset;
		this->definedModule = definedModule;
	}
};

struct MapItem {
	bool isNotDefined;
	bool isOnChain;
	bool isOutsideModule;
	char originalType;
	int word;
	std::string variableName;
	MapItem() {
		this->isOnChain = false;
		this->isNotDefined = false;
		this->isOutsideModule = false;
	}
	MapItem(char originalType, int word) {
		new (this) MapItem();
		this->originalType = originalType;
		this->word = word;
	}
};

////////////////////////////////////////////////////////////////////////////////

std::vector<ObjectFile> g_objectFiles;
std::map<std::string, SymbolItem> g_symbolTable;
std::vector<MapItem> g_memoryMap;

////////////////////////////////////////////////////////////////////////////////

/*
	Print final results to the output file
*/
void printToOutputFile() {
	// print symbol table
	fprintf(out, "Symbol Table\n");
	for (std::map<std::string, SymbolItem>::iterator it = g_symbolTable.begin();
		it != g_symbolTable.end();
		it++) {
		fprintf(out, "%s=%d", it->first.c_str(), it->second.globalOffset);
		if (it->second.isOutsideModule) {
			fprintf(out, " Error: The value of %s is outside module 2; zero (relative) used\n", it->first.c_str());
		}
		else if (it->second.isDuplicated) {
			fprintf(out, " Error: This variable is multiply defined; first value used.\n");
		}
		else {
			fprintf(out, "\n");
			
		}
	}

	fprintf(out, "\n");

	// print memory map
	fprintf(out, "Memory Map\n");
	for (int i = 0; i < g_memoryMap.size(); i++) {
		// print
		if (i < 10) { 
			fprintf(out, "%d:  %d", i, g_memoryMap[i].word);
		}
		else {
			fprintf(out, "%d: %d", i, g_memoryMap[i].word);
		}

		// handle errors
		if (g_memoryMap[i].isNotDefined == true) {
			fprintf(out, " Error: %s is not defined; zero used.\n", g_memoryMap[i].variableName.c_str());
		}
		else if (g_memoryMap[i].isOutsideModule == true) {
			fprintf(out, " Error: Pointer in use chain exceeds module size; chain terminated.\n");
		}
		else if (g_memoryMap[i].originalType == 'E' && g_memoryMap[i].isOnChain == false) {
			fprintf(out, " Error: E type address not on use chain; treated as I type.\n");
		}
		else if (g_memoryMap[i].originalType != 'E' && g_memoryMap[i].isOnChain == true) {
			fprintf(out, " Error: %c type address on use chain; treated as E type.\n", g_memoryMap[i].originalType);
		}
		else {
			fprintf(out, "\n");
		}
	}

	fprintf(out, "\n");

	for (std::map<std::string, SymbolItem>::iterator it = g_symbolTable.begin();
		it != g_symbolTable.end();
		it++) {
		if (!it->second.isUsed) {
			fprintf(out, "Warning: %s was defined in module %d but never used.\n", 
				it->first.c_str(), it->second.definedModule);
		}
	}
}

/*
	Process 1
	Missions:
	  - Store all information
	  - Generate symbol table
*/
void processOne()
{
	int fileIndex = 0;
	int itemsNum;
	char *variableName = new char[80];
	int i;
	bool isEnd = false;

	while (!isEnd) {
		// get number of definitions if not EOF
		if (fscanf(in, "%d", &itemsNum) == EOF) {
			isEnd = true;
			break;
		}

		// new file
		fileIndex++;
		g_objectFiles.push_back(ObjectFile());

		for (i = 0; i < itemsNum; i++) {
			// insert definitions
			fscanf(in, "%s", variableName);
			int innerOffset;
			fscanf(in, "%d", &innerOffset);
			g_objectFiles.back().definitionList.push_back(ListItem(variableName, innerOffset));

			// fill in symbol table
			if (g_symbolTable.find(variableName) == g_symbolTable.end()) { // do not find duplication
				g_symbolTable.insert(
					std::pair<std::string, SymbolItem>(
						variableName, SymbolItem(g_offset + innerOffset, fileIndex)));
			}
			else {
				g_symbolTable[variableName].isDuplicated = true;
			}
		}

		// number of uses
		fscanf(in, "%d", &itemsNum);
		for (i = 0; i < itemsNum; i++) {
			// insert uses
			fscanf(in, "%s", variableName);
			int offset;
			fscanf(in, "%d", &offset);
			g_objectFiles.back().useList.push_back(ListItem(variableName, offset));
		}

		// number of instructions
		fscanf(in, "%d", &itemsNum);
		for (i = 0; i < itemsNum; i++) {
			char addressType;
			bool isTypeValid = false;
			// insert instructions
			do {
				fscanf(in, "%c", &addressType);
				if (addressType >= 'A' && addressType <= 'Z') {
					isTypeValid = true;
				}
			} while (!isTypeValid);
			int word;
			fscanf(in, "%d", &word);
			g_objectFiles.back().programText.push_back(TextItem(addressType, word));
		}

		// check if definition of variables are outside the module
		for (int k = 0; k < g_objectFiles.back().definitionList.size(); k++) {
			std::string variableName = g_objectFiles.back().definitionList[k].variableName;
			if (g_objectFiles.back().definitionList[k].offset 
					>= g_objectFiles.back().programText.size()
					&& g_symbolTable[variableName].isDuplicated == false) {
				g_symbolTable[variableName].isOutsideModule = true;
				// reset address to zero (relative)
				g_symbolTable[variableName].globalOffset = g_offset;
			}
		}

		// update global offset
		g_offset += itemsNum;
	}

	delete[] variableName;
}

/*
	Process 2
	Missions:
	  - Relocation
	  - Print result
*/
void processTwo()
{
	g_offset = 0;

	// relocation
	for (int i = 0; i < g_objectFiles.size(); i++) {
		std::vector<ListItem> &definitionList = g_objectFiles[i].definitionList;
		std::vector<ListItem> &useList = g_objectFiles[i].useList;
		std::vector<TextItem> &programText = g_objectFiles[i].programText;

		// construct memory map
		for (int k = 0; k < programText.size(); k++) {
			g_memoryMap.push_back(MapItem());
			g_memoryMap[g_offset + k].originalType = programText[k].addressType;
			
			if (programText[k].addressType == 'R') {
				g_memoryMap[g_offset + k].word = g_offset + programText[k].word;
			}
			else { // I or A or E(bad type if not on use chain)
				g_memoryMap[g_offset + k].word = programText[k].word;
			}
		}

		for (int k = 0; k < useList.size(); k++) {
			for (int currentRefIndex = useList[k].offset; // first reference index
					currentRefIndex != 777;
					currentRefIndex = programText[currentRefIndex].word % 1000) {
				// handle errors
				std::map<std::string, SymbolItem>::iterator tryFindSymbol = g_symbolTable.find(useList[k].variableName);
				if (tryFindSymbol == g_symbolTable.end()) {
					g_memoryMap[g_offset + currentRefIndex].isNotDefined = true;
					g_memoryMap[g_offset + currentRefIndex].word = 1000;
					g_memoryMap[g_offset + currentRefIndex].variableName = useList[k].variableName;
					continue;
				}
				else {
					// now the symbol is used
					tryFindSymbol->second.isUsed = true;
				}

				// update memory map
				g_memoryMap[g_offset + currentRefIndex].originalType = programText[currentRefIndex].addressType;
				g_memoryMap[g_offset + currentRefIndex].word =
					programText[currentRefIndex].word / 1000 * 1000 +
					g_symbolTable[useList[k].variableName].globalOffset;
				g_memoryMap[g_offset + currentRefIndex].isOnChain = true;

				if (programText[currentRefIndex].word % 1000 >= programText.size()
						&& programText[currentRefIndex].word % 1000 != 777) {
					// reference address is outside the module
					g_memoryMap[g_offset + currentRefIndex].isOutsideModule = true;
					break;
				}
			}
		}

		// update global offset
		g_offset += programText.size();
	}

	printToOutputFile();
}

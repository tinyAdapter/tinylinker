# TinyLinker

本项目为 NYU 《操作系统设计》课程的实现代码。

项目要求见`instructions/NYU_lab instruction.pdf`。

(`instructions/SCU_TinyLinker instruction.doc`中有部分中文说明)

说明：在本版 TinyLinker 中，由于需要用到字符串，变长数组和字典等数据结构，而自行实现容易出现 BUG，因此直接套用 C++ STL 库中的 `std::string`, `std::vector`, `std::map` 类。该变化对 VC++ 6.0 下的编译过程无任何影响。

## 数据结构

为组织方便，TinyLinker 中设计了一些数据结构，如下表所示。

| 结构名     | 说明                                                         |
| ---------- | ------------------------------------------------------------ |
| ListItem   | 单个 Definition / Use 中（S, R）键值对的抽象                 |
| TextItem   | 单个 Program Text 中（type, word）对的抽象                   |
| ObjectFile | 单个文件的抽象，包含 Definition List, Use List, Program Text |
| SymbolItem | Symbol Table 中单个 Symbol 的抽象，并记录错误信息            |
| MapItem    | Memory Map 中单个 Word 的抽象，并记录错误信息                |

## 函数过程

整个运行过程如下：

1. `processOne()` 是 Linker 的第一次遍历，用于将全部文件信息存储到文件列表 g_objectFiles 中，形成拓扑，并在存储过程中构造 Symbol Table；
2. `processTwo()` 是 Linker 的第二次遍历，用于解释各个 Program Text，根据 Symbol Table 计算绝对地址并重定位符号位置，最终通过调用 `printToOutputFile()` 将结果一次性打印到 output 文件中。

## 附加需求实现

在项目需求文档中，提出了一系列附加需求。针对这些需求，TinyLinker 给出了解决方案：

### Arbitrary spaces / lines between items.

通过 `fscanf` 指定数据类型，跳过空格和空行，从而正常读取文件。

### If a symbol is multiply-defined, print an error message and use the value given in the first definition.

在第一次遍历构造 Symbol Table 时，插入新 Symbol 前检查该变量是否已经在表中，如果存在，记录错误信息（将 `SymbolItem::isDuplicated` 置为 `true`）。

```cpp
// fill in symbol table
if (g_symbolTable.find(variableName) == g_symbolTable.end()) {
// do not find duplication
	g_symbolTable.insert(
		std::pair<std::string, SymbolItem>(
			variableName, SymbolItem(g_offset + innerOffset, fileIndex)));
}
else {
	g_symbolTable[variableName].isDuplicated = true;
}
```

### If a symbol is used but not defined, print an error message and use the value zero.

在第二次遍历通过 Use List 重定位时，尝试检查变量在 Symbol Table 中是否存在，若不存在，记录错误信息（将 `MapItem::isNotDefined` 置为 `true`）后将 `word` 重置为 1000。

```cpp
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
```

### If a symbol is defined but not used, print a warning message and continue.

参见上方代码，如果变量被使用，则将其 `SymbolItem::isUsed` 置为 `true`。在打印结果时，如果发现某一变量的 `isUsed` 没有被置为 `true`，则输出警告信息。

```cpp
for (std::map<std::string, SymbolItem>::iterator it = g_symbolTable.begin();
	it != g_symbolTable.end();
	it++) {
	if (!it->second.isUsed) {
		fprintf(out, "Warning: %s was defined in module %d but never used.\n",
			it->first.c_str(), it->second.definedModule);
	}
}
```

### If an address appearing in a definition exceeds the size of the module, print an error message and treat the address given as 0 (relative).

在第一次遍历结束时，读取全部 Definition List 中变量并检查是否超出 Program Text 长度，如果出现该类型变量，记录错误信息（将 `SymbolItem::isOutsideModule` 置为 `true`）后将其 `SymbolItem::globalOffset` 置为 `g_offset`（相对地址 0）。

```cpp
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
```

### If an address appearing in a use list exceeds the size of the module, print an error message and treat the address as the sentinel ending the list.

在第二次遍历根据 word 信息跳转到下一个使用变量的地址前，检查其是否超出 Program Text 长度，如果出现该类型变量，记录错误信息（将 `MapItem::isOutsideModule` 置为 `true`）后直接结束该变量的重定位过程。

```cpp
if (programText[currentRefIndex].word % 1000 >= programText.size()
		&& programText[currentRefIndex].word % 1000 != 777) {
	// reference address is outside the module
	g_memoryMap[g_offset + currentRefIndex].isOutsideModule = true;
	break;
}
```

### If an address on a use list is not type E, print an error message and treat the address as type E.

针对每个被重定位的变量，将其 `MapItem::isOnChain` 置为 `true`。
在打印结果时检查，如果变量的 `originalType` 不为 E，但其 `isOnChain` 为 `true`，说明其在 Use List 中，打印错误信息。

```cpp
else if (g_memoryMap[i].originalType != 'E' && g_memoryMap[i].isOnChain == true) {
	fprintf(out, " Error: %c type address on use chain; treated as E type.\n", g_memoryMap[i].originalType);
}
```

### If a type E address is not on a use list, print an error message and treat the address as type I.

同上，在打印结果时检查，如果变量的 `originalType` 为 `E`，但其 `isOnChain` 为 `false`，说明其不在 Use List 中，打印错误信息。

```cpp
else if (g_memoryMap[i].originalType == 'E' && g_memoryMap[i].isOnChain == false) {
	fprintf(out, " Error: E type address not on use chain; treated as I type.\n");
}
```

#include "core.h"
#include <iostream>

// I don't trust inlines.
/*#define alloc_guard(SIZE)														\
	if (memory.maxSize - memory.currentSize < SIZE)								\
		if (tmpPtr = (uint8*)realloc(memory.data, memory.maxSize <<= 1))		\
			memory.data = tmpPtr;												\
		else																	\
			return;*/


namespace Core {
	Module getCore() {
		//if (Core::core != 0)
		//	return *Core::core;
		Module core;
		core.typeId = Table<String, ValueType>({	//typeName : typeID
			{T("int64"), ValueType::int64},
			{T("int32"), ValueType::int32},
			{T("int16"), ValueType::int16},
			{T("int8"), ValueType::int8},
			{T("uint64"), ValueType::uint64},
			{T("uint32"), ValueType::uint32},
			{T("uint16"), ValueType::uint16},
			{T("uint8"), ValueType::uint8},
			{T("float128"), ValueType::float128},
			{T("float64"), ValueType::float64},
			{T("float32"), ValueType::float32},
			{T("string"), ValueType::string},
			{T("name"), ValueType::name},
			{T("array"), ValueType::arr},
			{T("expression"), ValueType::expression},
		});
		core.typeName = Table<ValueType, String>({	//typeID : typeName
			{ValueType::int64, T("int64")},
			{ValueType::int32, T("int32")},
			{ValueType::int16, T("int16")},
			{ValueType::int8, T("int8")},
			{ValueType::uint64, T("uint64")},
			{ValueType::uint32, T("uint32")},
			{ValueType::uint16, T("uint16")},
			{ValueType::uint8, T("uint8")},
			{ValueType::float128, T("float128")},
			{ValueType::float64, T("float64")},
			{ValueType::float32, T("float32")},
			{ValueType::string, T("string")},
			{ValueType::name, T("name")},
			{ValueType::arr, T("array")},
			{ValueType::expression, T("expression")},
		});
		core.typeSize = Table<ValueType, int>({	//typeID : typeSize
			{ValueType::int64, 8},
			{ValueType::int32, 4},
			{ValueType::int16, 2},
			{ValueType::int8, 1},
			{ValueType::uint64, 8},
			{ValueType::uint32, 4},
			{ValueType::uint16, 2},
			{ValueType::uint8, 1},
			{ValueType::float128, 16},
			{ValueType::float64, 8},
			{ValueType::float32, 4},
			{ValueType::string, sizeof(void*)},
			{ValueType::name, sizeof(void*)},
			{ValueType::arr, 0},
			{ValueType::expression, sizeof(void*)},
		});
		core.prefix = Table<String, Table<ValueType, ValueType*>>();
		{
			Table<ValueType, ValueType*>* table;

			table = &core.prefix[T("%")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, test);

			table = &core.prefix[T("*")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, getValueProcedure);

			table = &core.prefix[T(">")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, allArrayInclusive);

			table = &core.prefix[T("^")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, allGroupInclusive);
		}
		core.postfix = Table<String, Table<ValueType, ValueType*>>();
		{
			Table<ValueType, ValueType*>* table;

			table = &core.postfix[T("%")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, test);
			
			table = &core.postfix[T("*")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, getValueProcedure);

			table = &core.postfix[T(">")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, allArrayExclusive);

			table = &core.postfix[T("^")];
			(*table)[ValueType::all] = Instruction::newValue(ValueType::unprocedure, allGroupExclusive);

			table = &core.postfix[T(":")];
			(*table)[ValueType::dict] = Instruction::newValue(ValueType::unprocedure, allContext);
			(*table)[ValueType::name] = Instruction::newValue(ValueType::unprocedure, allContext);

		}
		core.binary = Table<String, Table<ValueTypeBinary, ValueType*>>();
		{
			Table<ValueTypeBinary, ValueType*>* table;

			table = &core.binary[T("")];
			(*table)[ValueTypeBinary(ValueType::uprocedure, ValueType::arr)] = Instruction::newValue(ValueType::uprocedure, invokeFunction);
			(*table)[ValueTypeBinary(ValueType::ufunction, ValueType::arr)] = Instruction::newValue(ValueType::ufunction, invokeFunction);
			(*table)[ValueTypeBinary(ValueType::umethod, ValueType::arr)] = Instruction::newValue(ValueType::umethod, invokeFunction);
			(*table)[ValueTypeBinary(ValueType::unprocedure, ValueType::arr)] = Instruction::newValue(ValueType::unprocedure, invokeFunction);
			(*table)[ValueTypeBinary(ValueType::unfunction, ValueType::arr)] = Instruction::newValue(ValueType::unfunction, invokeFunction);
			(*table)[ValueTypeBinary(ValueType::unmethod, ValueType::arr)] = Instruction::newValue(ValueType::unmethod, invokeFunction);

			table = &core.binary[T("+")];
			(*table)[ValueTypeBinary(ValueType::int64, ValueType::int64)] = Instruction::newValue(ValueType::unprocedure, add); //TODO: make a separate method for each case to save program size

			/*
			core.binary[T("+")] = [] {
				SubroutinePatternMatching* arr = new SubroutinePatternMatching[2];
				//arr[0] = Binary(BinaryType::TypeTypeType | BinaryType::funcNative, &([](int64 l, int64 r) {return l + r;}), ValueType::int64, 0, ValueType::int64, 0, ValueType::int64);
				arr[1] = SubroutinePatternMatching();
				return arr;
			} ();
			core.binary[T(":")] = [] {
				SubroutinePatternMatching* arr = new SubroutinePatternMatching[3];
				//arr[0] = Binary(BinaryType::TypeTypeAll | BinaryType::procNative, getChild, ValueType::dict, 0, ValueType::name, 0, ValueType::all);
				//arr[1] = Binary(BinaryType::TypeTypeAll | BinaryType::procNative, getChild, ValueType::name, 0, ValueType::name, 0, ValueType::all);
				arr[2] = SubroutinePatternMatching();
				return arr;
			} ();
			core.binary[T(",")] = [] {
				SubroutinePatternMatching* arr = new SubroutinePatternMatching[2];
				//arr[0] = Binary(BinaryType::AllAllAll | BinaryType::procNative, comma, ValueType::all, 0, ValueType::all, 0, ValueType::all);
				arr[1] = SubroutinePatternMatching();
				return arr;
			} ();*/
		}

		//Core::core = new Module(core);

		return core;
	}

	void test(Program& program) {
		if (program.stackInstructions[program.stackInstructions.max_index].instr == InstructionType::value) {
			std::cout << program.specification.typeName.at(program.stackInstructions[program.stackInstructions.max_index].value) << std::endl;
			//program.stackInstructions.erase(program.stackInstructions.cend() - 2);
		}

		if (program.stackInstructions[program.stackInstructions.max_index - 2].instr == InstructionType::value) {
			std::cout << program.specification.typeName.at(program.stackInstructions[program.stackInstructions.max_index - 2].value) << std::endl;
			//program.stackInstructions.erase(program.stackInstructions.cend() - 2);
		} else {
			std::cout << program.specification.typeName.at(program.stackInstructions[program.stackInstructions.max_index - 1].value) << std::endl;
			//program.stackInstructions.erase(program.stackInstructions.cend() - 2);
		}
		/*
		program.stackInstructions.erase(program.stackInstructions.cend() - 2);
		switch (program.stackInstructions[program.stackInstructions.size() - 2].value)
		{
		case ValueType::string:
			std::cout << (**(String**)(program.memory.data + program.stackInstructions[program.stackInstructions.size() - 2].shift)).c_str() << std::endl;
		default:
			//std::cout << "Test! " << (int32)program.context.value << "\n";
			std::cout << program.specification.typeName.at(program.stackInstructions[program.stackInstructions.size() - 2].value) << std::endl;
		}*/
	}


	void invokeFunction(Program& program) {
		Instruction parameter = program.stackInstructions.get_r(0);
		Instruction function = program.stackInstructions.get_r(1);
		program.stackInstructions.max_index -= 2;

																									//TODO: add a function to add value
		program.stackInstructions.add(Instruction::call(parameter.value, parameter.shift));	//TODO: make a 'call' instruction that specifies change of executable string
		program.stackInstructions.add(Instruction::context(parameter.value, parameter.shift));


	}

	void invokeNativeProcedure(Program& program) {

	}

#include <stdexcept>
	void add(Program& program) {
		int64 number = 0;
		Instruction left = program.stackInstructions.get_r(2);
		Instruction right = program.stackInstructions.get_r(0);

		switch (right.value) {
		case ValueType::int8:
			number += program.memory.get<int8>(right.shift); program.memory.max_index -= sizeof(int8); break;
		case ValueType::int16:
			number += program.memory.get<int16>(right.shift); program.memory.max_index -= sizeof(int16); break;
		case ValueType::int32:
			number += program.memory.get<int32>(right.shift); program.memory.max_index -= sizeof(int32); break;
		case ValueType::int64:
			number += program.memory.get<int64>(right.shift); program.memory.max_index -= sizeof(int64); break;
		case ValueType::uint8:
			number += program.memory.get<uint8>(right.shift); program.memory.max_index -= sizeof(uint8); break;
		case ValueType::uint16:
			number += program.memory.get<uint16>(right.shift); program.memory.max_index -= sizeof(uint16); break;
		case ValueType::uint32:
			number += program.memory.get<uint32>(right.shift); program.memory.max_index -= sizeof(uint32); break;
		case ValueType::uint64:
			number += program.memory.get<uint64>(right.shift); program.memory.max_index -= sizeof(uint64); break;
		default: throw std::invalid_argument("received negative value");
		}

		program.memory.max_index -= sizeof(void*);

		switch (left.value) {
		case ValueType::int8:
			number += program.memory.get<int8>(left.shift); program.memory.max_index -= sizeof(int8); break;
		case ValueType::int16:
			number += program.memory.get<int16>(left.shift); program.memory.max_index -= sizeof(int16); break;
		case ValueType::int32:
			number += program.memory.get<int32>(left.shift); program.memory.max_index -= sizeof(int32); break;
		case ValueType::int64:
			number += program.memory.get<int64>(left.shift); program.memory.max_index -= sizeof(int64); break;
		case ValueType::uint8:
			number += program.memory.get<uint8>(left.shift); program.memory.max_index -= sizeof(uint8); break;
		case ValueType::uint16:
			number += program.memory.get<uint16>(left.shift); program.memory.max_index -= sizeof(uint16); break;
		case ValueType::uint32:
			number += program.memory.get<uint32>(left.shift); program.memory.max_index -= sizeof(uint32); break;
		case ValueType::uint64:
			number += program.memory.get<uint64>(left.shift); program.memory.max_index -= sizeof(uint64); break;
		default: throw std::invalid_argument("received negative value");
		}

		program.stackInstructions.max_index -= 2;
		program.stackInstructions.at_r(0).value = ValueType::int64;
		program.memory.add<int64>(number);
	}
	void sub(Program& program);
	void mul(Program& program);
	void div(Program& program);

	void addf(Program& program);
	void subf(Program& program);
	void mulf(Program& program);
	void divf(Program& program);


	void getValueProcedure(Program& program) { // Unordered map saves keys by address
		/*uint8* memory = program.memory.data + program.stackInstructions[program.stackInstructions.max_index].shift;
		uint8* value = (uint8*)(program.data.at_r(0)[**(String**)(memory)]);

		int l = program.specification.typeSize[*(ValueType*)value];

		program.stackInstructions[program.stackInstructions.max_index].value = *(ValueType*)value;

		value = (uint8*)((ValueType*)value + 1);

		for (int i = 0; i < l; ++i)
			*(memory + i) = *(value + i);

		program.memory.currentSize -= sizeof(void*);
		program.memory.currentSize += l;

		program.stackInstructions.at_r(1) = program.stackInstructions.at_r(0);
		program.stackInstructions.max_index -= 1;*/
	}

	void allArrayInclusive(Program& program) {

	}

	void allArrayExclusive(Program& program) {

	}

	void allGroupInclusive(Program& program) {

	}

	void allGroupExclusive(Program& program) {

	}

	void allContext(Program& program) {

	}

	void comma(Program& program) {

	}


	void getChild(Program& program) {
		/*String* right = *(String**)(program.memory.data + program.stackInstructions.at_r(0).shift);
		program.stackInstructions.max_index -= 2;
		String* left = *(String**)(program.memory.data + program.stackInstructions.at_r(0).shift);

		auto ptr = program.data.at_r(0)[*left];

		if (*ptr == ValueType::dict)
		{
			ptr = (**(std::unordered_map<String, ValueType*>**)(ptr + 1))[*right];
			//++ptr;
			//std::unordered_map<String, ValueType*>* map = *(std::unordered_map<String, ValueType*>**)ptr;
			//ptr = (*map)[*right];

			program.stackInstructions.at_r(0).value = *ptr;

			program.memory.currentSize -= sizeof(void*) + sizeof(void*) + sizeof(void*);
			for (int i = 0; i < program.specification.typeSize[*ptr]; ++i)
				*(program.memory.data + program.memory.currentSize + i) = *((uint8*)ptr + sizeof(ValueType) + i);
			program.memory.currentSize += program.specification.typeSize[*ptr];

			delete left,
			delete right;
		}*/
	}
}
#include "core.h"

namespace Core {

	void doNothing() {}

	void getValueNameResolve() {
		Instruction& name = g_stack_instruction.at_r(1);

		//LoadLibraryW(
	}

	void getValueProcedure() {

	}

	void getPointerR0() {
		Instruction& name = g_stack_instruction.at_r(0);
		auto table = g_data.at_r(0);

		if (table.count(*g_memory.at<String*>(name.shift)))
		{
			ValueType* ptr = table[*g_memory.at<String*>(name.shift)];

			delete (String**)g_memory.at<String*>(name.shift);

			g_memory.at<ValueType*>(name.shift) = (ValueType*&)ptr;
		}
		else
		{
			ValueType* ptr = (ValueType*)malloc(sizeof(ValueType));
			*ptr = ValueType::none;
			g_data.at_r(0)[*g_memory.at<String*>(name.shift)] = ptr;

			delete g_memory.at<String*>(name.shift);

			g_memory.at<ValueType*>(name.shift) = ptr;
		}
		name.value = ValueType::pointer;
	}

	void getPointerR1() {
		Instruction& name = g_stack_instruction.at_r(1);
		auto table = g_data.at_r(0);

		if (table.count(*g_memory.at<String*>(name.shift)))
		{
			ValueType* ptr = table[*g_memory.at<String*>(name.shift)];

			delete (String**)g_memory.at<String*>(name.shift);

			g_memory.at<ValueType*>(name.shift) = (ValueType*&)ptr;
		}
		else
		{
			ValueType* ptr = (ValueType*)malloc(sizeof(ValueType));
			*ptr = ValueType::none;
			g_data.at_r(0)[*g_memory.at<String*>(name.shift)] = ptr;

			delete g_memory.at<String*>(name.shift);

			g_memory.at<ValueType*>(name.shift) = ptr;
		}
		name.value = ValueType::pointer;
	}


	void getReferenceR0() {
		Instruction& name = g_stack_instruction.at_r(0);
		Table<String, ValueType*>* table = &g_data.at_r(0);
		String str = String((charT*)(g_memory.content + name.shift), name.modifier);

		if (table->count(str))
		{
			ValueType** ptr = &(*table)[str];
			g_memory.at<ValueType**>(name.shift) = (ValueType**)ptr;
			g_memory.max_index = name.shift + sizeof(void*);
		}
		else
		{
			ValueType* ptr = (ValueType*)malloc(sizeof(ValueType));
			*ptr = ValueType::none;
			auto ref = &g_data.at_r(0)[str];
			*ref = ptr;
			g_memory.at<ValueType**>(name.shift) = ref;
			g_memory.max_index = name.shift + sizeof(void*);
		}
		name.value = ValueType::reference;
	}

	void getReferenceR1() {
		Instruction& name = g_stack_instruction.at_r(1);
		Table<String, ValueType*>* table = &g_data.at_r(0);
		String str = String((charT*)(g_memory.content + name.shift), name.modifier);

		if (table->count(str))
		{
			ValueType** ptr = &(*table)[str];
			g_memory.at<ValueType**>(name.shift) = (ValueType**)ptr;
			g_memory.max_index = name.shift + sizeof(void*);
		}
		else
		{
			ValueType* ptr = (ValueType*)malloc(sizeof(ValueType));
			*ptr = ValueType::none;
			auto ref = &g_data.at_r(0)[str];
			*ref = ptr;
			g_memory.at<ValueType**>(name.shift) = ref;
			g_memory.max_index = name.shift + sizeof(void*);
		}
		name.value = ValueType::reference;
	}

	void getValueR0() {
		Instruction& name = g_stack_instruction.at_r(0);
		auto table = g_data.at(0);	//Access global namespace
		String str = String((charT*)(g_memory.content + name.shift), name.modifier);

		if (table.count(str))
		{
			ValueType* ptr = table[str];

			switch (*ptr) {
			case ValueType::string:
				return;
			case ValueType::tuple:
			case ValueType::dict:
			case ValueType::unprocedure:
				g_memory.max_index -= name.modifier;
				g_memory.add<void*>(*(void**)(ptr + 1));
				//g_memory.add<uintptr_t>((uintptr_t)(ptr + 1));
				name.value = *ptr;
				return;
			default:
				g_memory.replace(ptr + 1, g_specification->type.size[(uint8)*ptr], name.shift, sizeof(void*));
				name.value = *ptr;
				return;
			}
		}
		else
		{
			g_memory.max_index -= sizeof(void*);
			name.value = ValueType::none;
		}
	}

	void getValueR1() {
		Instruction name = g_stack_instruction.at_r(1);
		auto table = g_data.at_r(0);
		String str = String((charT*)(g_memory.content + name.shift), name.modifier);

		if (table.count(str))
		{
			ValueType* ptr = table[str];

			switch (*ptr) {
			case ValueType::string:
				return;
			case ValueType::tuple:
			case ValueType::dict:
			case ValueType::unprocedure:
				memcpy(g_memory.content + name.shift + sizeof(void*), g_memory.content + name.shift + name.modifier, g_memory.max_index - name.shift + name.modifier);
				*(Procedure*)(g_memory.content + name.shift) = *(Procedure*)(ptr + 1);
				g_memory.max_index += sizeof(void*) - name.modifier;
				g_stack_instruction.at_r(0).shift += sizeof(void*) - name.modifier;
				name.value = *ptr;
				name.modifier = 0;
				g_stack_instruction.at_r(1) = name;
				return;
			default:
				g_memory.replace(ptr + 1, g_specification->type.size[(uint8)*ptr], name.shift, sizeof(void*));
				name.value = *ptr;
				return;
			}
		}
		else
		{
			g_memory.max_index -= sizeof(void*);
			name.value = ValueType::none;
		}
	}


	void conditional() {
		size_t size = g_specification->type.size[(uint8)g_stack_instruction.at_r(0).value];

		uint8* ptr = (g_memory.content + g_stack_instruction.at_r(0).shift);

		while (size)
			if (*(ptr + --size) != 0)
			{
				g_memory.max_index = g_stack_instruction.at_r(1).shift;
				g_stack_instruction.max_index -= 1;
				g_stack_instruction.at_r(0).instr = InstructionType::skip_after_next;
				g_stack_instruction.at_r(0).modifier = 0;
				return;
			}

		g_memory.max_index = g_stack_instruction.at_r(1).shift;
		g_stack_instruction.max_index -= 1;
		g_stack_instruction.at_r(0).instr = InstructionType::skip_next;
		g_stack_instruction.at_r(0).modifier = 0;
	}

	void conditionalTrue() {
		g_memory.max_index = g_stack_instruction.at_r(1).shift;
		g_stack_instruction.max_index -= 1;
		g_stack_instruction.at_r(0).instr = InstructionType::skip_after_next;
		g_stack_instruction.at_r(0).modifier = 0;
	}

	void conditionalFalse() {
		g_memory.max_index = g_stack_instruction.at_r(1).shift;
		g_stack_instruction.max_index -= 1;
		g_stack_instruction.at_r(0).instr = InstructionType::skip_next;
		g_stack_instruction.at_r(0).modifier = 0;
	}


	void loopWhile() { //TODO
		size_t size = g_specification->type.size[(uint8)g_stack_instruction.at_r(0).value];

		uint8* ptr = (g_memory.content + g_stack_instruction.at_r(0).shift);

		while (size)
			if (*(ptr + --size) != 0)
			{
				g_memory.max_index = g_stack_instruction.at_r(1).shift;
				g_stack_instruction.max_index -= 1;
				g_stack_instruction.at_r(0).instr = InstructionType::skip_after_next;
				g_stack_instruction.at_r(0).modifier = 0;
				return;
			}
	}


	void invokeResolve() {

	}

	void invokeProcedure() {
		((Procedure)g_memory.get<void*>(g_stack_instruction.get_r(1).shift))();
	}

	void invokeFunction() {
		Instruction parameter = g_stack_instruction.get_r(0);
		Instruction function = g_stack_instruction.get_r(2);

		/*if (context.value == ValueType::dll)
		{
			GetProcAddress(memory.get<HMODULE>(context.shift), memory.get<String>(function.shift).c_str());
			g_stack_array.get_r(parameter.modifier).content;
		}*/

		g_stack_instruction.max_index -= 2;

		//TODO: add a function to add value
		g_stack_instruction.add(Instruction::call(parameter.value, parameter.shift));	//TODO: make a 'call' instruction that specifies change of executable string
		g_stack_instruction.add(Instruction::context(parameter.value, parameter.shift));


	}

	void invokeNativeFunction() {

	}


	void assign() {
		auto left = g_stack_instruction.get_r(2);
		auto right = g_stack_instruction.get_r(0);

		ValueType* ptr = (ValueType*)malloc(sizeof(ValueType) + g_specification->type.size[(uint8)right.value]);
		*ptr = right.value;
		memcpy(ptr + 1, g_memory.content + right.shift, (uint8)g_specification->type.size[(uint8)right.value]);

		g_stack_namespace.get_r(0)[g_memory.get<String>(left.shift)] = ptr;
	}

	void assignToReference() {
		auto left = g_stack_instruction.get_r(2);
		auto right = g_stack_instruction.get_r(0);

		ValueType* ptr = (ValueType*)malloc(sizeof(ValueType) + g_specification->type.size[(uint8)right.value]);
		*ptr = right.value;
		memcpy(ptr + 1, g_memory.content + right.shift, g_specification->type.size[(uint8)right.value]);

		g_stack_namespace.get_r(0)[g_memory.get<String>(left.shift)] = ptr;
	}


	void commaPrefix() { //TODO: redo or prohibit prefix comma.
		if (g_stack_instruction.at_r(2).instr == InstructionType::spacing) {
			Instruction instruction_r0 = g_stack_instruction.get_r(0);
			g_memory.move_relative(instruction_r0.shift, g_memory.max_index, -(int64)sizeof(charT));

			g_stack_instruction.at_r(3) = g_stack_instruction.at_r(0);
			g_stack_instruction.max_index -= 3;
		}
		else {
			g_stack_instruction.at_r(1) = g_stack_instruction.at_r(0);
			g_stack_instruction.max_index -= 1;
		}
	}

	void commaPostfix() {
		g_memory.max_index = g_stack_instruction.at_r(2).shift;
		g_stack_instruction.at_r(2) = g_stack_instruction.at_r(0);
		g_stack_instruction.max_index -= 2;
	}

	void commaBinary() {
		g_memory.max_index = g_stack_instruction.at_r(2).shift;
		g_stack_instruction.at_r(2) = g_stack_instruction.at_r(0);
		g_stack_instruction.max_index -= 2;
	}


	void allArrayInclusive() {

	}
	void allArrayExclusive() {

	}
	void allGroupInclusive() {

	}
	void allGroupExclusive() {

	}
	void allContext() {

	}
}
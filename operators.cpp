
#include <iostream>
#include <deque>



#include "extend.hpp"

#include "operators.hpp"

#include "operators/basic_math.hpp"
#include "operators/boolean_ops.hpp"
#include "operators/const.hpp"
#include "operators/ctl_flow.hpp"
#include "operators/data_types.hpp"
#include "operators/errors.hpp"
#include "operators/generic_operators.hpp"
#include "operators/io.hpp"
#include "operators/list_stuff.hpp"
#include "operators/literals.hpp"
#include "operators/ref_ops.hpp"
#include "operators/stack_ctl.hpp"
#include "operators/types.hpp"
#include "operators/threads.hpp"
#include "operators/objects.hpp"
#include "operators/libraries.hpp"
#include "operators/math.hpp"



namespace operators {

	bool callToken(Frame& f, Frame::Exit& exit) {

		for (const Token t : tokens)
			if (t.condition(f)) {
				exit = t.act(f);
				return true;
			}
		return false;
	}


	bool callOperator(Frame& f, Frame::Exit& exit) {

		auto d = operators.find(f.feed.tok);
		if (d != operators.end()) {
			if (d->second.native) {
				exit = d->second.act(f);
				return true;

			} else if (d->second.run) {
				f.stack.emplace_back(*d->second._val);
				//f.feed.offset--;
				return callByName(f, "@", exit);

			} else {
				f.stack.emplace_back(*d->second._val);
				return true;
			}
		}
		return false;
	}
	bool callOperator(Frame& f, Frame::Exit& exit, const Namespace& ns) {

		auto d = ns.find(f.feed.tok);
		if (d != ns.end()) {

			if (d->second.native) {
				exit = d->second.act(f);
				return true;
			} else if (d->second.run) {

				f.stack.emplace_back(*d->second._val);
				//f.feed.offset--;
				return callByName(f, "@", exit);

			} else {
				f.stack.emplace_back(*d->second._val);
				return true;
			}
		}
		return false;
	}
	inline std::vector<struct Token> genTokens() {
		// loads relevant tokens from operators/*
		return std::vector<struct Token>({
				// fast literals
				OP_NS_TO_TOK(op_var_literal),
				OP_NS_TO_TOK(op_ns_member_req),

				// comments
				OP_NS_TO_TOK(op_line_comment),
				OP_NS_TO_TOK(op_multiline_comment),

				// literals
				OP_NS_TO_TOK(op_const_string),
				OP_NS_TO_TOK(op_const_macro),
				OP_NS_TO_TOK(op_const_list),

				// more literals (slower execution time
				OP_NS_TO_TOK(op_const_int),
				OP_NS_TO_TOK(op_const_number),

				OP_NS_TO_TOK(op_obj_mem_acc),
		});
	}

	inline const Namespace genOperators() {
		return Namespace({

			// cmp
			{ op_equals_to::name, op_equals_to::act },    // same value?
			{ "=?", op_equals_to::act },    // ^
			OP_NS_TO_PAIR(op_ne),           // !=
			OP_NS_TO_PAIR(op_gt),           // >
			OP_NS_TO_PAIR(op_lt),

			// lam + macro stuff
			OP_NS_TO_PAIR(op_exec),         // @ runs macro/lambda

			// references
			OP_NS_TO_PAIR(op_var_op),       // converts str to ref
			OP_NS_TO_PAIR(op_equals),       // change the depest value of reference
			OP_NS_TO_PAIR(op_set),          // give a variable a new value
			OP_NS_TO_PAIR(op_copy_value),   // get depest value and put on stack
			OP_NS_TO_PAIR(op_vars),         // debug variables
			OP_NS_TO_PAIR(op_const),        // converts ref to IMR

			// stack
			OP_NS_TO_PAIR(op_stack),        // stack:clear, stack:pop, stack:dup, stack:reverse, stack:swap, stack:...
			OP_NS_TO_PAIR(op_stk_pop),      // pops top elem

			// structured programming
			OP_NS_TO_PAIR(op_repeat_loop),  // run given number of times
			OP_NS_TO_PAIR(op_cond),         // unlabled elseelseifif statements

			// const values
			OP_NS_TO_PAIR(op_const_empty),  // empty value
			OP_NS_TO_PAIR(op_const_null),   // null reference
			OP_NS_TO_PAIR(op_const_true),   // 1
			OP_NS_TO_PAIR(op_const_false),  // 0

			// boolean
			OP_NS_TO_PAIR(op_not),          // !
			OP_NS_TO_PAIR(op_truthy),       // !! -- test for truthiness
			OP_NS_TO_PAIR(op_and),          // &&
			OP_NS_TO_PAIR(op_or),           // ||

			// cio
			OP_NS_TO_PAIR(op_println),      // prints value + std::endl
			OP_NS_TO_PAIR(op_print),        // prints value
			OP_NS_TO_PAIR(op_input),        // gets value

			// type conversion
			OP_NS_TO_PAIR(op_str),          // toString
			OP_NS_TO_PAIR(op_depict),       // raw-data depiction
			OP_NS_TO_PAIR(op_typeof),       // datatype
			OP_NS_TO_PAIR(op_int),

			OP_NS_TO_PAIR(op_size),

			OP_NS_TO_PAIR(op_namespace),    // namespace
			OP_NS_TO_PAIR(op_ns_mem_req_op),// : operator
			OP_NS_TO_PAIR(op_def),          // define
			OP_NS_TO_PAIR(op_index),        // ]
			OP_NS_TO_PAIR(op_while),        // {body} {condition} while

			OP_NS_TO_PAIR(op_add),
			OP_NS_TO_PAIR(op_minus),
			OP_NS_TO_PAIR(op_multiply),
			OP_NS_TO_PAIR(op_divide),
			OP_NS_TO_PAIR(op_int_divide),
			OP_NS_TO_PAIR(op_pow),
			OP_NS_TO_PAIR(op_remainder),
			OP_NS_TO_PAIR(op_abs),

			OP_NS_TO_PAIR(op_shift_left),
			OP_NS_TO_PAIR(op_shift_right),
			OP_NS_TO_PAIR(op_bw_and),
			OP_NS_TO_PAIR(op_bw_xor),
			OP_NS_TO_PAIR(op_bw_or),

			OP_NS_TO_PAIR(op_object),
			OP_NS_TO_PAIR(op_lambda),

			OP_NS_TO_PAIR(op_return),
			OP_NS_TO_PAIR(op_up),
			OP_NS_TO_PAIR(op_escape),

			OP_NS_TO_PAIR(op_keys),
			OP_NS_TO_PAIR(op_quit),
			OP_NS_TO_PAIR(op_obj_mem_acc_op),

			OP_NS_TO_PAIR(op_load_lib),

			OP_NS_TO_PAIR(op_math),
			OP_NS_TO_PAIR(ns_object),
			OP_NS_TO_PAIR(op_list_ns),
			OP_NS_TO_PAIR(op_apply_namespace),

			OP_NS_TO_PAIR(op_var_name),
			OP_NS_TO_PAIR(op_defs),
			OP_NS_TO_PAIR(op_strong),
			OP_NS_TO_PAIR(op_ref),
			OP_NS_TO_PAIR(op_trace_ref),
			OP_NS_TO_PAIR(op_let),

			OP_NS_TO_PAIR(op_throw),
			OP_NS_TO_PAIR(op_catch),

			// implementing these natively would involve >1000 lines of code
			{ "<=", Def("> !", true) }, // le is opposite of gt
			{ ">=", Def("< !", true) }, // ge is opposite of lt
			{ "++", Def("Stack:dup 1 + =", true) },
			{ "--", Def("Stack:dup 1 - =", true) },
			{ "+=", Def("'v' let Stack:swap = Stack:dup $v + =", true) },
			{ "-=", Def("'v' let Stack:swap = Stack:dup $v - =", true) },
			{ "*=", Def("'v' let Stack:swap = Stack:dup $v * =", true) },
			{ "/=", Def("'v' let Stack:swap = Stack:dup $v / =", true) },
			{ "<<=", Def("'v' let Stack:swap = Stack:dup $v << =", true) },
			{ ">>=", Def("'v' let Stack:swap = Stack:dup $v >> =", true) },
			{ "%=", Def("'v' let Stack:swap = Stack:dup $v % =", true) },
			{ "&=", Def("'v' let Stack:swap = Stack:dup $v & =", true) },
			{ "|=", Def("'v' let Stack:swap = Stack:dup $v | =", true) },
			{ "^=", Def("'v' let Stack:swap = Stack:dup $v ^ =", true) },
			{ "**=", Def("'v' let Stack:swap = Stack:dup $v ** =", true) },
		});
	}

	Namespace operators = genOperators();
	std::vector<struct Token> tokens = genTokens();


	bool callByName(Frame &f, const std::string &name, Frame::Exit &exit) {

		auto d = operators.find(name);
		if (d != operators.end()) { // found
			if (d->second.native) {
				exit = d->second.act(f);
				return true;
			} else if (d->second.run) {
				f.stack.emplace_back(*d->second._val);
				//f.feed.offset--;
				return callByName(f, "@", exit);
			} else {
				f.stack.emplace_back(*d->second._val);
				return true;
			}
		}

		for (const Token& t : tokens)
			if (t.name == name) {
				exit = t.act(f);
				return true;
			}

		return false;

	}

}
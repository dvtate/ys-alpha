
#include <ctype.h>
#include "operators.hpp"
#include "frame.hpp"


Frame::Exit Frame::run()
{
	//std::cout <<"running line: " <<feed.body <<std::endl;

	Frame::Exit ev;
	do {

		//std::cout <<"framerun::body[offset]: \'" <<feed.fromOffset() <<"\'\n";
		int op_ind = findOperator(*this);
		if (op_ind == -1) {
			ev = Frame::Exit(Frame::Exit::ERROR, "SyntaxError",
							 "unknown token on line " + std::to_string(feed.lineNumber()) + " near `" + feed.tok +
							 "`\n", feed.lineNumber());
			break;
		}
		if (op_ind == -2 || feed.offset >= feed.body.length())
			return Frame::Exit(Frame::Exit::FEED_END);

		//std::cout <<"framerun::body[offset]: \'" <<feed.fromOffset() <<"\'\n";
		ev = operators[op_ind].act(*this);

	} while (ev.reason == Frame::Exit::CONTINUE);


	if (ev.reason == Frame::Exit::ERROR)
		ev.genMsg(feed);

	//std::cout <<ev.msg;
	return ev;

}

const Value* Frame::getVar(const std::string vname) {
	auto v = vars.find(vname);

	if (v == vars.end()) {
		// make a new empty value to point to
		Value* empty = new Value();

		ref_vals.emplace_back(empty);	// will get deallocated at end of frame
		vars.emplace(vname, empty);
		v = vars.find(vname);
	}

	return v->second.ref;

}

// Name        : tokenClasses.cpp
// Code for ENSC 254 final exam prep -- Summer 2017
// Copyright (C) 2017 School of Engineering Science, SFU

#include <sstream>
#include "tokenClassifier.hpp"
#include "tokenClasses.hpp"

using namespace std;

namespace ensc251 {

// global variables
std::vector<TokenSP> tokenObjectPs; // objects for abstract syntax tree
std::size_t tracker{0}; // tracker index
evalMapType evalMap; // map holding evaluation results

void print_evaluation_map()
{
	cout << std::endl<<"-----Evaluation Map-----"<< std::endl;
	for(auto it = evalMap.cbegin(); it != evalMap.cend(); ++it)
	{
		cout << it->first << "     =     " << it->second.sptc->getStringValue()  << std::endl;
	}
	cout << std::endl;
}

TokenSP div_exp()
{
	TokenSP lowerNodeP{tokenObjectPs[tracker]->process_primary_exp()};
	if (lowerNodeP) {
		while (TokenSP upperNodeP{tokenObjectPs[tracker]->advance_past_div_operator()})
			if(TokenSP ueTreeP{tokenObjectPs[tracker]->process_primary_exp()}) {
				upperNodeP->add_childP(lowerNodeP);
				upperNodeP->add_childP(ueTreeP);
				lowerNodeP = upperNodeP;
			}
			else
				return nullptr;
	}
	return lowerNodeP;
}

TokenSP assignment_exp()
{
	size_t old_tracker{tracker};

	TokenSP subTreeP{tokenObjectPs[tracker]->process_id()};
	// production: id assignment_operator assignment_exp
	if (subTreeP)
		if(TokenSP tObjP{tokenObjectPs[tracker]->advance_past_gen_assignment_operator()})
		{
			tObjP->add_childP(subTreeP);
			if((subTreeP = assignment_exp()))
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				// we have assignment operator, but not valid assignment expression,
				//	so there must be an error and it should be reported.
				return nullptr;
		}
		// no assignment operator, so this is not this production

	// production:  div_exp
	tracker = old_tracker;
	return div_exp();
}

TokenSP recursive_parser()
{
//	if (tokenObjectPs.size())
		return assignment_exp();
//	else
//		throw ensc251::TokenException();
}

shared_ptr_to_constant create(float v) {
		return std::make_shared<numeric_const<float>>(v);
}

} // namespace ensc251

int main() {
	int rv{-1};
	try {
		istringstream input("errno = _doserrno = 124 / 10 / 1.2;");
		ensc251::tokenObjectPs = tokenClassifier(input);
		auto ast_root{ensc251::recursive_parser()};
		if(ast_root) {
			ast_root->print_ast();
			cout << endl << "About to evaluate AST." << endl;
			try {
				ast_root->eval();
				rv = 0;
			} catch(std::exception & e) {
				cout << e.what() << endl;
				rv = 2;
			}
			ensc251::print_evaluation_map();
		}
		else {
			cout << "Syntax error in input. Please try again..." << endl;
			rv = 1;
		}
	}
	catch (exception& e){
		cout << "Exception caught in main(): " << e.what() << endl;
		rv = 3;
	}
	catch (...) {
		cout << "Unknown exception caught in main() " << endl;
		rv = 4;
	}
	return rv;
}

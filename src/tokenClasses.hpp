// Name        : tokenClasses.hpp
// Code for ENSC 254 final exam prep -- Summer 2017
// Copyright (C) 2017 School of Engineering Science, SFU
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <stdexcept>
#include <memory>

#include "TreeNode.hpp"

namespace ensc251 {

void print_evaluation_map();

// forward declarations
class constant;
class Token;

typedef std::shared_ptr<constant> shared_ptr_to_constant;

// global variable external references (evalMap below)
typedef std::shared_ptr<Token> TokenSP;
extern std::vector<TokenSP> tokenObjectPs; // objects for abstract syntax tree
extern std::size_t tracker; // tracker index walks through tokenObjectPs

class Token : public TreeNode {	// The Token class, parent to all types of tokens

public:

	virtual const std::string& getStringValue() const = 0;
	virtual bool has_string_value(const std::string& aString) { return getStringValue() == aString; };

	void print_ast() { print_traverse(); OUT << ";" << std::endl;}

	virtual TokenSP process_id() { return nullptr; }
	virtual TokenSP process_primary_exp() { return nullptr; }

	TokenSP process_numeric_id()	{ return process_id(); }          // changed
	virtual TokenSP process_numeric_primary_exp() { return nullptr; }

	virtual TokenSP process_int_id() { return nullptr; }
	virtual TokenSP process_int_primary_exp() { return nullptr; }

	virtual TokenSP advance_past_div_operator() { return nullptr; }

	virtual TokenSP advance_past_gen_assignment_operator() { return nullptr; }

	virtual shared_ptr_to_constant eval() { return nullptr; };

	Token* getChildToken(unsigned childNum) const { // put on one line // return shared pointer?
		return static_cast<Token*>(this->childSPVector[childNum].get());
	}

	shared_ptr_to_constant evalChildToken(unsigned childNum) {  // put on one line
		return getChildToken(childNum)->eval();
	}

	TokenSP process_token()
	{
		childSPVector.clear();

		tracker++;
		if (tracker < tokenObjectPs.size()){
			return shared_from(this);
		}
		else
			// our grammar must end in ';'. If *this is the end
			//  then it is a premature end which we will report now.
			return nullptr;
	}

	virtual void print_action() { OUT << getStringValue() << " "; }

	virtual void print_traverse()
	{
		if(childSPVector.size() == 2)
			traverse_children_in_order(
					static_cast<traverse_func>(&Token::print_traverse),
					static_cast<action_func>(&Token::print_action));
		else
			traverse_children_pre_order(
					static_cast<traverse_func>(&Token::print_traverse),
					static_cast<action_func>(&Token::print_action));
	}

	virtual ~Token() {}
};

template<typename T>
class numeric_const;

template<typename T>
class int_const;

class constant : public Token {	// subclasses: numeric_const<T>, ...
public:
	virtual shared_ptr_to_constant putValueIntoInt() = 0;

	virtual shared_ptr_to_constant rdiv(float v) = 0;
	virtual shared_ptr_to_constant rdiv(int v) = 0;
	virtual shared_ptr_to_constant rdiv(char v) = 0;
	
	virtual shared_ptr_to_constant div(constant& obj) = 0;

	/* throw errors unless overridden */
	virtual int getIntValue() { throw std::domain_error("Requires int_const"); }

	TokenSP process_primary_exp() { return process_token(); }
};

class StringBasedToken:public Token {
protected:
	std::string m_stringValue;
public:
	StringBasedToken(const std::string &aStringValue): m_stringValue (aStringValue) {};
	const std::string& getStringValue() const override { return m_stringValue; }
};

class punctuator:public StringBasedToken {
	// e.g., ';', '*', '(', ')', ':', etc.
public:
	punctuator(const std::string &puncValue): StringBasedToken(puncValue){ };
};

class type_spec:public StringBasedToken {	// 'int'
public:
	type_spec(): StringBasedToken("int") { };
	type_spec(const std::string &typeValue): StringBasedToken(typeValue){ };

	shared_ptr_to_constant ofTypeCreateConstant(shared_ptr_to_constant constToCreateFromSP)
	{
		switch (this->m_stringValue[0]) {
		case 'i': // int
			return constToCreateFromSP->putValueIntoInt();
		}
		throw std::logic_error ("Should never get to this line of code");
	}
};

struct varInfo {
	shared_ptr_to_constant sptc;
	type_spec ts;
};

typedef std::map<std::string, varInfo > evalMapType;
extern
evalMapType evalMap;  // map holds the evaluation results for AST.

class id:public StringBasedToken {	// identifiers -- example:  sum
public:
	id(const std::string &idValue): StringBasedToken(idValue) {  };
	TokenSP process_id()	{ return process_token(); }
	TokenSP process_primary_exp() { return process_id(); }
	shared_ptr_to_constant eval() { return evalMap[getStringValue()].sptc; }
};

class numeric_id:public id {	// id of type float (and see below)
public:
	numeric_id(const std::string &idValue): id(idValue) {  };
	TokenSP process_numeric_primary_exp() { return process_numeric_id(); }
};

class int_id:public numeric_id {	// id of types char and int
public:
	int_id(const std::string &idValue): numeric_id(idValue) {  };
	TokenSP process_int_id()	{ return process_id(); } // a different pattern is being followed here.
	TokenSP process_int_primary_exp() { return process_int_id(); }
};

class gen_assignment_operator:public StringBasedToken
{
	// '=' | '/=' | '+=' | '-='
public:
	gen_assignment_operator(const std::string &opValue): StringBasedToken(opValue) { };
	TokenSP advance_past_gen_assignment_operator() { return process_token();	}

	shared_ptr_to_constant eval()
	{ // '='
		auto right = evalChildToken(1); // can get rid of 'right'
		auto key = getChildToken(0)->getStringValue();
		return evalMap[key].sptc = evalMap[key].ts.ofTypeCreateConstant(right);
	};

};

class div_operator:public StringBasedToken {	// '/'
public:
	div_operator(const std::string &opValue): StringBasedToken(opValue) { };
	TokenSP advance_past_div_operator() { return process_token(); }

	shared_ptr_to_constant eval()
	{
		auto right_eval = evalChildToken(1);
		return evalChildToken(0)->div(*right_eval);
	}
};

shared_ptr_to_constant create(float v); // definition in tokenClasses.cpp

// template function.  See beginning of chapter 17.
template<typename T> // needed for int and char
shared_ptr_to_constant create(T v) {
		return std::make_shared<int_const<T>>(v);
}

template<typename T>
class numeric_const:public constant
{
private:
	T m_value;
	std::string m_stringValue;
public:
	numeric_const(const T& constValue): m_value(constValue), m_stringValue(std::to_string(m_value))  {}
	shared_ptr_to_constant putValueIntoInt() { return std::make_shared<int_const<int>>(m_value);}

	const std::string& getStringValue() const { return m_stringValue; }
	const T& getValue() const { return m_value; }

	shared_ptr_to_constant rdiv(float v) { return create(v / m_value); }
	shared_ptr_to_constant rdiv(int v) { return create(v / m_value); }
	shared_ptr_to_constant rdiv(char v) { return create(v / m_value); }

	shared_ptr_to_constant div(constant& obj) { return obj.rdiv(m_value); }

	shared_ptr_to_constant eval() {	return create(m_value); }

	TokenSP process_numeric_primary_exp() { return this->process_primary_exp(); }
};

template<typename T>
class int_const:public numeric_const<T>
{
public:
	int_const(const T& constValue): numeric_const<T>(constValue) {}

	int getIntValue() { return this->getValue(); }

	shared_ptr_to_constant rdiv(int v)
	{
		if (this->getValue())
			return create(v / this->getValue());
		else
			throw(std::overflow_error("Divide by zero"));
	}

	shared_ptr_to_constant rdiv(char v)
	{
		return rdiv(static_cast<int>(v));
	}

	TokenSP process_int_primary_exp() { return this->process_primary_exp(); }
};

} // namespace ensc251

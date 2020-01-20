#ifndef INPUT_PASRSER_H
#define INPUT_PASRSER_H

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Input_Parser {
      public:
	template <typename T>
	void add_value(T &value, const std::string &key,
		       const std::string &description, bool *is_set = nullptr,
		       std::function<T(const std::string &)> conversion =
			   [](const std::string &string) {
				   T value;
				   std::stringstream(string) >> value;
				   return value;
			   });

	void add_flag(bool &value, const std::string &key,
		      const std::string &description);

	void parse(int argc, char *argv[]);

	std::string get_help() const;

      private:
	class Abstract_Value {
	      public:
		Abstract_Value(const std::string &description, bool *is_set);
		virtual ~Abstract_Value() = 0;

		const std::string get_description() const;

		virtual void set(const std::string &value_string) const = 0;

	      private:
		std::string description;

	      protected:
		bool *is_set;
	};
	template <typename T> class Value : public Abstract_Value {
	      public:
		Value(T &value, const std::string &description, bool *is_set,
		      std::function<T(const std::string &)> conversion);
		virtual ~Value() override;
		virtual void
		set(const std::string &value_string) const override;

	      private:
		T &value;
		std::function<T(const std::string &)> conversion;
	};
	class Flag {
	      public:
		Flag(bool &value, const std::string &description);

		const std::string get_description() const;

		void set() const;

	      private:
		bool &value;
		std::string description;
	};

	std::unordered_map<std::string, std::unique_ptr<const Abstract_Value>>
	    values;
	std::unordered_map<std::string, std::unique_ptr<const Flag>> flags;
};

template <typename T>
Input_Parser::Value<T>::Value(T &value, const std::string &description,
			      bool *is_set,
			      std::function<T(const std::string &)> conversion)
    : Abstract_Value{description, is_set}, value{value},
      conversion(conversion) {}

template <typename T> Input_Parser::Value<T>::~Value() {}

template <typename T>
void Input_Parser::Value<T>::set(const std::string &value_string) const {
	try {
		this->value = conversion(value_string);
		if (this->is_set) {
			*this->is_set = true;
		}
	} catch (const std::invalid_argument &e) {
		std::cout << "Ignoring argument with invalid value \""
			  << value_string << "\"" << std::endl;
	}
}

template <typename T>
void Input_Parser::add_value(T &value, const std::string &key,
			     const std::string &description, bool *is_set,
			     std::function<T(const std::string &)> conversion) {
	this->values[key] =
	    std::make_unique<Value<T>>(value, description, is_set, conversion);
}

#endif
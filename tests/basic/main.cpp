#include "../../src/clargs/parser.hpp"

#include <utki/debug.hpp>
#include <utki/exception.hpp>


int main(int argc, char** argv){
	// test adding same key twice (only short key)
	{
		clargs::parser p;

		p.add('a', "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with short key 'a' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}
	
	// test adding same key twice (only long key)
	{
		clargs::parser p;

		p.add("abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add("abrakadabra", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with long key 'abrakadabra' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}

	// test adding same key twice (same short key and different long key)
	{
		clargs::parser p;

		p.add('a', "abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add('a', "simsalabim", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with short key 'a' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}

	// test adding same key twice (different short key and same long key)
	{
		clargs::parser p;

		p.add('a', "abrakadabra", "description", [](){});

		bool exception_caught = false;

		try{
			p.add('b', "abrakadabra", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			exception_caught = true;
			ASSERT_INFO_ALWAYS(std::string(e.what()) == "argument with long key 'abrakadabra' already exists", e.what())
		}
		ASSERT_ALWAYS(exception_caught)	
	}

	// test adding long key of one letter
	{
		clargs::parser p;

		p.add('a', "b", "description", [](){});

		try{
			p.add('b', "c", "description 2", [](std::string&&){});
		}catch(utki::invalid_state& e){
			ASSERT_ALWAYS(false)
		}
	}

	// test disabling of key arguemnts parsing
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a, &p](){
			++a;
			if(a == 3){
				p.enable_key_parsing(false);
			}
		});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		auto res = p.parse(utki::make_span(args));

		ASSERT_INFO_ALWAYS(a == 3, "a = " << a)
		ASSERT_INFO_ALWAYS(res.size() == 3, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "--aaa", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "-a", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "--aaa", "res[2] = " << res[2])
	}

	// test enabling of key arguemnts parsing
	{
		clargs::parser p;

		unsigned a = 0;
		
		p.add('a', "aaa", "description", [&a, &p](){
			++a;
			if(a == 2){
				p.enable_key_parsing(false);
			}
		});

		std::vector<const char*> args = {{
			"program_executable",
			"-a",
			"--aaa",
			"-a",
			"--aaa",
			"-a",
			"--aaa"
		}};

		std::vector<std::string> res;

		p.parse(utki::make_span(args), [&res, &p](std::string&& str){
			res.push_back(std::move(str));
			if(res.size() == 3){
				p.enable_key_parsing(true);
			}
		});

		ASSERT_INFO_ALWAYS(a == 3, "a = " << a)
		ASSERT_INFO_ALWAYS(res.size() == 3, "res.size() = " << res.size())
		ASSERT_INFO_ALWAYS(res[0] == "-a", "res[0] = " << res[0])
		ASSERT_INFO_ALWAYS(res[1] == "--aaa", "res[1] = " << res[1])
		ASSERT_INFO_ALWAYS(res[2] == "-a", "res[2] = " << res[2])
	}

	return 0;
}
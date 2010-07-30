// -*- mode: c++; fill-column: 79 -*-
// $Id$

/*
 * STX Execution Pipe Library v0.7.1
 * Copyright (C) 2010 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "stx-execpipe.h"

#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>

// Test pipe: none -> program -> string
void test_none_program_string()
{
    stx::ExecPipe ep;

    std::string output;
    ep.set_output_string(&output);

    ep.add_exec("/bin/echo", "test123");

    assert( ep.run().all_return_codes_zero() );

    assert( output == "test123\n" );
}

// Test pipe: string -> program -> string
void test_string_program_string()
{
    stx::ExecPipe ep;

    std::string input = "test123";
    input += std::string(1024*1024, '\1');
    ep.set_input_string(&input);

    std::string output;
    ep.set_output_string(&output);

    ep.add_exec("/bin/cat");

    assert( ep.run().all_return_codes_zero() );

    //std::cout << "o " << output.size() << " i " << input.size() << "\n";

    assert( output == input );
}

// Test pipe: string -> program -> program -> string
void test_string_program_program_string()
{
    stx::ExecPipe ep;

    std::string input = "test123";
    ep.set_input_string(&input);

    std::string output;
    ep.set_output_string(&output);

    ep.add_exec("/bin/cat");
    ep.add_execp("md5sum");

    assert( ep.run().all_return_codes_zero() );

    assert( output == "cc03e747a6afbbcbf8be7668acfebee5  -\n" );
}

// Test pipe: file -> program -> string
void test_file_program_string()
{
    stx::ExecPipe ep;

    ep.set_input_file("/proc/uptime");

    std::string output;
    ep.set_output_string(&output);

    ep.add_execp("sort");

    assert( ep.run().all_return_codes_zero() );

    assert( output.size() );
}

// Test pipe: string -> program -> object

class TestSink : public stx::PipeSink
{
public:
    std::string		m_save;

    bool		m_ok;

    TestSink()
	: m_ok(false)
    {
    }

    virtual void process(const void* data, unsigned int datalen)
    {
	m_save.append(reinterpret_cast<const char*>(data), datalen);
    }

    virtual void eof()
    {
	m_ok = (m_save == "cc03e747a6afbbcbf8be7668acfebee5  -\n");
    }
};

void test_string_program_object()
{
    stx::ExecPipe ep;

    std::string input = "test123";
    ep.set_input_string(&input);

    TestSink sink;
    ep.set_output_sink(&sink);

    ep.add_execp("md5sum");

    assert( ep.run().all_return_codes_zero() );

    assert( sink.m_ok );
}

class TestSource : public stx::PipeSource
{
public:
    unsigned int	m_count;

    std::string		m_wrote;

    TestSource()
	: m_count(100*1024)
    {
    }

    virtual bool poll()
    {
	for (unsigned int i = 0; i < 1000 && m_count > 0; ++i, --m_count)
	{
	    write(&i, sizeof(unsigned char));

	    m_wrote.append(reinterpret_cast<const char*>(&i), sizeof(unsigned char));
	}

	return (m_count > 0);
    }
};

// Test pipe: object -> program -> string

void test_object_program_string()
{
    stx::ExecPipe ep;

    TestSource source;
    ep.set_input_source(&source);

    std::string output;
    ep.set_output_string(&output);

    ep.add_execp("cat");

    assert( ep.run().all_return_codes_zero() );

    assert( source.m_wrote == output );
}

// Test pipe: object -> program -> function -> program -> string

#include <openssl/md5.h>

class TestFunctionMD5 : public stx::PipeFunction
{
public:

    MD5_CTX 	m_ctx;

    std::string m_digest;

    TestFunctionMD5()
    {
	MD5_Init(&m_ctx);
    }

    virtual void process(const void* data, unsigned int datalen)
    {
	MD5_Update(&m_ctx, data, datalen);

	write(data, datalen);
    }

    virtual void eof()
    {
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5_Final(digest, &m_ctx);

	m_digest.assign(reinterpret_cast<char*>(digest), sizeof(digest));
    }
};

std::string HexString(const std::string& str)
{
    std::ostringstream os;

    os << std::hex << std::setfill('0');

    for (std::string::const_iterator si = str.begin();
	 si != str.end(); ++si)
    {
	os << std::setw(2) << (static_cast<unsigned int>(*si) & 0xFF);
    }

    return os.str();
}

void test_object_program_object_program_string()
{
    stx::ExecPipe ep;

    TestSource source;
    ep.set_input_source(&source);

    std::string output;
    ep.set_output_string(&output);

    ep.add_execp("cat");

    TestFunctionMD5 func;
    ep.add_function(&func);

    ep.add_execp("md5sum");

    assert( ep.run().all_return_codes_zero() );

    assert( HexString(func.m_digest) == "0b66fcadf3a46cce7184487c4dabaf0f" );
    assert( output == "0b66fcadf3a46cce7184487c4dabaf0f  -\n" );
}

void test_object_program_object_string()
{
    stx::ExecPipe ep;

    TestSource source;
    ep.set_input_source(&source);

    std::string output;
    ep.set_output_string(&output);

    ep.add_execp("cat");

    TestFunctionMD5 func;
    ep.add_function(&func);

    assert( ep.run().all_return_codes_zero() );

    assert( HexString(func.m_digest) == "0b66fcadf3a46cce7184487c4dabaf0f" );
    assert( output.size() == 100*1024 );
}

void test_none_program_set_string()
{
    stx::ExecPipe ep;

    std::vector<std::string> args;
    args.push_back("/bin/sh");
    args.push_back("-c");
    args.push_back("set");

    std::vector<std::string> envs;
    envs.push_back("TEST=123");

    ep.add_exece("/bin/sh", &args, &envs);

    std::string output;
    ep.set_output_string(&output);

    assert( ep.run().all_return_codes_zero() );

    assert( output.find("TEST=123") != std::string::npos );
}

void test_error_debug_output_null(const char*)
{
}

void test_error_none_program_none()
{
    stx::ExecPipe ep;
    ep.set_debug_level(stx::ExecPipe::DL_INFO);
    ep.set_debug_output(test_error_debug_output_null);

    ep.add_exec("xyz-non-existing-program");

    ep.run();

    assert( !ep.all_return_codes_zero() );

    assert( ep.get_return_code(0) == 255 );
}

void test_segfault_none_program_none()
{
    stx::ExecPipe ep;

    ep.add_exec("./test_segfault");

    ep.run();
    assert( !ep.all_return_codes_zero() );

    assert( ep.get_return_signal(0) == 11 );
}

int main()
{
    test_none_program_string();
    test_string_program_string();
    test_string_program_program_string();
    test_file_program_string();
    test_string_program_object();
    test_object_program_string();
    test_object_program_object_program_string();
    test_object_program_object_string();
    test_none_program_set_string();

    test_error_none_program_none();
    test_segfault_none_program_none();

    return 0;
}

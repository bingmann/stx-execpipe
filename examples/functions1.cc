// -*- mode: c++; fill-column: 79 -*-
// $Id$

/*
 * This example shows how to use the function classes stx::PipeSource and
 * stx::PipeFunction to insert custom processing into a pipe sequence. The
 * application calls tar to create an archive, calculates the SHA1 digest of
 * the uncompressed tarball and then pipes the data into gzip for compression.
 */

#include "stx-execpipe.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include <openssl/sha.h>

class FilelistSource : public stx::PipeSource
{
public:
    
    // List of files to send to tar.
    std::vector<std::string>	m_list;

    // Current position in list.
    unsigned int	m_pos;
    
    FilelistSource()
	: m_pos(0)
    {
    }

    // Send one file name each time polled.
    virtual bool poll()
    {
	if (m_pos < m_list.size())
	{
	    write(m_list[m_pos].data(), m_list[m_pos].size());
	    write("\n", 1);
	    ++m_pos;
	}

	return (m_pos < m_list.size());
    }
};

class Sha1Function : public stx::PipeFunction
{
public:

    // Context of running SHA1 digest
    SHA_CTX 	m_ctx;

    // Finished digest generated in eof().
    std::string m_digest;

    Sha1Function()
    {
	SHA1_Init(&m_ctx);
    }

    // Update the sha1 digest context and pass on unmodified data.
    virtual void process(const void* data, unsigned int datalen)
    {
	SHA1_Update(&m_ctx, data, datalen);

	write(data, datalen);
    }

    // Calculate final SHA1 digest once the data stream closes.
    virtual void eof()
    {
	unsigned char digest[SHA_DIGEST_LENGTH];
	SHA1_Final(digest, &m_ctx);

	m_digest.assign(reinterpret_cast<char*>(digest), sizeof(digest));
    }
};

// Converts a binary string into hexadecimal digits.
std::string HexString(const std::string& str)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0');

    for (std::string::const_iterator si = str.begin(); si != str.end(); ++si)
    {
	os << std::setw(2) << (static_cast<unsigned int>(*si) & 0xFF);
    }

    return os.str();
}

int main()
{
    stx::ExecPipe ep;

    // initialize a source object generating some file names. obviously in a
    // real application this list would be longer.
    FilelistSource source;

    source.m_list.push_back("/bin/sh");
    source.m_list.push_back("/bin/bash");
    source.m_list.push_back("/bin/ls");
    source.m_list.push_back("/bin/gzip");

    ep.set_input_source(&source);

    // add new exec stage calling tar with an option to read files from stdin
    std::vector<std::string> tarargs;

    tarargs.push_back("tar");
    tarargs.push_back("--create");
    tarargs.push_back("--verbose");
    tarargs.push_back("--no-recursion");
    tarargs.push_back("--files-from");
    tarargs.push_back("/dev/stdin");

    ep.add_execp(&tarargs);

    // insert an intermediate processing stage to save the SHA1 sum of the
    // uncompressed tarball.
    Sha1Function sha1tar;

    ep.add_function(&sha1tar);

    // add compression stage
    ep.add_execp("gzip", "-9");

    // set output stream to a temporary file
    ep.set_output_file("/tmp/stx-execpipe-functions1.tar.gz");

    // run pipe
    try {
	if (!ep.run().all_return_codes_zero())
	{
	    std::cout << "Error calling programs: " << std::endl
		      << "tar returned = " << ep.get_return_code(0) << std::endl
		      << "gzip returned = " << ep.get_return_code(2) << std::endl;
        }
	else
	{
	    std::cout << "SHA1 of uncompress tar: " << HexString(sha1tar.m_digest) << std::endl
		      << "You can verify the digest using:" << std::endl
		      << "    zcat /tmp/stx-execpipe-functions1.tar.gz | sha1sum" << std::endl;
	}
    }
    catch (std::runtime_error &e)
    {
	std::cout << "Error running children: " << e.what() << std::endl;
    }

    return 0;
}

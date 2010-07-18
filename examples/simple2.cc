// -*- mode: c++; fill-column: 79 -*-
// $Id$

/*
 * This second example shows how to directly call a program, but this time use
 * execp() variants, which search the PATH environment.
 */

#include "stx-execpipe.h"

#include <iostream>
#include <stdexcept>

int main()
{
    stx::ExecPipe ep;

    std::string input = "test123";
    ep.set_input_string(&input);

    ep.add_execp("sha1sum");

    std::string output;
    ep.set_output_string(&output);

    try {
	if (!ep.run().all_return_codes_zero())
	{
	    std::cout << "Error calling echo: return code = " << ep.get_return_code(0) << std::endl;
        }
	else
	{
	    std::cout << "sha1sum output: " << output << std::endl;
	}
    }
    catch (std::runtime_error &e)
    {
	std::cout << "Error running children: " << e.what() << std::endl;
    }

    return 0;
}

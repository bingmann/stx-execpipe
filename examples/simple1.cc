// -*- mode: c++; fill-column: 79 -*-
// $Id$

/*
 * This first example shows how to directly call a program, in this case
 * "/bin/echo" and save it's output in a std::string.
 */

#include "stx-execpipe.h"

#include <iostream>
#include <stdexcept>

int main()
{
    stx::ExecPipe ep;

    ep.add_exec("/bin/echo", "-n", "test123");

    std::string output;
    ep.set_output_string(&output);

    try {
	if (!ep.run().all_return_codes_zero())
	{
	    std::cout << "Error calling echo: return code = " << ep.get_return_code(0) << std::endl;
        }
	else
	{
	    std::cout << "echo output: " << output << std::endl;
	}
    }
    catch (std::runtime_error &e)
    {
	std::cout << "Error running children: " << e.what() << std::endl;
    }

    return 0;
}


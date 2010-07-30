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


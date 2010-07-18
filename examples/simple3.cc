// -*- mode: c++; fill-column: 79 -*-
// $Id$

/*
 * STX Execution Pipe Library v0.7.0
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
 * This third example shows how to call a sequence of programs. The pipe
 * consists of "ls --size /bin" listing a direction, grepping out all 'shells'
 * and sorting these by file size.
 */

#include "stx-execpipe.h"

#include <iostream>
#include <stdexcept>

int main()
{
    stx::ExecPipe ep;

    ep.add_execp("ls", "--size", "/bin");

    ep.add_execp("grep", "sh");

    std::vector<std::string> sortargs;
    sortargs.push_back("sort");
    sortargs.push_back("--numeric-sort");
    sortargs.push_back("--ignore-leading-blanks");
    sortargs.push_back("--reverse");
    sortargs.push_back("--stable");
    ep.add_execp(&sortargs);

    std::string output;
    ep.set_output_string(&output);

    try {
	if (!ep.run().all_return_codes_zero())
	{
	    std::cout << "Error calling programs: " << std::endl
		      << "ls returned = " << ep.get_return_code(0) << std::endl
		      << "grep returned = " << ep.get_return_code(1) << std::endl
		      << "sort returned = " << ep.get_return_code(2) << std::endl;
        }
	else
	{
	    std::cout << "pipe output: " << output << std::endl;
	}
    }
    catch (std::runtime_error &e)
    {
	std::cout << "Error running children: " << e.what() << std::endl;
    }

    return 0;
}

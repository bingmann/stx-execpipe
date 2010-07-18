// $Id$

#include "stx-execpipe.h"

// need access to anonymous namespace
#include "stx-execpipe.cc"

void test1()
{
    stx::RingBuffer rb;

    for (unsigned int i = 0; i < 128; ++i)
    {
	rb.write(&i, sizeof(i));
    }

    assert( rb.size() == 128 * sizeof(unsigned int) );
    
    for (unsigned int i = 0; i < 128; ++i)
    {
	assert( rb.bottomsize() >= sizeof(unsigned int) );
	assert( *reinterpret_cast<unsigned int*>(rb.bottom()) == i );

	rb.advance( sizeof(unsigned int) );
    }

    assert( rb.size() == 0 );
    assert( rb.bottomsize() == 0 );
    assert( rb.buffsize() == 1024 );

    for (unsigned int i = 0; i < 512; ++i)
    {
	assert( rb.size() == 0 );
	assert( rb.bottomsize() == 0 );

	rb.write(&i, sizeof(i));

	assert( rb.bottomsize() == 4 );
	assert( *reinterpret_cast<unsigned int*>(rb.bottom()) == i );

	rb.advance( sizeof(unsigned int) );
    }
}

void test2()
{
    char buffer[2048];

    for (unsigned int i = 0; i < sizeof(buffer); ++i)
	buffer[i] = i;

    // test growth in first buffer state
    {
	stx::RingBuffer rb;

	rb.write(buffer, 256);
	rb.advance(256);

	assert(rb.size() == 0);
	assert(rb.bottomsize() == 0);
	assert(rb.buffsize() == 1024);

	rb.write(buffer, 512);

	assert(rb.size() == 512);
	assert(rb.bottomsize() == 512);
	assert(rb.buffsize() == 1024);

	rb.write(buffer, 1024);

	assert(rb.size() == 512+1024);
	assert(rb.bottomsize() == 512+1024);
	assert(rb.buffsize() == 2048);
    }

    // test growth in second buffer state
    {
	stx::RingBuffer rb;

	rb.write(buffer, 512+256);
	rb.advance(512+256);

	assert(rb.size() == 0);
	assert(rb.bottomsize() == 0);
	assert(rb.buffsize() == 1024);

	rb.write(buffer, 512);

	assert(rb.size() == 512);
	assert(rb.bottomsize() == 256);
	assert(rb.buffsize() == 1024);

	rb.write(buffer, 1024);

	assert(rb.size() == 512+1024);
	assert(rb.bottomsize() == 256);
	assert(rb.buffsize() == 2048);

    }
}

int main()
{
    test1();
    test2();

    return 0;
}

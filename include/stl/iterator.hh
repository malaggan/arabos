// -*- mode: c++; -*-

#pragma once

#include <stl/new.hh>
#include <lib.h>
#include <asm.h> // for atomic_inc
#include <types.h> // for SFINAE stuff

namespace aos {

#include <stl/bits/iterator_type.hh>
#include <stl/bits/iterator_traits.hh>
#include <stl/bits/range_access.hh>
#include <stl/bits/iterator_funcs.hh>
#include <stl/bits/back_insert_iterator.hh>
#include <stl/bits/reverse_iterator.hh>

	// std::ceil
	// std::chrono::system_clock::now()

	inline int ceil(double) __attribute__((always_inline));
	inline int ceil(double x) {
		int sign = (x < 0)?-1:1;
		x *= sign; // make positive
		int ix = static_cast<int>(x);
		if(sign * x  == static_cast<double>(ix)) return sign * ix; // x was exactly an int
		return sign*ix+1;
	}
	// std::chrono::system_clock::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().ctime);
	// std::make_shared
	// std::unique_lock<std::mutex>
	// std::lock_guard<std::mutex>

#include <stl/bits/copy.hh>
#include <stl/bits/move.hh>
#include <stl/bits/copy_backward.hh>
#include <stl/bits/move_backward.hh>
#include <stl/bits/uninitialized_copy.hh>
#include <stl/bits/uninitialized_move.hh>
#include <stl/bits/swap.hh>
#include <stl/bits/copy_if.hh>
#include <stl/bits/copy_n.hh>
#include <stl/bits/find.hh>
#include <stl/bits/shared_ptr.hh>
#include <stl/bits/vector.hh>
#include <stl/bits/static_vector.hh>
#include <stl/bits/string.hh>


	class time_point{
	} __attribute__((packed));

	inline __time_t to_time_t(time_point) __attribute__((always_inline)); // TODO: implement
	inline __time_t to_time_t(time_point) { return 1234; } // dummpy implementation

	inline time_point now() __attribute__((always_inline)); // TODO: implement
	inline time_point now() { return time_point{} ; } // dummy implementation for now. should use rtdsc.
}

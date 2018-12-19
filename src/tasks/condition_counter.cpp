/*!
 * \author ddubois 
 * \date 17-Dec-18.
 */

#include "condition_counter.hpp"

namespace nova::ttl {
	void condition_counter::add(const uint32_t num) {
		bool should_trigger = false;
		{
			std::unique_lock l(mut);
			counter += num;
			should_trigger = counter == wait_val;
		}

        if(should_trigger) {
			cv.notify_all();
        }
	}

    void condition_counter::sub(const uint32_t num) {
		bool should_trigger = false;
		{
			std::unique_lock l(mut);
			counter -= (counter < num) ? counter : 0;
			should_trigger = counter == wait_val;
		}

		if(should_trigger) {
			cv.notify_all();
		}	    
	}

    void condition_counter::wait_for_value(const uint32_t val) {
		wait_val = val;

		{
			std::unique_lock l(mut);
            // I want to explicitly copy wait_val so that the same condition variable can be waited on for different 
		    // values, but I need to copy counter by reference
			cv.wait(l, [&, wait_val] {return counter == wait_val; });
		}
	}
}

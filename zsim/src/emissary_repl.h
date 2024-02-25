#ifndef EMISSARY_REPL_H_
#define EMISSARY_REPL_H_

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "repl_policies.h"

template <bool sharersAware>
// EMISSARY : Enhanced Miss Aware Replacement Policy
class EmissaryReplPolicy : public ReplPolicy {
    protected:
		uint64_t timestamp; // incremented on each access
		//Array to keep track of timestamps
    		uint64_t* array;
    		uint32_t numLines;
		//Array to track whether a line is high-priority(1) or low-priority(0)		
		bool* priorityArray;
		//Threshold (N) -> indicates number of high priority lines that can exist at any given time
		uint32_t hpThreshold;
		//Variable to keep track of number of high-priority entries (should be less than or equal to threshold)
		uint32_t hpCount;
		//Config variable for pseudo-random selection probability (1/randProb)
		uint32_t randProb;
		//Variable for random number
		uint32_t randNumber;
		//Variable for pseudo-random selection
		bool randSelect;
	
    public:
    		explicit EmissaryReplPolicy(uint32_t _numLines, uint32_t _hpThreshold, uint32_t _randProb) : timestamp(1), numLines(_numLines), hpThreshold(_hpThreshold), randProb(_randProb) {
            		array = gm_calloc<uint64_t>(numLines);
	    		priorityArray = gm_calloc<bool>(numLines); // Assume all cache lines are low-priority initially
			hpCount = 0; //Initialize count to 0
			srand(static_cast<unsigned int>(time(0))); //Random seed	
        	}

        	~EmissaryReplPolicy() {
            		gm_free(array);
	    		gm_free(priorityArray);
        	}

        	void update(uint32_t id, const MemReq* req) {
			//Increment timestamp on update
			array[id] = timestamp++;
        	}

        	void replaced(uint32_t id) {
			//Reset timestamp of entry on replacement
        		array[id] = 0;
			//Reset priority of entry on replacement (if high priority entry got replaced, change priority back to low)
			if (priorityArray[id]) {
				priorityArray[id] = false;
				hpCount--;
				//std::cout << "DBG::emissary_repl.h::HPCount_ReplacedHP=" << hpCount << std::endl;
			}
        	}
	
		void set_priority(uint32_t id, bool priority) {
		    //Pseudo-Random Selection
		    int randNumber = rand() % randProb;
		    //Selection is based on probability of 1/randProb
    		    if (randNumber == 0) {
		    	randSelect=true;
	            } else {
	                randSelect=false;
		    }
		    //std::cout << "DBG::emissary_repl.h::RandomNumber=" << randNumber << std::endl;
		    //std::cout << "DBG::emissary_repl.h::RandomSelect=" << randSelect << std::endl;
	    	    if(!priorityArray[id] && priority && hpCount<hpThreshold && randSelect) {
		        hpCount++; //Increment count
		    	priorityArray[id] = priority;
		    }
		}
		
		bool return_priority(uint32_t id) {
	    	    return priorityArray[id];
		}

        	template <typename C> inline uint32_t rank(const MemReq* req, C cands) {
			uint32_t bestLowPriorityCand = -1;
    			uint64_t bestLowPriorityScore = (uint64_t)-1L;
    			uint32_t bestHighPriorityCand = -1;
    			uint64_t bestHighPriorityScore = (uint64_t)-1L;
            		
			//Emissary Eviction Policy
			//If number of high-priority (P = 1) lines <= threshold, evict the LRU among the low-priority (P = 0) lines
			//Else, evict the LRU among high-priority (P = 1) lines
			
			// First pass to find the best candidate in each priority category
    			for (auto ci = cands.begin(); ci != cands.end(); ci.inc()) {
        			if (priorityArray[*ci]) {
            				if (score(*ci) < bestHighPriorityScore) {
                				bestHighPriorityCand = *ci;
                				bestHighPriorityScore = score(*ci);
            				}
        			} 
				else {
            				if (score(*ci) < bestLowPriorityScore) {
                				bestLowPriorityCand = *ci;
                				bestLowPriorityScore = score(*ci);
            				}
        			}
    			}
			//std::cout << "DBG::emissary_repl.h::BestLowPriorityCand=" << bestLowPriorityCand << std::endl;
			//std::cout << "DBG::emissary_repl.h::BestHighPriorityCand=" << bestHighPriorityCand << std::endl;
			// Decide which candidate to evict based on the high-priority line count and threshold
    			//std::cout << "DBG::emissary_repl.h::HPCount_Rank=" << hpCount << std::endl;
			if (hpCount < hpThreshold) {
        			// If high-priority line count is below the threshold, evict from low-priority lines
        			//std::cout << "DBG::emissary_repl.h::Returned Low Priority Cand For Eviction" << std::endl;
        			return bestLowPriorityCand;
    			}
			else if ((hpCount>=hpThreshold) && (bestHighPriorityCand != -1)) {
        			// If high-priority line count is equal to or exceeds the threshold, evict from high-priority lines
        			//std::cout << "DBG::emissary_repl.h::Returned High Priority Cand For Eviction" << std::endl;
        			return bestHighPriorityCand;
    			}
			else {
				//std::cout << "DBG::emissary_repl.h::Returned Low Priority Cand (Fallback) For Eviction" << std::endl;
				return bestLowPriorityCand; // Fallback to low-priority if no high-priority candidates are found
			}
		}

        	DECL_RANK_BINDINGS;

	private:
        	inline uint64_t score(uint32_t id) { //higher is least evictable
            		//array[id] < timestamp always, so this prioritizes by:
            		// (1) valid (if not valid, it's 0)
            		// (2) sharers, and
            		// (3) timestamp
            		return (sharersAware? cc->numSharers(id) : 0)*timestamp + array[id]*cc->isValid(id);
        	}
};
#endif // EMISSARY_REPL_H_

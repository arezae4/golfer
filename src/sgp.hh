#ifndef _SGP_HH
#define _SGP_HH

#include <vector>
#include <ostream>
#include <stdexcept>
#include <string>
#include "log4cpp/Category.hh"
#include <climits>
#include <map>
#include <iterator>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

namespace sgp{

const int UNSET = -1;
	
enum class INIT_ALG {
	RANDOM,
};

enum class SEL_ALG {
	RANDOM,
	RANDOM_CONFLICT
};

double gen_rand();
unsigned int gen_rand(unsigned int n);

class SGPTabuList;

#ifdef TEST	
#include "gtest/gtest_prod.h"
// Defines FRIEND_TEST
#endif

struct decision{
	unsigned int w;
	unsigned int g;
	unsigned int val;
	/* mark this decision as erased and to-be-discard*/
	mutable bool erased;	
	decision(unsigned int w, unsigned int g, unsigned int val);
	friend bool operator < (const decision& lhs, const decision& rhs);
	friend bool operator == (const decision& lhs, const decision& rhs);
};

class decisionHash{
  private:
	int g;
	int p;
	int w;
  public:
	decisionHash(int g, int p, int w);
	std::size_t operator() (const decision& d) const;
};


class SGP{
	
  public:
	SGP(unsigned int g, unsigned int p, unsigned int w);
	SGP(const SGP& sgp);
	SGP& operator=(SGP& other);

	unsigned int g() const;
	unsigned int p() const;
	unsigned int w() const;
	std::string get_conflict_matrix_str() const; //TODO: add move semantic
	void set_init_alg(INIT_ALG alg);
	int get_eval() const;
	void init_solution();	
	void local_search(class SGPTabuList& tabu, unsigned int best_eval);

  private:
	unsigned int _g; 	// 	#Groups
	unsigned int _p; 	// 	#Players
	unsigned int _w;	//	#Weeks
	
	int best_eval = INT_MAX;
	
	/**
	*	Multi-index hash set for storing DECISIONS
	*	Ordered uniquely by operator <
	*	Ordered secondairly by W
	*/
	typedef boost::multi_index_container<
	decision,
	boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique< 
				boost::multi_index::identity<decision> >,
				/*	ordered by operator< */
			boost::multi_index::ordered_non_unique< 
				boost::multi_index::member<	decision , 	unsigned int , 
														&decision::w> >	
		>
	> multi_index_decision_set;

	
	std::vector<std::vector<std::set<int>>> _tables;
	std::vector<std::vector<int>> _groups;
	std::vector<std::vector<int>> _conflict_matrix;
	std::vector<bool> is_in_conflict_set;
	INIT_ALG _init_alg = INIT_ALG::RANDOM;
	//std::unordered_set<decision, decisionHash> conflict_set;
	multi_index_decision_set conflict_set;

	friend std::ostream& operator<<(std::ostream& os, const SGP& sgp);
#ifdef TEST
	FRIEND_TEST(SGPTest, SET_FIELD);
	FRIEND_TEST(SGPTest, CALC_CONFLICTS);
	FRIEND_TEST(SGPTest, INIT_TEST);
	friend class SGPTest;
#endif
	
	
	/**
	 * > Increase the conflicts between player P1 and P2
	 * > P1 is the new player being inserted in <G1, W1>  where P2 resides.   
	 * > ALWAYS inserts <w1, g1, p1> in conflict_set 
	 * if the conflicts become > 1
	 * > inserts ALL other positions of P1 and P2 in conflict_set if 
	 * conflicts becomes 2 by inserting P1.
	 */
	void 
	inc_conflict(unsigned int w1, unsigned int g1, 	unsigned int p1, 
													unsigned int p2);
	/**
	 * Decrease the conflicts between player P1 and P2
	 * P1 is the player being erased from < W1, G1> where P2 resides  
	 * ALWAYS removes < w1, g1, p1> from conflict_set.
	 * Removes ALL other positions of P1 and P2 in conflict_set if 
	 * conflicts become 1 by removing P1.
	 */
	void 
	dec_conflict(unsigned int w1, unsigned int g1, 	unsigned int p1, 
													unsigned int p2);

	void 
	add_conflict(unsigned int w, unsigned int g, unsigned int val);
	
	void 
	remove_conflict(unsigned int w, unsigned int g, unsigned int val);
	
	/**
	 * set the <(W, G, _)> = OLD_VAL cell to NEW_VAL and adjusts the 
	 * conflicts and best eval
	 */ 
	void 
	set_field(	unsigned int w, unsigned int g, int old_val, int new_val);
	
	/**
	 * Calculates the nett conflicts difference in the group G 
	 * if OLD_VAL was to be replaced with NEW_VAL
	 * Does NOT change _conflict_matrix
	 * PRE-CONDITION: 	OLD_VAL must be a member of <W, G>
	 * 					NEW_VAL must NOT be contained in <W, G>
	 *
	 * 					MUST NOT generate duplicates! 
	 * 					it will not be checked due to efficiency reasons. 
	 */
	int 
	calc_conflicts_diff_in_group(	unsigned int w,	unsigned int g, 
									int old_val, 	int new_val);
	
	/**
	 * Returns the accumulated conflicts of PLAYER only in this group <W,G> 
	 */
	unsigned int 
	calc_conflicts_player_in_group(unsigned int w, 	unsigned int g,
													unsigned int player);
	

	unsigned int 
	calc_conflicts_player(int player);

	inline 
	log4cpp::Category& logger() {
		static log4cpp::Category& logger(
							log4cpp::Category::getInstance("sgp"));
		return logger;
	}
};


/**
 * Base type for all solvers
 */
class SGPSolver{
	public:
		virtual ~SGPSolver() {};
		virtual void run() = 0;
		virtual double runtime() = 0;
		virtual unsigned int iterations() = 0;
};

} // NAMESPACE_SGP


#endif //_SGP_HH

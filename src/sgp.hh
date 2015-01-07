#ifndef _SGP_HH
#define _SGP_HH

#include <vector>
#include <ostream>
#include <stdexcept>
#include <string>
#include "log4cpp/Category.hh"
#include <climits>
#include <map>
#include <unordered_set>
#include <iterator>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

namespace sgp{

/* Changes the stream color to red */	
/*inline std::ostream& red(std::ostream &s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    SetConsoleTextAttribute(hStdout, 
                FOREGROUND_RED|FOREGROUND_INTENSITY);
    return s;
}

* Changes the stream color to white *
inline std::ostream& white(std::ostream &s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    SetConsoleTextAttribute(hStdout, 
       FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    return s;
}*/

const int UNSET = -1;
	
enum class INIT_ALG {
	RANDOM,
};

enum class SEL_ALG {
	RANDOM,
	RANDOM_CONFLICT
};

class Test_SGP;

double gen_rand();
unsigned int gen_rand(unsigned int n);

struct decision{
	unsigned int w;
	unsigned int g;
	unsigned int val;
	mutable bool erased;	/* mark this decision as erased and to-be-discard*/
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

/**
 * Interface for Tabu Lists used in local search for SGP
 */
class SGPTabuList{
  	public:
	virtual void add(unsigned int week, int player1, int player2) = 0;
	virtual bool is_tabu(unsigned int week, int player1, int player2) = 0;
	virtual void clear(unsigned int week) = 0;
	virtual void clearAll() = 0;
	virtual void incr_iteration() = 0;
	virtual ~SGPTabuList() {};
};

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
		/*	boost::multi_index::ordered_non_unique< 
				boost::multi_index::member<	decision , 	int , 
														&decision::val> >*/
		>
	> multi_index_decision_set;

#ifdef TEST	
#include "gtest/gtest_prod.h"
// Defines FRIEND_TEST
#endif

class SGP{
	
	private:
		unsigned int _g; // 	#Groups
		unsigned int _p; // 	#Players
		unsigned int _w;	//	#Weeks
		
		int best_eval = INT_MAX;
		
		std::vector<std::vector<std::set<int>>> _tables;
		std::vector<std::vector<int>> _groups;
		std::vector<std::vector<int>> _conflict_matrix;
		std::vector<bool> is_in_conflict_set;
		INIT_ALG _init_alg = INIT_ALG::RANDOM;
		//std::unordered_set<decision, decisionHash> conflict_set;
		multi_index_decision_set conflict_set;

		friend std::ostream& operator<<(std::ostream& os, const SGP& sgp);
		friend class Test_SGP;
#ifdef TEST
		FRIEND_TEST(SGPTest, SET_FIELD);
		FRIEND_TEST(SGPTest, CALC_CONFLICTS);
		FRIEND_TEST(SGPTest, INIT_TEST);
		friend class SGPTest;
#endif
		
		
		void inc_conflict(unsigned int p1, unsigned int p2);
		void dec_conflict(unsigned int p1, unsigned int p2);
		
		/**
		 * Adds new entries to conflict_set if the player is recently joined 
		 * a conflict (if has_conflict[] is still false), or set CONFLICTS 
		 * in all previous corresponding entries to 0 if the player is not 
		 * currently in any conflict
		 */
		void update_conflict_set(unsigned int player);
		
		void add_conflict(unsigned int w, unsigned int g, unsigned int val);
		void remove_conflict(unsigned int w, unsigned int g, unsigned int val);
		
		/**
		 * set the <(W, G, _)> = OLD_VAL cell to NEW_VAL and adjusts the conflicts
		 * and best eval
		 */ 
		void set_field(	unsigned int w, unsigned int g, int old_val, 
														int new_val);
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
		int calc_conflicts_in_group(	unsigned int w,	unsigned int g, 
										int old_val, 	int new_val);
		
		unsigned int calc_conflicts_player(int player);
	
		inline log4cpp::Category& logger() {
			return log4cpp::Category::getInstance("sgp");
		}

	public:
		SGP(unsigned int g, unsigned int p, unsigned int w);
		SGP(const SGP& sgp);
		//SGP& operator=(SGP&& other) = default; //move operator
		SGP& operator=(SGP& other);
	
		unsigned int g() const;
		unsigned int p() const;
		unsigned int w() const;
		std::string get_conflict_matrix_str() const; //TODO: add move semantic
		void set_init_alg(INIT_ALG alg);
		int get_eval() const;
		void init_solution();	
		void local_search(SGPTabuList& tabu, unsigned int best_eval);
};


/**
 * Base type for all solvers
 */
class SGPSolver{
	protected:
		SGP& sgp;
		SGPSolver(SGP& sgp);
	public:
		virtual ~SGPSolver() {};
};

} // NAMESPACE_SGP


#endif //_SGP_HH

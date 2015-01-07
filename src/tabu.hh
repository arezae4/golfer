#ifndef _TABU_HH
#define _TABU_HH

#include "sgp.hh"

namespace sgp{

/**
 * Tabu list implementation  
 * Based on Dotu and Hentenryck paper "Scheduling Social Golfers Locally", 2005
 */
class SGPDotuTabuList : public SGPTabuList 
{
	
  private:
	/**
	 * Elements of tabu list
	 */
	struct TabuElem{
		int a;			/* Player A */
		int b;			/* Player B */
		/* First iteration where A and B can be swapped again*/
		mutable unsigned int i;	
	
		friend bool operator == (const TabuElem& lhs, const TabuElem& rhs){
			return 	(lhs.a == rhs.a && lhs.b == rhs.b) ||
					(lhs.a == rhs.b && lhs.b == rhs.a);
		}
	};

	struct TabuElemHasher{
		unsigned int week_size;
		TabuElemHasher(unsigned int week_size);
		size_t operator()(const TabuElem& e) const;
	};

	std::vector<std::unordered_set<TabuElem, TabuElemHasher>> tlist;
	unsigned int iteration;
   	unsigned int iter_lb; /* iteration tabu lower bound */ 	
   	unsigned int iter_ub; /* iteration tabu upper bound */ 	
 
  public:
	SGPDotuTabuList(const SGP& sgp);
	SGPDotuTabuList(const SGP& sgp, unsigned int lower_tabu, 
									unsigned int upper_tabu);
	void add(unsigned int week, int player1, int player2);
	bool is_tabu(unsigned int week, int player1, int player2);
	void clear(unsigned int week);
	void clearAll();
	void incr_iteration();
	~SGPDotuTabuList();
}; 

/**
 * Base class for all tabu algorithms
 */
class SGPTabuSolver : public SGPSolver
{
	protected:
		int max_tries;
		int max_stable;
		virtual SGPTabuList& tabu_list() = 0;
		SGPTabuSolver(SGP& sgp, int max_tries, int max_stable);
		virtual ~SGPTabuSolver() {};
	public:
		void run(void);

};

class SGPDotuTabuSolver : public SGPTabuSolver
{
  private:
	SGPDotuTabuList tlist;
  protected:
	SGPTabuList& tabu_list();
  public:
	SGPDotuTabuSolver(SGP& sgp, unsigned int max_tries, 
								unsigned int stable_tries);
	SGPDotuTabuSolver(SGP& sgp, unsigned int max_tries, 	
								unsigned int stable_tries,
								unsigned int tabu_min, 	
								unsigned int tabu_max);
	~SGPDotuTabuSolver() {};
};

} // NAMESPACE_SGP

#endif

#include "tabu.hh"
#include <stdexcept>
#include <functional>
#include <ctime>
#include <log4cpp/Category.hh>

static log4cpp::Category& logger( log4cpp::Category::getInstance("tabu")); 

/**=====   SGPDotuTabuList ========**/
/**================================**/



/**=====  ::TabuElemHash =======**/
sgp::SGPDotuTabuList::TabuElemHasher::TabuElemHasher(unsigned int week_size)
	: week_size(week_size)
{}

/**-------------------------------**/
size_t 
sgp::SGPDotuTabuList::TabuElemHasher::operator()(const TabuElem& e) const
{
	std::hash<int> hs;
	return hs( e.a <= e.b ? e.a * week_size + e.b : e.b * week_size + e.a);
}

/**=====  END ::TabuElemHash ==**/

/**-------------------------------**/
sgp::SGPDotuTabuList::SGPDotuTabuList(const SGP& sgp) 
	: 	SGPDotuTabuList(sgp, 4, 100)
{}

/**-------------------------------**/
sgp::SGPDotuTabuList::SGPDotuTabuList(	const SGP& sgp, 
										unsigned int lower_tabu,
										unsigned int upper_tabu) 
	: 	tlist(	sgp.w(),
				std::unordered_set<TabuElem, TabuElemHasher>(
										10,
										TabuElemHasher( sgp.g() * sgp.p() )
										)
			),
		iteration(0),
		iter_lb(lower_tabu),
		iter_ub(upper_tabu)
{
	if(lower_tabu > upper_tabu)
		throw std::invalid_argument("Lower tabu < upper tabu");
}


/**-------------------------------**/
void sgp::SGPDotuTabuList::add(unsigned int week, int player1, int player2) 
{
	if(week >= tlist.size() ){
		throw std::domain_error("Invalid week in tabu list");
	}
	
	/* t in [lb, ub]*/
	unsigned int iter_val = sgp::gen_rand(iter_ub - iter_lb) + iter_lb; 
	iter_val += iteration;
	auto result = 
		tlist[week].insert(
				std::move( TabuElem {player1, player2, iter_val}) 
				);
	
	if(!result.second){
		result.first->i = iter_val;
	}
}

/**-------------------------------**/
bool sgp::SGPDotuTabuList::is_tabu(unsigned int week, int player1, int player2)
{
	if(week >= tlist.size() ){
		throw std::domain_error("Invalid week in tabu list");
	}
	TabuElem elem {player1, player2, 0}; 

	auto iter = tlist[week].find(elem);	
	return 	iter != tlist[week].end() &&
			iteration < iter->i ;
}

/**-------------------------------**/
void sgp::SGPDotuTabuList::clear(unsigned int week)
{
	if(week >= tlist.size() ){
		throw std::domain_error("Invalid week in tabu list");
	}

	tlist[week].clear();	
}

/**-------------------------------**/
void sgp::SGPDotuTabuList::clearAll()
{
	for(auto &iter : tlist)
		iter.clear();	
}

/**-------------------------------**/
sgp::SGPDotuTabuList::~SGPDotuTabuList()
{}


/**-------------------------------**/
void sgp::SGPDotuTabuList::incr_iteration()
{
	iteration++;
}

/**================================**/
/**=========  SGPTabuSolver =======**/
/**================================**/
sgp::SGPTabuSolver::SGPTabuSolver(SGP& sgp, int max_tries, int max_stable)
	: 	sgp(sgp),
		runtime_(0),
		iterations_(0),
   		max_tries(max_tries),
		max_stable(max_stable)	
{}

/**-------------------------------**/
void sgp::SGPTabuSolver::run(void) 
{
	/* clear tabu list for all weeks */
	tabu_list().clearAll();
	
	std::clock_t start = std::clock(); /* START TIME */
	sgp.init_solution();

	int tries = 0;
	int stable_tries = 0;
	SGP best_sgp(sgp);

	while(tries < max_tries)
	{
		if(sgp.get_eval() == 0){
			break;
		}
		
		sgp.local_search(tabu_list(), best_sgp.get_eval());
	

		if(sgp.get_eval() < best_sgp.get_eval()){
			best_sgp = sgp;
			logger.info("New best sgp (%d) found", sgp.get_eval() );
			stable_tries = 0;
		}else if(stable_tries > max_stable){
			tabu_list().clearAll();
			stable_tries = 0;
			sgp.init_solution();
			//std::cout << sgp << std::endl;
			logger.info("MAX STABLE REACHED: iterations: %d", tries);
			logger.info("Restarted eval: %d", sgp.get_eval());
			logger.info("Best eval: %d",best_sgp.get_eval());
		}else{
			stable_tries++;
		}	

		tries++;
		tabu_list().incr_iteration();
	}

	this->runtime_ = 	(std::clock() - start ) / 
						static_cast<double>(CLOCKS_PER_SEC);
	this->iterations_ = tries;
	sgp = best_sgp;
}

/**-------------------------------**/
double sgp::SGPTabuSolver::runtime(){ return runtime_; }
/**-------------------------------**/
unsigned int sgp::SGPTabuSolver::iterations(){ return iterations_; }
/**-------------------------------**/


/**================================**/
/**=====  SGPDotuTabuSolver =======**/
/**================================**/
sgp::SGPDotuTabuSolver::SGPDotuTabuSolver( 	SGP& sgp, 
											unsigned int max_tries, 
											unsigned int max_stable)
	: 	SGPTabuSolver(sgp, max_tries, max_stable),
		tlist(sgp, 4, 100)	
{}

sgp::SGPDotuTabuSolver::SGPDotuTabuSolver( 	SGP& sgp, 
											unsigned int max_tries, 
											unsigned int max_stable,
											unsigned int tabu_min, 
											unsigned int tabu_max)
	: 	SGPTabuSolver(sgp, max_tries, max_stable),
		tlist(sgp, tabu_min, tabu_max)	
{}


/**-------------------------------**/
sgp::SGPTabuList& sgp::SGPDotuTabuSolver::tabu_list(){
	return tlist;
}
/**-------------------------------**/


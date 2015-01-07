#include "tabu.hh"
#include <stdexcept>
#include <functional>

/**=====   SGPDotuTabuList ========**/
/**================================**/



/**=====  ::TabuElemHash =======**/
sgp::SGPDotuTabuList::TabuElemHasher::TabuElemHasher(unsigned int week_size)
	: week_size(week_size)
{}

size_t sgp::SGPDotuTabuList::TabuElemHasher::operator()(const TabuElem& e) const
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
	: 	SGPSolver(sgp),
   		max_tries(max_tries),
		max_stable(max_stable)	
{}

/**-------------------------------**/
void sgp::SGPTabuSolver::run(void) 
{
	/* clear tabu list for all weeks */
	tabu_list().clearAll();
	sgp.init_solution();

	std::cout << sgp << std::endl;

	int tries = 0;
	int stable_tries = 0;
	SGP best_sgp(sgp);

	while(tries < max_tries)
	{
		if(sgp.get_eval() == 0){
			return;
		}
		
		int eval = sgp.get_eval();
		sgp.local_search(tabu_list(), best_sgp.get_eval());
	

		if(sgp.get_eval() < best_sgp.get_eval()){
			best_sgp = sgp;
			std::cout << "New best sgp (%d) found" 	<< sgp.get_eval() 
													<< std::endl;
			stable_tries = 0;
		}else if(stable_tries > max_stable){
			tabu_list().clearAll();
			stable_tries = 0;
			sgp.init_solution();
			//std::cout << sgp << std::endl;
			std::cout << "MAX STABLE REACHED" << std::endl;
			std::cout << "Restarted eval: " << sgp.get_eval() << std::endl;
			std::cout << "Best eval: " << best_sgp.get_eval() << std::endl;
		}else{
			//assert( sgp.get_eval() >= eval);
			/*std::cout << "old eval = " << eval << "new eval: " 
												<< sgp.get_eval()<< std::endl
												<< " best eval: "
												<< best_sgp.get_eval()<< std::endl;
			*/
			stable_tries++;
		}	

		tries++;
		tabu_list().incr_iteration();
	}

	sgp = best_sgp;
}

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
											unsigned int tabu_min, unsigned int tabu_max)
	: 	SGPTabuSolver(sgp, max_tries, max_stable),
		tlist(sgp, tabu_min, tabu_max)	
{}


/**-------------------------------**/
sgp::SGPTabuList& sgp::SGPDotuTabuSolver::tabu_list(){
	return tlist;
}

/**-------------------------------**/


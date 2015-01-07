#include "sgp.hh"
#include <sstream>
#include <iomanip>
#include <random>
#include <cassert>
#include <algorithm>
#include <utility>
#include <ctime>


/**-----------------------------------------------------------------------**/
sgp::decision::decision(unsigned int w, unsigned int g, 
										unsigned int val)
	: w(w), g(g), val(val), erased(false) 
{}
/**-----------------------------------------------------------------------**/
namespace sgp{

	bool operator < (const decision& lhs, const decision& rhs)
	{
		return 	lhs.w < rhs.w ||
				(lhs.w == rhs.w && lhs.g < rhs.g ) ||
				(lhs.w == rhs.w && lhs.g == rhs.g && lhs.val < rhs.val) ; 
	}

	bool operator == (const decision& lhs, const decision& rhs)
	{
		return 	lhs.w == rhs.w &&
				lhs.g == rhs.g &&
				lhs.val == rhs.val ; 
	}

}

/**-----------------------------------------------------------------------**/
sgp::decisionHash::decisionHash(int g, int p, int w):
	g(g), p(p), w(w) {}

std::size_t sgp::decisionHash::operator() (const sgp::decision& d) const{
	return d.w * ( g * p ) + d.g * p + d.val; 
}


/**-----------------------------------------------------------------------**/
sgp::SGP::SGP(unsigned int g, unsigned int p, unsigned int w) : 
	_g(g) , _p(p), _w(w),
	_tables(_w , 
			std::vector<std::set<int>>(_g , std::set<int>())
			),
	_groups(_w , std::vector<int>(_g * _p, -1)),
	_conflict_matrix( _g * _p , std::vector<int>(_g * _p , 0) ),
	is_in_conflict_set(_g * _p, false)
	/*conflict_set(w * g * p / 2, decisionHash(g,p,w))*/
{
	best_eval = 0;
	//init_solution();	
}

/**-----------------------------------------------------------------------**/
sgp::SGP& sgp::SGP::operator=(sgp::SGP& other){
	_g = other._g;
	_p = other._p;
	_w = other._w;
	best_eval = other.best_eval;
	_tables = other._tables;
	_groups = other._groups;
	_conflict_matrix = other._conflict_matrix;
	_init_alg = other._init_alg;
	is_in_conflict_set= other.is_in_conflict_set;

	return *this;
}

/**-----------------------------------------------------------------------**/
sgp::SGP::SGP(const SGP& sgp)
	: 	_g(sgp._g), _p(sgp._p), _w(sgp._w),
		best_eval(sgp.best_eval),
		_tables(sgp._tables),
		_groups(sgp._groups),
		_conflict_matrix(sgp._conflict_matrix),
		is_in_conflict_set(sgp.is_in_conflict_set),
		_init_alg(sgp._init_alg),
		conflict_set(sgp.conflict_set)
{}

/**-----------------------------------------------------------------------**/
unsigned int sgp::SGP::p() const {	return _p;	}

/**-----------------------------------------------------------------------**/
 unsigned int sgp::SGP::g() const {	return _g;	}

/**-----------------------------------------------------------------------**/
 unsigned int sgp::SGP::w() const {	return _w;	}

/**-----------------------------------------------------------------------**/
namespace sgp
{
std::ostream& operator<<(std::ostream& os , const sgp::SGP& sgp)
{

	int week_in_row = 4;
	int week_rows = sgp._w / week_in_row;
	int wrow_cnt = 0;
	int week_title_cnt = 1;
	int week_table_cnt = 0;

	
	while(wrow_cnt <= week_rows){
		
		if(wrow_cnt == week_rows){ 	//We have reached the last row
			week_in_row = sgp._w % week_in_row; 
		}
		for( int wir = 0 ; wir < week_in_row ; wir++ ){
			os 	<< std::internal 
				<< std::setw(sgp._p * 3 - 1) 
				<< "WEEK " << week_title_cnt++;
			os << std::setw(8) << "";
		}

		os << std::right ;
		os << std::endl;

		for(unsigned  int gr = 0 ; gr < sgp._g ; gr++){
			for( int wir = 0 ; wir < week_in_row ; wir++ ){
				for(const int &p : sgp._tables[week_table_cnt + wir][gr]){
						os 	<< std::setw(3) 
							<< p;
				}
				os << std::setw(8) << "";  // group seperator
			}
			os << std::endl;
		}

		os << std::endl;

		wrow_cnt++;
		week_table_cnt += week_in_row;
	}

	os << "CURRENT EVAL = " << sgp.best_eval << std::endl;
	os 	<< "CONFLICT MATRIX : " << std::endl 
		<< sgp.get_conflict_matrix_str() 
		<< std::endl;
	return os;
}
}

/**-----------------------------------------------------------------------**/
std::string sgp::SGP::get_conflict_matrix_str() const
{
	std::stringstream ss;

	ss << std::right;
	ss << "   ";
	for(unsigned  int j = 0; j < _g*_p; ++j){
		ss <<  std::setw(3) << j ;
	}

	ss << std::endl;	
	ss << "   ";
	ss.width( _p * _g  * 3 );
	ss.fill('-');
	ss << "-" << std::endl;
	ss.fill(' ');

	for(unsigned int i = 0 ; i < _g*_p ; ++i){
		ss << std::setw(2) << i << "|" ; 
		for(unsigned  int j = 0; j < _g*_p; ++j){
			int c = _conflict_matrix[i][j];
			ss << std::setw(3) << c ;
		}
		ss << std::endl;
	}

	return ss.str();
}

/**------------------------------------------------------------------------**/
void sgp::SGP::set_init_alg(sgp::INIT_ALG alg){
	this->_init_alg = alg;
}

/**------------------------------------------------------------------------**/
void sgp::SGP::init_solution()
{


	this->best_eval = 0;
	conflict_set.clear();
	is_in_conflict_set = std::vector<bool>(_g * _p, false);
	_groups = std::vector<std::vector<int>>(_w , std::vector<int>(_g * _p, -1));
	switch (_init_alg){
	
		case sgp::INIT_ALG::RANDOM:

			std::vector<int> vals;
			for(unsigned int i = 0 ; i < _g * _p; ++i) 
			{
				for(unsigned int j = 0 ; j < _g * _p; ++j){
				   _conflict_matrix[i][j] = _conflict_matrix[j][i] = 0;	
				}
				vals.push_back(i);
			}

			for(unsigned int w = 0 ; w < _w ; w++){
			
				std::vector<int> to_be_inserted_vals(vals);
				for(unsigned int i = 0 ; i < _g ; i++){
					
					//clear the group set
					_tables[w][i].clear();
					for(unsigned int j = 0 ; j < _p ; j++){
						int new_pos = gen_rand(to_be_inserted_vals.size());	
						set_field(w, i , UNSET, to_be_inserted_vals[new_pos]);
						to_be_inserted_vals.erase(
								to_be_inserted_vals.begin() + new_pos);	
					}
				}
				assert(to_be_inserted_vals.size() == 0 );
			}

			//this->best_eval = 0;
			//set_all_conflicts();
			break;			

	}
}

/**------------------------------------------------------------------------**/
void sgp::SGP::inc_conflict(unsigned int p1, unsigned int p2)
{
	assert(p1 < _p *_g && p2 < _p * _g);
	_conflict_matrix[p1][p2] = ++_conflict_matrix[p2][p1];
	if(_conflict_matrix[p1][p2] > 1){
		best_eval++;
	}
}
/**------------------------------------------------------------------------**/
void sgp::SGP::dec_conflict(unsigned int p1, unsigned int p2)
{
	assert(p1 < _p *_g && p2 < _p * _g);
	if(_conflict_matrix[p1][p2] > 1){
		best_eval--;
	}
	_conflict_matrix[p1][p2] = --_conflict_matrix[p2][p1];
}

/**------------------------------------------------------------------------**/
void sgp::SGP::update_conflict_set(unsigned int player)
{
	assert(player < _p *_g );
	unsigned int conf = calc_conflicts_player(player);
	if( conf > 0 /*&& !is_in_conflict_set[player]*/){
		for(unsigned int w = 0; w < _w; w++)
		{
			const int &g = _groups[w][player];
			if(g == -1)
				continue;
			add_conflict(w, g, player);
			is_in_conflict_set[player] = true;
		}	
	}else if( conf == 0 /*&& is_in_conflict_set[player]*/){
		for(unsigned int w = 0; w < _w; w++)
		{
			const int &g = _groups[w][player];
			if(g == -1)
				continue;
			remove_conflict(w, g, player);
			is_in_conflict_set[player] = false;
		}
	}
}

/**------------------------------------------------------------------------**/
void sgp::SGP::add_conflict(unsigned int w, unsigned int g, unsigned int val){

	assert(w < _w && g < _g);
	decision elem(w, g, val);
	auto iter = conflict_set.get<0>().insert(conflict_set.end(),std::move( elem ));
	iter->erased = false;
}
/**------------------------------------------------------------------------**/
void sgp::SGP::remove_conflict(unsigned int w, 	unsigned int g, 
												unsigned int val){

	decision elem(w, g, val);
	auto iter = conflict_set.find(std::move( elem ));
	if(iter != conflict_set.end()){
		iter->erased = true;
	}
}
/**------------------------------------------------------------------------**/
void sgp::SGP::set_field(	unsigned int w,	
							unsigned int g,	
							int old_val, 
							int new_val)
{

	if(w >= _w)
		throw std::domain_error("invalid week");
	if(g >= _g)
		throw std::domain_error("invalid group");

	if(old_val == new_val){
		return;
	}

	if(old_val == UNSET){
		//we expect a new insertion
		logger().debug("<%d, %d> .size= %d", w, g, _tables[w][g].size());
		assert(_tables[w][g].size() < _p );
	}else{
		assert(_tables[w][g].size() == _p);
	}

	for(const int& p2 : _tables[w][g] ){

		if(p2 == UNSET)
			continue;

		if(old_val != UNSET){
			if(p2 == old_val)
				continue;
			//reduce conflict
			dec_conflict(old_val, p2);
		}
		
		//increase conflict
		inc_conflict(new_val,p2);

		update_conflict_set(p2);
			
	}//END_FOR

	
	_tables[w][g].insert(std::move(new_val));
	_groups[w][new_val] = g;
	update_conflict_set(new_val);

	if(old_val != UNSET){	
	
		remove_conflict(w, g, old_val);	

		size_t cnt = _tables[w][g].erase(std::move(old_val));
		if(cnt == 0){
			std::cout << "FALSE INSERTION: in " << w <<" " << g << " " << 
																" " << old_val 
																<< "with " 
																<< new_val 
																<< std::endl;
			std::cout << *this;
		}
		assert(cnt == 1);

		_groups[w][old_val] = -1;
		update_conflict_set(old_val);
	}

}

/**------------------------------------------------------------------------**/
double sgp::gen_rand()
{
	
	static std::random_device rdev{};
	static std::default_random_engine e(rdev());
	static std::uniform_real_distribution<double> d(0.0, 1.0);

	return d(e);
}

/**------------------------------------------------------------------------**/
unsigned int sgp::gen_rand(unsigned int n)
{
	return static_cast<unsigned int>(gen_rand() * n);
}

/**------------------------------------------------------------------------**/
int sgp::SGP::calc_conflicts_in_group(	unsigned int w,	unsigned int g, 
														int old_val, 
														int new_val)
{
			
	if(old_val == new_val)
		return 0;
	
	int res = 0;
	for(const int& p2 : _tables[w][g]){
		
		if(p2 != old_val){
			if(_conflict_matrix[old_val][p2] > 1){  
				res--;		
			}
		
			if(_conflict_matrix[new_val][p2] >= 1){  
				res++;
			}
		}
	}

	return res;	
}


/**------------------------------------------------------------------------**/
unsigned int sgp::SGP::calc_conflicts_player(int player)
{
	return accumulate(	_conflict_matrix[player].begin(), 
						_conflict_matrix[player].end() , 
						0,
						[](int x, int y)
								{return x + std::max(y - 1, 0);} 
					);
}

/**------------------------------------------------------------------------**/
int sgp::SGP::get_eval() const
{
	return best_eval;
}

/**------------------------------------------------------------------------**/
void sgp::SGP::local_search(SGPTabuList& tabu, unsigned int best_eval)
{
	logger().debug("check--");
	if(this->best_eval == 0){
		return;
	}
	// best difference found so far
	int best_diff = best_eval - this->best_eval;	
		
	for(	auto s1 = conflict_set.begin() ; 
				s1 != conflict_set.end() ; 
				s1++ )
	{

		/* Clean the list from Decisions whose conflicts is 0 */
		while( 	 s1->erased ){
			logger().debug("erased (%d, %d, %d)", s1->w , s1->g, s1->val);
			s1 = conflict_set.erase(s1);
			if(s1 == conflict_set.end())
				return;
		}
		
		decision chosed_decision = *s1;	// stores the decision value 
										// corresponding to best_diff
		int chosed_diff = INT_MAX;		// best NON_TABU difference 
										//chosen so far

		logger().debug("----------------------------------");
		logger().debug("selected conflict <%d, %d, [%d]>", s1->w, s1->g, s1->val);
		for(unsigned int g = 0 ; g < _g; g++ )
		{		
			if(g == s1->g )
				continue;		// skip s1's group
			for(const int &new_val : _tables[s1->w][g])
			{		
			
				//std::cin.get();	
				logger().debug("\ninspecting <%d, %d,[%d]", s1->w , g, new_val);
				int g1_diff = calc_conflicts_in_group( 	s1->w, 	s1->g, 
																s1->val, 
																new_val);	
				logger().debug("g1_diff = %d", g1_diff);
				int g2_diff = calc_conflicts_in_group( s1->w, g, new_val, s1->val);	
				logger().debug("g2_diff = %d", g2_diff);

				int diff = g1_diff + g2_diff;
				logger().debug("diff = %d", diff);

				if(	diff <= chosed_diff &&
					!tabu.is_tabu( s1->w, s1->val, new_val ) )
				{
					logger().debug("found non-tabu %d, %d, %d",
										s1->w, g, new_val);
					chosed_diff = diff;
					chosed_decision.g = g;
					chosed_decision.val = new_val;
					if(chosed_diff < best_diff){
						best_diff = chosed_diff;
					}
				}
				else if( diff < best_diff) /** update aspiration */
				{
					best_diff = diff;
					chosed_diff = diff;
					chosed_decision.g = g;
					chosed_decision.val = new_val;
					logger().debug("found aspiration %d, %d, [%d]", 
														chosed_decision.w,
														chosed_decision.g,
														chosed_decision.val);
				}
				
				logger().debug("chosed_diff is %d", chosed_diff);
				logger().debug("best_diff is %d", best_diff);
				logger().debug("best_eval is %d", this->best_eval);
			}
		}

		//assert(chosed_decision.g != s1->g && chosed_decision.val != s1->val);

		if(chosed_diff == INT_MAX){
			continue;
		}

		/* Add to tabu list*/
		tabu.add(chosed_decision.w, s1->val , chosed_decision.val); 
		//swap the values
		logger().debug("swapped (%d, %d [%d])with (%d, %d, [%d] )", 
					s1->w, s1->g, s1->val, 
					chosed_decision.w , 
					chosed_decision.g, chosed_decision.val);
			
		int eval_before = this->best_eval;
		set_field(s1->w, s1->g, s1->val, chosed_decision.val);
		set_field(	chosed_decision.w, chosed_decision.g, 
										chosed_decision.val,
			   							s1->val);
		int eval_after = this->best_eval;
	
		if(chosed_diff != eval_after - eval_before){
			std::cout << *this;
		}
		assert( chosed_diff == (eval_after - eval_before));	
		best_diff -= (eval_after - eval_before);
		logger().debug("eval_before is %d", eval_before);
		logger().debug("eval_after is %d", eval_after);
		logger().debug("best_diff is %d", best_diff);

	}

	logger().debug("CONFLICT list includes %d members: ", conflict_set.size());
	/*std::for_each(	conflict_set.begin(), conflict_set.end() , 
					[&] (const decision d) 
						{ logger().debug(" (%d, %d, %d) ", d.w, d.g, d.p); });
	*/
}

/** =================== SGPSolver =================== **/

sgp::SGPSolver::SGPSolver(SGP& sgp)
	: sgp(sgp) {}
	

/*
void sgp::SGPSolver::solve_local_search_with_lns(int iteration, int timeout){
	
	_best_eval = INT_MAX;
	int size = 20;
	SGP local_candidate(_g, _p, _w);
	int tries = 0;

	logger.debug("STARTING LOCAL-SEARCH-WITH-LNS WITH #ITER= %d, TIMEOUT= %d", iteration, timeout);
	std::clock_t start = std::clock();
	while ( _best_eval > 0 
			&& ( (std::clock() - start ) * 1000 / static_cast<double>( CLOCKS_PER_SEC )) < timeout && tries < iteration ){
	
		local_candidate.run_local_search();
		logger.debug("FOUND NEW BEST EVAL WITH LOCAL SEARCH = %d", local_candidate.get_eval());
		
		if(local_candidate.get_eval() > 0){
			while(true ){
				SGPPartialSolution psol(local_candidate, size , sgp::Sel_alg::RANDOM_CONFLICT);
				if(psol.find_first_solution()){

					logger.debug("FOUND NEW SOLUTION WITH LNS: = %d", psol.get_eval());
					break;
				}
			}
		}
		if(local_candidate.get_eval() < _best_eval){
			_best_eval = local_candidate.get_eval();
			best_sgp = std::move(local_candidate);
			logger.debug("FOUND NEW BEST EVAL = %d", _best_eval);
		}

		tries++;
	}

}

const sgp::SGP& sgp::SGPSolver::best_solution(){
	return best_sgp;
}

int sgp::SGPSolver::best_eval(){
	return _best_eval;

}

*/

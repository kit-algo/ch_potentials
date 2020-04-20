#include <routingkit/timestamp_flag.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/id_queue.h>
#include <routingkit/timer.h>
#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/graph_util.h>
#include <routingkit/dijkstra.h>

#include <iostream>
#include <string>
#include <vector>

using namespace RoutingKit;
using namespace std;

std::vector<unsigned>compute_pseudo_dfs_node_order(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head){
        unsigned node_count = first_out.size()-1;
        std::vector<bool>was_pushed(node_count, false);
        std::vector<unsigned>order(node_count);
        unsigned order_end = 0;
        std::vector<unsigned>stack(node_count);
        unsigned stack_end = 0;
        for(unsigned s=0; s<node_count; ++s){
                if(!was_pushed[s]){
                        stack[stack_end++] = s;
                        was_pushed[s] = true;

                        while(stack_end != 0){
                                unsigned x = stack[--stack_end];
                                order[order_end++] = x;
                                for(unsigned xy=first_out[x]; xy!=first_out[x+1]; ++xy){
                                        unsigned y = head[x];
                                        if(!was_pushed[y]){
                                                stack[stack_end++] = y;
                                                was_pushed[y] = true;                                        
                                        }
                                }
                        }
                }
        }
        return order;
}

struct ZeroPot{
        unsigned eval(unsigned source_node){
                return 0;
        }

        void set_target(unsigned target_node){

        }

        void preprocess(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head, const std::vector<unsigned>&lower_bound_weight, const ContractionHierarchy&ch){
                
        }
};

struct PotUsingCHQuery{
        void preprocess(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head, const std::vector<unsigned>&lower_bound_weight, const ContractionHierarchy&ch){
                ch_query.reset(ch);
        }

        void set_target(unsigned target_node){
                this->target_node = target_node;
        }

        unsigned eval(unsigned source_node){
                return ch_query.reset().add_source(source_node).add_target(target_node).run().get_distance();
        }

        ContractionHierarchyQuery ch_query;
        unsigned target_node;
};


struct PotUsingCHManyToOneQuery{
        void preprocess(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head, const std::vector<unsigned>&lower_bound_weight, const ContractionHierarchy&ch){
                ch_query.reset(ch);
                pot.resize(node_count);
                for(unsigned i=0; i<node_count; ++i)
                        pot[i] = i;
                ch_query.pin_sources(pot);
        }

        void set_target(unsigned target_node){
                ch_query.reset_target().add_target(target_node).run_to_pinned_sources().get_distances_to_sources(pot.data());
        }

        unsigned eval(unsigned source_node){
                return pot[source_node];
        }

        ContractionHierarchyQuery ch_query;
        std::vector<unsigned>pot;
};

struct QueryWeight{

        QueryWeight(const std::vector<unsigned>&lower_bound_weight, unsigned percent_extra):
                lower_bound_weight(lower_bound_weight), percent(percent_extra+100){}

        unsigned eval(unsigned arc)const{
                if(lower_bound_weight[arc] < inf_weight)
                        return static_cast<uint64_t>(lower_bound_weight[arc])*percent / 100;
                else
                        return inf_weight;
        }

        const std::vector<unsigned>&lower_bound_weight;
        unsigned percent;
};

struct CHPot{
        const ContractionHierarchy*ch;
        std::vector<unsigned>tentative_distance;
        TimestampFlags was_pot_computed, was_pushed;
        MinIDQueue queue;
        #ifndef NDEBUG
        ContractionHierarchyQuery ch_query;
        unsigned target_node;
        #endif        

        void preprocess(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head, const std::vector<unsigned>&lower_bound_weight, const ContractionHierarchy&ch){
                tentative_distance.resize(node_count);
                was_pushed = TimestampFlags(node_count);
                was_pot_computed = TimestampFlags(node_count);
                queue = MinIDQueue(node_count);
                this->ch = &ch;
                #ifndef NDEBUG
                ch_query.reset(ch);
                #endif
        }

        void set_target(unsigned target_node){
                #ifndef NDEBUG
                this->target_node = target_node;
                #endif

                unsigned t = ch->rank[target_node];

                was_pushed.reset_all();
                queue.clear();
                queue.push({t, 0});
                was_pushed.set(t);
                tentative_distance[t] = 0;
                
                #ifndef NDEBUG
                unsigned last_key = 0;
                #endif

                while(!queue.empty()){
                        auto e = queue.pop();
                        unsigned x = e.id;
                        unsigned x_dist = e.key;
                        #ifndef NDEBUG
                        assert(e.key >= last_key);
                        last_key = e.key;
                        assert(was_pushed.is_set(x));
                        unsigned correct_dist = ch_query.reset().add_source(ch->order[x]).add_target(target_node).run().get_distance();
                        assert(correct_dist <= x_dist);
                        #endif
                        for(unsigned xy = ch->backward.first_out[x]; xy < ch->backward.first_out[x+1]; ++xy){
                                unsigned xy_dist = ch->backward.weight[xy];
                                if(xy_dist < inf_weight){
                                        unsigned y = ch->backward.head[xy];
                                        unsigned y_dist = x_dist + xy_dist;

                                        #ifndef NDEBUG
                                        unsigned correct_y_dist = ch_query.reset().add_source(ch->order[y]).add_target(target_node).run().get_distance();
                                        unsigned correct_x_dist = ch_query.reset().add_source(ch->order[x]).add_target(target_node).run().get_distance();
                                        assert(correct_y_dist <= y_dist);
                                        #endif

                                        if(!was_pushed.is_set(y)){
                                                tentative_distance[y] = y_dist;
                                                was_pushed.set(y);
                                                queue.push({y, y_dist});
                                        }else if(tentative_distance[y] > y_dist){
                                                tentative_distance[y] = y_dist;
                                                if(queue.contains_id(y)){
                                                        queue.decrease_key({y, y_dist});
                                                }
                                        }
                                }
                        }
                }

                was_pot_computed.reset_all();
        }

private:
        unsigned eval_using_ch_node_order(unsigned x){
                if(!was_pot_computed.is_set(x)){
                        unsigned x_dist;
                        if(was_pushed.is_set(x))
                                x_dist = tentative_distance[x];
                        else
                                x_dist = inf_weight;

                        for(unsigned xy = ch->forward.first_out[x]; xy < ch->forward.first_out[x+1]; ++xy){
                                unsigned xy_dist = ch->forward.weight[xy];
                                unsigned y = ch->forward.head[xy];
                                unsigned y_dist = eval_using_ch_node_order(y);
                                unsigned d = xy_dist + y_dist;
                                if(d < x_dist)
                                        x_dist = d; 
                        }
                        tentative_distance[x] = x_dist;
                        was_pot_computed.set(x);
                }
                return tentative_distance[x];
        }
public:

        unsigned eval(unsigned source_node){
                unsigned x_pot = eval_using_ch_node_order(ch->rank[source_node]);
                #ifndef NDEBUG
                unsigned correct_dist = ch_query.reset().add_source(source_node).add_target(target_node).run().get_distance();
                assert(correct_dist == x_pot);
                #endif

                return x_pot;
        }


};

template<class QueryWeight, class Potential>
struct AStar{
        const std::vector<unsigned>&first_out;
        const std::vector<unsigned>&head;
        const QueryWeight&query_weight;
        Potential&pot;
        MinIDQueue queue;
        std::vector<unsigned>tentative_distance;
        TimestampFlags was_pushed;

        AStar(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, const QueryWeight&query_weight, Potential&pot):
                first_out(first_out), head(head), 
                query_weight(query_weight), pot(pot),
                queue(first_out.size()-1),
                tentative_distance(first_out.size()-1, inf_weight),
                was_pushed(first_out.size()-1){}

        unsigned run(unsigned source_node, unsigned target_node){
                was_pushed.reset_all();
                queue.clear();
                tentative_distance[source_node] = 0;
                queue.push({source_node, pot.eval(source_node)});
                was_pushed.set(source_node);
                
                #ifndef NDEBUG
                unsigned last_key = 0;
                #endif

                while(!queue.empty()){
                        auto e = queue.pop();
                        #ifndef NDEBUG
                        assert(e.key >= last_key);
                        last_key = e.key;
                        assert(was_pushed.is_set(e.id));
                        assert(e.key == pot.eval(e.id) + tentative_distance[e.id]);
                        #endif
                        unsigned x = e.id;
                        unsigned x_dist = tentative_distance[x];
                        if(x == target_node)
                                break;
                        for(unsigned xy=first_out[x]; xy<first_out[x+1]; ++xy){
                                unsigned y = head[xy];
                                unsigned xy_dist = query_weight.eval(xy);
                                if(xy_dist < inf_weight){
                                        unsigned y_pot = pot.eval(y);
                                        unsigned y_dist = x_dist + xy_dist;

                                        if(was_pushed.is_set(y)){
                                                if(tentative_distance[y] > y_dist){
                                                        queue.decrease_key({y, y_dist+y_pot});
                                                        tentative_distance[y] = y_dist;
                                                }
                                        }else{
                                                was_pushed.set(y);
                                                tentative_distance[y] = y_dist;
                                                queue.push({y, y_dist+y_pot});
                                        }
                                }
                        }
                }

                if(was_pushed.is_set(target_node))
                        return tentative_distance[target_node] ;
                else
                        return inf_weight;
        }
};

template<class Potential, class QueryWeight>
void test_astar(const char*name, const std::vector<unsigned>&first_out, const std::vector<unsigned>&tail, const std::vector<unsigned>&head, const std::vector<unsigned>&lower_bound_weight, const QueryWeight&query_weight, const std::vector<unsigned>&source, const std::vector<unsigned>&target, const std::vector<unsigned>&ref_dist, const ContractionHierarchy&ch){
        unsigned node_count = first_out.size()-1;
        unsigned query_count = source.size();
        unsigned arc_count = tail.size();
        

        cout << "Check whether lower bound weight property is correct" << endl;
        for(unsigned i=0; i<arc_count; ++i)
                assert(lower_bound_weight[i] <= query_weight.eval(i));

        Potential pot;

        cout << "Preprocess "<<name << endl;
        long long preproc_timer = -get_micro_time();
        pot.preprocess(node_count, tail, head, lower_bound_weight, ch);
        preproc_timer += get_micro_time();

        cout << "Preprocess time : "<< preproc_timer << " musec"<<endl;
        
        /*cout << "Check whether potential is consistent" << endl;
        for(unsigned q=0; q<query_count; ++q){
                pot.set_target(target[q]);
                for(unsigned i=0; i<arc_count; ++i){
                        assert(pot.eval(tail[i]) <= query_weight.eval(i) + pot.eval(head[i]));
                }
                assert(pot.eval(target[q]) == 0);
        }*/


        AStar<QueryWeight, Potential> a_star(first_out, head, query_weight, pot);

        cout << "Start "<< name << endl;

        long long set_target_timer = 0;
        long long search_timer = 0;

        for(unsigned q=0; q<query_count; ++q){
                set_target_timer -= get_micro_time();

                pot.set_target(target[q]);

                auto t = get_micro_time();
                set_target_timer += t;
                search_timer -= t;

                unsigned result = a_star.run(source[q], target[q]);

                if(result != ref_dist[q]){
                        cout << "Query "<<q << " wrong; should be "<<ref_dist[q] << " but is "<< result << " source = "<<source[q] << " target = " << target[q] << endl;
                }

                search_timer += get_micro_time();
        }

        cout << "Avg. set target time : "<< set_target_timer/query_count<< " musec"<<endl;
        cout << "Avg. search time : " << search_timer/query_count << " musec" << endl;

        cerr << name << ',' << preproc_timer << ',' << set_target_timer/query_count << ',' << search_timer/query_count << endl;
}

void keep_only_queries_with_path(std::vector<unsigned>&source, std::vector<unsigned>&target, std::vector<unsigned>&dist){
        unsigned in=0, out=0, end=source.size();
        while(in != end){
                if(dist[in] < inf_weight){
                        dist[out] = dist[in];
                        source[out] = source[in];
                        target[out] = target[in];
                        ++out;                
                }
                ++in;
        }
        source.erase(source.begin()+out, source.end());
        target.erase(target.begin()+out, target.end());
        dist.erase(dist.begin()+out, dist.end());
}

int main(int argc, char*argv[]){
	std::vector<unsigned>tail = load_vector<unsigned>("tail");
        std::vector<unsigned>head = load_vector<unsigned>("head");
        std::vector<unsigned>first_out = load_vector<unsigned>("first_out");
        std::vector<unsigned>lower_bound_weight = load_vector<unsigned>("travel_time");

        std::vector<unsigned>source = load_vector<unsigned>("source");
        std::vector<unsigned>target = load_vector<unsigned>("target");
        
        source.resize(200);
        target.resize(source.size());

        unsigned node_count = first_out.size()-1;

        {
                cout << "Reorder nodes" << endl;
                std::vector<unsigned> node_perm = compute_pseudo_dfs_node_order(first_out, head);
                node_perm = invert_permutation(node_perm);
                inplace_apply_permutation_to_elements_of(node_perm, tail);
                inplace_apply_permutation_to_elements_of(node_perm, head);

                inplace_apply_permutation_to_elements_of(node_perm, source);
                inplace_apply_permutation_to_elements_of(node_perm, target);
                
                cout << "Reorder arcs" << endl;
                std::vector<unsigned> arc_perm = compute_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, tail, head);

                head = apply_permutation(arc_perm, head);
                lower_bound_weight = apply_permutation(arc_perm, lower_bound_weight);

                cout << "Invert tail" << endl;
                first_out = invert_vector(tail, node_count);
        }

        ContractionHierarchy ch;
        try{
                ch = ContractionHierarchy::load_file("ch");
                cout << "Loaded CH from file" << endl;
        }catch(...){
                long long timer = -get_micro_time();
                ch = ContractionHierarchy::build(node_count, tail, head, lower_bound_weight);
                timer += get_micro_time();
                cout << "Build CH : "<< timer << endl;
                ch.save_file("ch");
                cout << "Save CH to file " << endl;
        }

        unsigned arc_count = tail.size();
        unsigned query_count = source.size();

        std::vector<unsigned>ref_dist(query_count);

        QueryWeight query_weight(lower_bound_weight, 3);

        cout << "Running RoutingKit Dijkstra" << endl;

        {
                Dijkstra dij(first_out, tail, head);

                long long timer = -get_micro_time();

                for(unsigned q=0; q<query_count; ++q){
                        dij.reset().add_source(source[q]);

                        unsigned dist = inf_weight;

                        unsigned t = target[q];

                        while(!dij.is_finished()){
                                auto r = dij.settle([&](unsigned arc, unsigned){return query_weight.eval(arc);});
                                if(r.node == t){
                                        dist = r.distance;
                                        break;
                                }
                        }

                        ref_dist[q] = dist;
                }

                timer += get_micro_time();

                cout << "Avg. time : " << timer/query_count << endl;
        }

        keep_only_queries_with_path(source, target, ref_dist);
        query_count = source.size();

        cout << "Query count : " << query_count << endl;

        cout << "First reference distances:" << endl;
        for(unsigned i=0; i<10 && i<query_count; ++i){
                cout << ref_dist[i] << " ";
        }
        cout << endl;
       
        
        for(unsigned i=0; i<20; ++i){
                QueryWeight query_weight(lower_bound_weight, i);
                cerr << i << ',';
                test_astar<PotUsingCHManyToOneQuery>("many_to_one", first_out, tail, head, lower_bound_weight, query_weight, source, target, ref_dist, ch);
                cerr << i << ',';
                test_astar<CHPot>("ch_pot", first_out, tail, head, lower_bound_weight, query_weight, source, target, ref_dist, ch);
        }

    
        /*//test_astar<ZeroPot>("zero_pot", first_out, tail, head, lower_bound_weight, query_weight, source, target, ref_dist, ch);
        //test_astar<PotUsingCHQuery>("ch_query", first_out, tail, head, lower_bound_weight, query_weight, source, target, ref_dist, ch);
        test_astar<PotUsingCHManyToOneQuery>("many_to_one", first_out, tail, head, lower_bound_weight, query_weight, source, target, ref_dist, ch);
        test_astar<CHPot>("ch_pot", first_out, tail, head, lower_bound_weight, query_weight, source, target, ref_dist, ch);*/
}

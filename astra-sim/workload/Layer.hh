/******************************************************************************
Copyright (c) 2020 Georgia Institute of Technology
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Author : Saeed Rashidi (saeed.rashidi@gatech.edu)
*******************************************************************************/
#ifndef __LAYER_HH__
#define __LAYER_HH__


#include <map>
#include <math.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <tuple>
#include <cstdint>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "astra-sim/system/Sys.hh"
#include "Workload.hh"
#include "CSVWriter.hh"

namespace AstraSim{
class Layer:public Callable,public StreamStat{
public:
    std::string id;
    int layer_num;
    Sys *generator;
    Workload *workload;

    int fwd_pass_compute_time;
    ComType fwd_pass_comm_type;
    int fwd_pass_comm_size;
    Tick fwd_update_time;

    int input_grad_compute_time;
    ComType input_grad_comm_type;
    int input_grad_comm_size;
    Tick input_grad_update_time;

    int weight_grad_compute_time;
    ComType weight_grad_comm_type;
    int weight_grad_comm_size;
    Tick weight_grad_update_time;

    bool needs_fwd_in_bckwd_initiation;
    bool is_checkpoint;
    ParallelismPolicy specific_parallellism;

    int lookup_table_size;
    int collective_counter;


    //DataSet *fwd_pass_dataset;
    //DataSet *input_grad_dataset;
    //DataSet *weight_grad_dataset;

    std::map<int,DataSet*> fwd_pass_datasets;
    std::list<Tick> started_waiting_for_fwd_pass;
    std::map<int,DataSet*> input_grad_datasets;
    std::list<Tick> started_waiting_for_input_grad;
    std::map<int,DataSet*> weight_grad_datasets;
    std::list<Tick> started_waiting_for_weight_grad;


    //reports
    Tick total_forward_pass_compute;
    Tick total_input_grad_compute;
    Tick total_weight_grad_compute;
    Tick total_weight_grad_comm;
    Tick total_input_grad_comm;
    Tick total_fwd_comm;

    Tick last_fwd_finished;
    Tick last_wg_finished;
    Tick last_ig_finished;

    Tick total_waiting_for_wg_comm;
    Tick total_waiting_for_ig_comm;
    Tick total_waiting_for_fwd_comm;


    CollectiveBarrier fwd_barrier;
    CollectiveBarrier wg_barrier;
    CollectiveBarrier ig_barrier;

    Layer(std::string id,int layer_num,Sys *generator,Workload *workload,int fwd_pass_compute_time,ComType fwd_pass_comm_type,
          int fwd_pass_comm_size,int input_grad_compute_time,ComType input_grad_comm_type,int input_grad_comm_size,
          int weight_grad_compute_time,ComType weight_grad_comm_type,int weight_grad_comm_size,int weight_grad_update_time);
    void call(EventType event,CallData *mdata);
    Tick get_fwd_pass_compute();
    Tick get_input_grad_compute();
    Tick get_weight_grad_compute();
    void increment_waiting_for_wg();
    void increment_waiting_for_ig();
    void increment_waiting_for_fwd();
    bool is_fwd_pass_comm_finished();
    bool is_input_grad_comm_finished();
    bool is_weight_grad_comm_finished();
    bool is_fwd_pass_comm_finished_blocking();
    bool is_input_grad_comm_finished_blocking();
    bool is_weight_grad_comm_finished_blocking();
    LayerData report(std::string run_name,int layer_num,int total_rows,int stat_row,CSVWriter *detailed,CSVWriter *EndToEnd,double &total_compute,double &total_exposed, bool seprate_log);
    void issue_forward_pass_comm(bool local,bool vertical,bool horizontal,SchedulingPolicy pref_scheduling,
                                 CollectiveBarrier barrier);
    void issue_input_grad_comm(bool local,bool vertical,bool horizontal,SchedulingPolicy pref_scheduling,
                               CollectiveBarrier barrier);
    void issue_weight_grad_comm(bool local,bool vertical,bool horizontal,SchedulingPolicy pref_scheduling,
                                CollectiveBarrier barrier);
};
}
#endif

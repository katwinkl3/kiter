/*
 * Constraint Programming With MiniZinc
 *
 *  Created on: Dec 2021
 *      Author: katwinkl3
 */

#include <sstream>
#include <algorithm>
#include <models/Dataflow.h>
#include "cons_prog.h"
#include "../symbolic_execution.h"

std::string printFile(models::Dataflow* const dataflow){
    return "here";
}

void algorithms::scheduling::CPCompute(models::Dataflow* const dataflow, 
                                        parameters_list_t param_list) {

    std::cout << "Creating datafile data.dzn in directory" << std::endl;
    std::ofstream OutputFile("data.dzn");
    // 1. Read TDMA file 
    int max_slot = 1;
    const size_t noc_node = dataflow->getNoC().size();
    int** tdma = new int*[noc_node];
    for(size_t i = 0; i < noc_node; ++i) tdma[i] = new int[noc_node];
    if (param_list.find("TDMA") == param_list.end()) VERBOSE_ERROR("Missing tdma file under -p TDMA parameter")
    std::string tdma_file_name = param_list["TDMA"];
    std::ifstream TDMAReadFile(tdma_file_name);
    std::string line;
    std::vector<int> tdma_tmp;
    while (TDMAReadFile) {
        line = "";
        tdma_tmp.clear();
        std::getline (TDMAReadFile, line);
        std::stringstream nums_str(line);
        size_t idx = 0;
        while(nums_str.good()){
            std::string num;
            getline(nums_str, num, ',');
            if (idx % 2) tdma_tmp.push_back(stoi(num));
            idx ++;
        }
        max_slot = std::max(max_slot, tdma_tmp.back());
        tdma[tdma_tmp[0]][tdma_tmp[1]] = tdma_tmp[2]; // assumes tdma file contains all edges (except to oneself)
    }
    for(size_t i = 0; i < noc_node; ++i) tdma[i][i] = 0;
    TDMAReadFile.close();
    OutputFile << "tdma_table=[|";
    for (size_t i = 0; i < noc_node; ++i){
        for(size_t j = 0; j < noc_node; ++j){
            if(j != 0) OutputFile << ",";
            OutputFile << tdma[i][j];
        }
        OutputFile << "|\n";
    }
    OutputFile << "|];\n";
    OutputFile << "tdma_max=" << (max_slot+1) << ";\n";

    // 2. Take terminal output of symbolic execution call 
    std::array<char, 128> buffer;
    std::string file_name = dataflow->getFilename();
    std::string cmd="./Debug/bin/kiter -f "+file_name+" -a createNoC -a randomMapping -a randomRouting -a SymbolicExecutionWP";
    std::vector<int> source;
    std::vector<int> dest;
    std::map<int, std::vector<int>> dependency;
    std::vector<std::string> messages;
    std::vector<int> duration;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) VERBOSE_ERROR("Terminal read failed on popen()");
    int line_idx = 0;
    int max_msg = 0;
    while (!feof(pipe.get())) {
        line_idx ++;
        size_t idx = 0;
        int m_id = 0; 
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr){
            if (line_idx < 4) continue;
            std::stringstream term_line(buffer.data());
            std::string num;
            while(term_line.good()){
                getline(term_line, num, ',');
                if (idx == 0) {source.push_back(stoi(num)+1);}
                else if (idx == 1) {dest.push_back(stoi(num)+1);}
                else if (idx == 2) {duration.push_back(stoi(num));}
                else if (idx == 3) {
                    messages.push_back("m"+std::to_string(stoi(num)+1));
                    m_id = stoi(num);
                    max_msg = std::max(max_msg, m_id+1);
                }
                else if (idx > 4) {
                    if (dependency.find(m_id) == dependency.end()){
                        dependency.insert({m_id, {stoi(num)}});
                    } else {
                        dependency[m_id].push_back(stoi(num));
                    }
                }
                idx ++; 
            }
        }
    } 
    int max_time = (param_list.find("TIME") != param_list.end()) ? stoi(param_list["TIME"]) : 50;
    OutputFile << "max_time=" << (max_time) << ";\n";
    OutputFile << "src=[";
    for (size_t i = 0; i < source.size(); ++i){
        if(i != 0) OutputFile << ",";
        OutputFile << source[i];
    }
    OutputFile << "];\n";
    OutputFile << "dest=[";
    for (size_t i = 0; i < dest.size(); ++i){
        if(i != 0) OutputFile << ",";
        OutputFile << dest[i];
    }
    OutputFile << "];\n";
    OutputFile << "messages={";
    for (size_t i = 0; i < messages.size(); ++i){
        if(i != 0) OutputFile << ",";
        OutputFile << messages[i];
    }
    OutputFile << "};\n";
    OutputFile << "durations={";
    for (size_t i = 0; i < duration.size(); ++i){
        if(i != 0) OutputFile << ",";
        OutputFile << duration[i];
    }
    OutputFile << "};\n";
    std::vector<int> actors;
    sort(dest.begin(), dest.end());
    sort(source.begin(), source.end());
    std::set_union(source.begin(), source.end(), dest.begin(), dest.end(),
                std::back_inserter(actors));
    OutputFile << "actors={";
    for (size_t i = 0; i < actors.size(); ++i){
        if(i != 0) OutputFile << ",";
        OutputFile << "a" << actors[i];
    }
    OutputFile << "};\n";
    if (messages.size() != max_msg) VERBOSE_WARNING("Messages have gaps in between, dependency matrix not reliable")
    int** dependencies = new int*[max_msg];
    for(size_t i = 0; i < max_msg; ++i){
        dependencies[i] = new int[max_msg];
        for(size_t j = 0; j < max_msg; ++j){
            dependencies[i][j] = 0;
        }
    }
    for (auto const& v: dependency){
        for(int i : v.second) dependencies[v.first][i] = 1;
    }
    OutputFile << "dependencies=[|";
    for (size_t i = 0; i < max_msg; ++i){
        for(size_t j = 0; j < max_msg; ++j){
            if(j != 0) OutputFile << ",";
            OutputFile << dependencies[i][j];
        }
        OutputFile << "|\n";
    }
    OutputFile << "|];\n";
    OutputFile.close();

    std::cout << "Running constraint model" << std::endl;
    if (param_list.find("MODEL") == param_list.end()) VERBOSE_ERROR("Missing Minizinc model file under -p MODEL parameter")
    // if (system("minizinc") != 0) VERBOSE_ERROR("Minizinc not installed on machine");
    cmd = "minizinc --solver Gecode "+param_list["MODEL"];
    int res = system(cmd.c_str());
    // std::ifstream SampleReadFile("sample_cmd.txt");
    // while (SampleReadFile) {
    //     line = "";
    //     std::getline (SampleReadFile, line);
    //     std::stringstream tmp_str(line);
    //     while(tmp_str.good()){
    //         std::string tmp_tmp;
    //         getline(tmp_str, tmp_tmp, ',');
    //     }
    // }
    // std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    // if (!pipe) VERBOSE_ERROR("Terminal read failed on popen()");
    // int line_idx = 0;
    // while (!feof(pipe.get())) {
    //     line_idx ++;
    //     size_t idx = 0;
    //     int m_id = 0; 
    //     if (fgets(buffer.data(), 128, pipe.get()) != nullptr){
    //         if (line_idx < 4) continue;
    //         std::stringstream term_line(buffer.data());
    //         std::string num;
    //         while(term_line.good()){
    //             getline(term_line, num, ',');
    //             if (idx == 0) {source.push_back(stoi(num)+1);}
    //             else if (idx == 1) {dest.push_back(stoi(num)+1);}
    //             else if (idx == 2) {duration.push_back(stoi(num));}
    //             else if (idx == 3) {
    //                 messages.push_back("m"+std::to_string(stoi(num)+1));
    //                 m_id = stoi(num);
    //                 max_msg = std::max(max_msg, m_id+1);
    //             }
    //             else if (idx > 4) {
    //                 if (dependency.find(m_id) == dependency.end()){
    //                     dependency.insert({m_id, {stoi(num)}});
    //                 } else {
    //                     dependency[m_id].push_back(stoi(num));
    //                 }
    //             }
    //             idx ++; 
    //         }
    //     }
    // } 

    return;
}

//m_len, actors
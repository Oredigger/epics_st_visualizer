/**
 * EPICS sequencer grapher
 * Written by Andrew Wang   (main state machine and parsing)
 *            Daniel Kenner (preprocessing)
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

static void replace_all_occurrences(std::string &txt, std::string key, std::string val)
{
    int pos = 0, prevPos = 0, counter = 0;
    int key_len = key.length();
    std::string new_txt = "";

    while ((pos = txt.find(key, pos + 1)) != std::string::npos)
    {
        new_txt.append(txt.substr(prevPos, pos - prevPos));
        new_txt.append(val);
        prevPos = pos + key_len;
    }

    new_txt.append(txt.substr(prevPos, std::string::npos));
    txt = new_txt;
}

int preprocess_string_macros(std::string &txt, std::string param_fn)
{
    std::ifstream fin(param_fn);

    if (!fin.is_open())
    {
        return 1;
    }

    int status = 3;

    while (!fin.eof())
    {
        std::string line;
        getline(fin, line);
        int delim = line.find(",");

        if (delim != -1)
        {
            status = 0;
            std::string key = line.substr(0, delim);
            std::string val = line.substr(delim + 1);
            replace_all_occurrences(txt, key, val);
        }
    }

    fin.close();
    return status;
}

std::string read_seq_file(std::string seq_fn)
{
    std::ifstream seq_f;
    seq_f.open(seq_fn);
    std::string result = "";

    if (seq_f.is_open())
    {
        result = std::string((std::istreambuf_iterator<char>(seq_f) ), (std::istreambuf_iterator<char>())) ;
    }

    seq_f.close();
    return result;
}

void write_seq_file(std::string seq_fn, std::string txt)
{
    std::ofstream seq_f(seq_fn);
    seq_f << txt;
    seq_f.close();
}

typedef struct state_machine state_machine_t;
typedef struct seq_transit seq_transit_t;

struct state_machine
{
    std::string name;
    std::vector<seq_transit_t> tt;
};

struct seq_transit
{
    std::string cond;
    std::string src_state;
    std::string dest_state;
};

std::vector <state_machine_t> get_transits(std::string raw_txt)
{
    std::vector<state_machine_t> sm;
    std::vector<seq_transit_t> tt;

    enum State
    {
        GLOBAL_DECLARATIONS,
        STATE_MACHINE_FOUND,
        INSIDE_STATE_MACHINE,
        STATE_FOUND,
        INSIDE_STATE_BODY,
        TRANSIT_FOUND,
        INSIDE_TRANSIT_BODY,
    };

    std::string token, cond, src_state, state_machine;

    int  curly_nested = 0, parenthesis_nested = 0;
    bool waive_flag = false, when_flag = false, is_comment_flag = false, in_state_machine_flag = false, 
         in_state_body_flag = false, in_transit_body_flag = false;

    const std::string OP = " \r\t\n\v\f{}()=+-/*,;.[]&|^%";
    State s = GLOBAL_DECLARATIONS;

    for (int i = 1; i < raw_txt.length(); i++)
    {
        char curr_c = raw_txt[i - 1];
        char next_c = raw_txt[i];  

        if ((next_c == '*' && curr_c == '/') || (next_c == '/' && curr_c == '/'))
        {
            is_comment_flag = true;
        }
        else if ((next_c == '/' && curr_c == '*') || (is_comment_flag && next_c == '\n'))
        {
            is_comment_flag = false;
        }
        else if (!is_comment_flag)
        {
            if (!waive_flag && std::find(OP.begin(), OP.end(), curr_c) == OP.end()) 
            {
                token += curr_c;
            }
            
            if (s == STATE_FOUND)
            {
                if (in_state_body_flag && isspace(next_c))
                {
                    seq_transit_t temp;
                    temp.cond = cond;
                    temp.src_state = src_state;
                    temp.dest_state = token;

                    tt.push_back(temp);

                    cond.clear();
                    token.clear();
                    
                    s = INSIDE_STATE_BODY;
                }
                else if (next_c == '{')
                {
                    src_state = token;
                    token.clear();
                    
                    s = INSIDE_STATE_BODY;
                    in_state_body_flag = true;
                }
            }
            else if (s == INSIDE_STATE_BODY)
            {
                if (token == "state" && isspace(next_c))
                {
                    s = STATE_FOUND;
                    token.clear();
                }
                else if (isspace(next_c) || next_c == '(')
                {
                    if (token == "when")
                    {
                        when_flag = true;
                        token.clear();
                    }
                    else
                    {
                        token.clear();
                    }

                    if (when_flag && next_c == '(')
                    {
                        s = TRANSIT_FOUND;
                        waive_flag = true;
                        when_flag = false;
                        token.clear();
                    }
                }
                else if (next_c == '{')
                {
                    s = INSIDE_TRANSIT_BODY;
                    in_transit_body_flag = true;
                }
                else if (next_c == '}' && in_state_body_flag)
                {
                    s = INSIDE_STATE_MACHINE;
                    in_state_body_flag = false;
                    src_state.clear();
                }
            }
            else if (s == TRANSIT_FOUND)
            {
                if (next_c == '(')
                {
                    parenthesis_nested++;
                }
                else if (next_c == ')')
                {
                    if (!parenthesis_nested)
                    {
                        waive_flag = false;
                        s = INSIDE_STATE_BODY;
                        cond = token;
                    }
                    else
                    {
                        parenthesis_nested--;
                    }
                }

                if (next_c == '\"' || next_c == '\\')
                {
                    token += "\\";
                }

                token += next_c;
            }
            else if (s == INSIDE_TRANSIT_BODY)
            {
                if (in_transit_body_flag)
                {
                    if (next_c == '{')
                    {
                        curly_nested++;
                    }
                    if (next_c == '}')
                    {
                        if (!curly_nested)
                        {
                            in_transit_body_flag = false;
                            s = INSIDE_STATE_BODY;
                            token.clear();
                        }
                        else
                        {
                            curly_nested--;
                        }
                    }
                }
            }
            else if (s == STATE_MACHINE_FOUND)
            {
                if (next_c == '{')
                {
                    s = INSIDE_STATE_MACHINE;
                    in_state_machine_flag = true;
                    state_machine = token;
                    token.clear();
                }
            }
            else if (s == INSIDE_STATE_MACHINE)
            {
                if (isspace(next_c))
                {
                    if (token == "state")
                    {
                        s = STATE_FOUND;
                    }
                    
                    token.clear();
                }
                else if (next_c == '}' && in_state_machine_flag)
                {
                    in_state_machine_flag = false;
                    s = GLOBAL_DECLARATIONS;
                    
                    state_machine_t s;
                    s.name = state_machine;
                    s.tt = tt;
                    
                    sm.push_back(s);
                    tt.clear();
                }
            }
            else if (s == GLOBAL_DECLARATIONS)
            {
                if (isspace(next_c))
                {
                    if (token == "ss")
                    { 
                        s = STATE_MACHINE_FOUND;
                    }

                    token.clear();
                }
            }
        }
    }    

    return sm;
}

std::vector<std::string> state_names(std::vector<seq_transit_t> tt)
{
    std::vector<std::string> states;

    for (auto t : tt)
    {
        if (!std::count(states.begin(), states.end(), t.src_state))
        {
            states.push_back(t.src_state);
        }

        if (!std::count(states.begin(), states.end(), t.dest_state))
        {
            states.push_back(t.dest_state);
        }
    }

    return states;
}

int create_dot(std::vector<state_machine_t> state_machines)
{
    std::default_random_engine gen;
    std::uniform_int_distribution<int> dist(1, 16777216);
    std::ofstream fout("st.dot", std::ofstream::trunc);

    if (!fout.is_open())
    {
        return 1;
    }

    fout << "digraph\n{\n";

    for (auto sm : state_machines)
    {   
        fout << "\tsubgraph cluster" << sm.name << "\n\t{\n\t\tlabel=\"" << sm.name << "\"\n";
        std::vector<std::string> states = state_names(sm.tt);

        for (auto s : states)
        {
            fout << "\t\t" << sm.name << "_" << s << " [label=\"" << s << "\"]\n";
        }

        for (auto t : sm.tt)
        {
            int color_int = dist(gen);
            fout << "\t\t" << sm.name << "_" << t.src_state << " -> " 
                 << sm.name << "_" << t.dest_state 
                 << " [label=\"" << t.cond 
                 << "\", fontsize=\"10\", color=\"#" << std::hex << color_int << "\", fontcolor=\"#" << std::hex << color_int << "\"]\n";
        }

        fout << "\t}\n";
    }

    fout << "}";
    fout.close();

    return 0;
}

int main(void)
{
    const std::string seq_fn = "seq.st";
    std::string raw_txt = read_seq_file(seq_fn);

    if (preprocess_string_macros(raw_txt, "param.txt") > 1)
    {
        std::cout << "Could not find file\n";
        return EXIT_FAILURE;
    }
    
    std::vector<state_machine_t> state_machines = get_transits(raw_txt);
    
    if (create_dot(state_machines))
    {
        std::cout << "Could not make dot file\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}